
#include "RenderHandler.h"
#include "Bmp.h"
#include "lodepng.h"
#include "toojpeg.h"

namespace cefpdf
{

	bool _hasBeenRendered;


	RenderHandler::RenderHandler()
	{
		_hasBeenRendered = false;
	}


	// CefRenderHandler methods:
	// -------------------------------------------------------------------------
	void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = 128;
		rect.height = 128;
	}



	//returns 0 if all went ok, non-0 if error
	//output image is always given in RGBA (with alpha channel), even if it's a BMP without alpha channel
	uint8_t* bmp2png(uint8_t* bmp, int bytes_per_pixel, unsigned int w, unsigned int h)
	{
		uint32_t bytes_per_row_without_padding = w * bytes_per_pixel;
		uint32_t padding = (4 - (w * bytes_per_pixel) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;

		// FSCK, the bmp may have 3 bytes_per_pixel, but the PNG always has 4 bytes_per_pixel
		// uint8_t* image = (uint8_t*)malloc(w*h*numChannels*sizeof(uint8_t));
		uint8_t* image = (uint8_t*)malloc(w*h * 4 * sizeof(uint8_t));


		//The amount of scanline bytes is width of image times channels, with extra bytes added if needed
		//to make it a multiple of 4 bytes.
		// unsigned int scanlineBytes = w * numChannels;
		// if(scanlineBytes % 4 != 0)
		//    scanlineBytes = (scanlineBytes / 4) * 4 + 4;

		// printf("scanlineBytes: %d\n", scanlineBytes);

		// There are 3 differences between BMP and the raw image buffer for LodePNG:
		// -it's upside down
		// -it's in BGR instead of RGB format (or BRGA instead of RGBA)
		// -each scanline has padding bytes to make it a multiple of 4 if needed
		// The 2D for loop below does all these 3 conversions at once.
		for (unsigned int y = 0; y < h; y++)
		{

			for (unsigned int x = 0; x < w; x++)
			{
				//pixel start byte position in the BMP
				unsigned int bmpos = (h - y - 1) * row_size_bytes + bytes_per_pixel * x;
				//pixel start byte position in the new raw image
				unsigned int newpos = 4 * y * w + 4 * x;

				if (bytes_per_pixel == 3)
				{
					image[newpos + 0] = bmp[bmpos + 2]; //R
					image[newpos + 1] = bmp[bmpos + 1]; //G
					image[newpos + 2] = bmp[bmpos + 0]; //B
					image[newpos + 3] = 255;            //A
				}
				else
				{
					image[newpos + 0] = bmp[bmpos + 3]; //R
					image[newpos + 1] = bmp[bmpos + 2]; //G
					image[newpos + 2] = bmp[bmpos + 1]; //B
					image[newpos + 3] = bmp[bmpos + 0]; //A
				}

			} // Next x

		} // Next y

		return image;
	}



	uint8_t* bmp2jpg(uint8_t* bmp, int bytes_per_pixel, unsigned int w, unsigned int h)
	{
		uint32_t bytes_per_row_without_padding = w * bytes_per_pixel;
		uint32_t padding = (4 - (w * bytes_per_pixel) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;

		// FSCK, the bmp may have 3 bytes_per_pixel, but the PNG always has 4 bytes_per_pixel
		// uint8_t* image = (uint8_t*)malloc(w*h*numChannels*sizeof(uint8_t));
		uint8_t* image = (uint8_t*)malloc(w*h * 3 * sizeof(uint8_t));


		//The amount of scanline bytes is width of image times channels, with extra bytes added if needed
		//to make it a multiple of 4 bytes.
		// unsigned int scanlineBytes = w * numChannels;
		// if(scanlineBytes % 4 != 0)
		//    scanlineBytes = (scanlineBytes / 4) * 4 + 4;

		// printf("scanlineBytes: %d\n", scanlineBytes);

		// There are 3 differences between BMP and the raw image buffer for LodePNG:
		// -it's upside down
		// -it's in BGR instead of RGB format (or BRGA instead of RGBA)
		// -each scanline has padding bytes to make it a multiple of 4 if needed
		// The 2D for loop below does all these 3 conversions at once.
		for (unsigned int y = 0; y < h; y++)
		{

			for (unsigned int x = 0; x < w; x++)
			{
				//pixel start byte position in the BMP
				unsigned int bmpos = (h - y - 1) * row_size_bytes + bytes_per_pixel * x;
				//pixel start byte position in the new raw image
				unsigned int newpos = 3 * y * w + 3 * x;

				if (bytes_per_pixel == 3)
				{
					image[newpos + 0] = bmp[bmpos + 2]; //R
					image[newpos + 1] = bmp[bmpos + 1]; //G
					image[newpos + 2] = bmp[bmpos + 0]; //B
					// image[newpos + 3] = 255;            //A
				}
				else // TODO: ever hit ? 
				{
					image[newpos + 0] = bmp[bmpos + 3]; //R
					image[newpos + 1] = bmp[bmpos + 2]; //G
					image[newpos + 2] = bmp[bmpos + 1]; //B
					// image[newpos + 3] = bmp[bmpos + 0]; //A
				}

			} // Next x

		} // Next y

		return image;
	}




	uint8_t* upside_down(uint8_t* bmp, int bytes_per_pixel, unsigned int w, unsigned int h)
	{
		uint32_t bytes_per_row_without_padding = w * bytes_per_pixel;
		uint32_t padding = (4 - (w * bytes_per_pixel) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;

		// FSCK, the bmp may have 3 bytes_per_pixel, but the PNG always has 4 bytes_per_pixel
		// uint8_t* image = (uint8_t*)malloc(w*h*numChannels*sizeof(uint8_t));
		uint8_t* image = (uint8_t*)malloc(w*h * bytes_per_pixel * sizeof(uint8_t));


		//The amount of scanline bytes is width of image times channels, with extra bytes added if needed
		//to make it a multiple of 4 bytes.
		// unsigned int scanlineBytes = w * numChannels;
		// if(scanlineBytes % 4 != 0)
		//    scanlineBytes = (scanlineBytes / 4) * 4 + 4;

		// printf("scanlineBytes: %d\n", scanlineBytes);

		// There are 3 differences between BMP and the raw image buffer for LodePNG:
		// -it's upside down
		// -it's in BGR instead of RGB format (or BRGA instead of RGBA)
		// -each scanline has padding bytes to make it a multiple of 4 if needed
		// The 2D for loop below does all these 3 conversions at once.
		for (unsigned int y = 0; y < h; y++)
		{

			for (unsigned int x = 0; x < w; x++)
			{
				//pixel start byte position in the BMP
				unsigned int bmpos = (h - y - 1) * row_size_bytes + bytes_per_pixel * x;
				//pixel start byte position in the new raw image
				unsigned int newpos = bytes_per_pixel * y * w + bytes_per_pixel * x;

				if (bytes_per_pixel == 3)
				{
					image[newpos + 0] = bmp[bmpos + 0]; // R
					image[newpos + 1] = bmp[bmpos + 1]; // G
					image[newpos + 2] = bmp[bmpos + 2]; // B
				}
				else if (bytes_per_pixel == 4)
				{
					image[newpos + 0] = bmp[bmpos + 0]; // R
					image[newpos + 1] = bmp[bmpos + 1]; // G
					image[newpos + 2] = bmp[bmpos + 2]; // B
					image[newpos + 3] = bmp[bmpos + 3]; // A
				}

			} // Next x

		} // Next y

		return image;
	}



	void encodePng(const char* filename, uint8_t* image, unsigned int width, unsigned int height)
	{
		// Encode the image 
		// int bytes_per_pixel = 4;
		// uint8_t* pngByteArrangement = bmp2png(image, bytes_per_pixel, width, height);

		unsigned error = lodepng_encode32_file(filename, image, width, height);
		// free(pngByteArrangement);

		// if there's an error, display it
		if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
	}


	// Open file. In case of error, print message and exit.
	FILE *_open_file(const char *filename, const char *mode)
	{
		FILE *fp = fopen(filename, mode);
		if (fp == NULL)
		{
			fprintf(stderr, "Could not open file %s\n", filename);

			exit(EXIT_FAILURE);
		}

		return fp;
	}


	void encodeJpg(const char* filename, uint8_t* image, unsigned int width, unsigned int height)
	{
		int bytes_per_pixel = 4;
		uint8_t* jpegByteArrangement = bmp2jpg(image, bytes_per_pixel, width, height);

		FILE *input_ptr = _open_file(filename, "wb");
		TooJpeg::WRITE_ONE_BYTE lambda = [input_ptr](unsigned char oneByte) { fputc(oneByte, input_ptr); };

		bool ok = TooJpeg::writeJpeg(lambda, image, width, height);
		if (ok != true)
		{
			fprintf(stderr, "Could not write JPEG-file \"%s\".\n", filename);
		}

		fclose(input_ptr);

		free(jpegByteArrangement);
	}




	// Function to reverse every sub-array formed by 
	// consecutive k elements 
	void reverse(uint8_t* arr, int n, int k)
	{
		for (int i = 0; i < n; i += k)
		{
			int left = i;

			// to handle case when k is not multiple of n 
			int right = min(i + k - 1, n - 1);

			// reverse the sub-array [left, right] 
			while (left < right)
				std::swap(arr[left++], arr[right--]);

		}
	}



	uint8_t * pngMagic2(uint8_t* src, int width, int height, int pixelSize)
	{
		uint32_t bytes_per_row_without_padding = width * pixelSize;
		uint32_t padding = (4 - (width * pixelSize) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;

		uint8_t * dst = upside_down(src, pixelSize, width, height);

		for (int y = 0; y < height; ++y)
		{
			reverse(&dst[y*row_size_bytes], row_size_bytes, 4);
		}


		int size = width * height*pixelSize;
		for (size_t j = 0; j < size; j += 4)
		{

			uint8_t temp0 = dst[j];
			uint8_t temp1 = dst[j + 1];
			uint8_t temp2 = dst[j + 2];
			uint8_t temp3 = dst[j + 3];

			dst[j] = temp0;
			dst[j + 1] = temp3;
			dst[j + 2] = temp2;
			dst[j + 3] = temp1;
		}


		return dst;
	}

	uint8_t * memcpyrev(uint8_t* src, int width, int height, int pixelSize)
	{
		uint32_t bytes_per_row_without_padding = width * pixelSize;
		uint32_t padding = (4 - (width * pixelSize) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;
		uint8_t * dst = (uint8_t*)malloc(width*height*pixelSize);

		for (int y = 0; y < height; ++y)
		{
			int fromRow = height - 1 - y;

			memcpy((void*)&dst[fromRow* row_size_bytes], (void*)&src[y* row_size_bytes], row_size_bytes);
		}

		return dst;
	}


	uint8_t * pngMagic3(uint8_t* src, int width, int height, int pixelSize)
	{
		// uint32_t bytes_per_row_without_padding = width * pixelSize;
		// uint32_t padding = (4 - (width * pixelSize) % 4) % 4;
		// uint32_t row_size_bytes = bytes_per_row_without_padding + padding;

		// CEF to bitmap 
		uint8_t * dst = upside_down(src, pixelSize, width, height);
		
		// bitmap to png components order 
		int size = width * height*pixelSize;
		for (size_t j = 0; j < size; j += 4)
		{
			
			// https://en.wikipedia.org/wiki/BMP_file_format#Example_2
			// Bitmap pixel-arrays are BGRA
			uint8_t temp0 = dst[j]; // Blue
			uint8_t temp1 = dst[j + 1]; // Green
			uint8_t temp2 = dst[j + 2]; // Red
			uint8_t temp3 = dst[j + 3]; // Alpha

			// PNG pixels are ABGR 
			dst[j] = temp3; // Alpha
			dst[j + 1] = temp0; // Blue
			dst[j + 2] = temp1; // Green
			dst[j + 3] = temp2; // Red
		}
		
		// bitmap to png 
		uint8_t* pngByteArrangement = bmp2png(dst, pixelSize, width, height);
		return pngByteArrangement;
	}



	uint8_t * pngMagic(uint8_t* src, int width, int height, int pixelSize)
	{
		uint32_t bytes_per_row_without_padding = width * pixelSize;
		uint32_t padding = (4 - (width * pixelSize) % 4) % 4;
		uint32_t row_size_bytes = bytes_per_row_without_padding + padding;
		

		int size = width * height*pixelSize;
		uint8_t * dst = (uint8_t*)malloc(size);

		for (size_t i = 0; i < size; ++i)
			dst[size - 1 - i] = src[i];


		for (size_t j = 0; j < size; j += 4)
		{

			uint8_t temp0 = dst[j];
			uint8_t temp1 = dst[j + 1];
			uint8_t temp2 = dst[j + 2];
			uint8_t temp3 = dst[j + 3];

			dst[j] = temp0;
			dst[j + 1] = temp3;
			dst[j + 2] = temp2;
			dst[j + 3] = temp1;
		}

		for (int y = 0; y < height; ++y)
		{
			reverse(&dst[y*row_size_bytes], row_size_bytes, 4);
		}
		

		return dst;
	}




	uint8_t * argb2rgba(uint8_t* src, size_t n)
	{
		uint8_t * dst = (uint8_t*)malloc(n);

		for (size_t j = 0; j < n; j += 4)
		{

			uint8_t A = dst[j];
			uint8_t R = dst[j + 1];
			uint8_t G = dst[j + 2];
			uint8_t B = dst[j + 3];

			dst[j] = A;
			dst[j + 1] = B;
			dst[j + 2] = G;
			dst[j + 3] = R;
		}

		return dst;
	}





	void RenderHandler::OnPaint(
		CefRefPtr<CefBrowser> browser,
		CefRenderHandler::PaintElementType type,
		const CefRenderHandler::RectList& dirtyRects,
		const void* buffer, int width, int height
	)
	{
		if (_hasBeenRendered)
			return;

		//BMPImage * image = CreateBitmapFromScan0((int32_t)width, (int32_t) height, (uint8_t*)buffer);
		// bool result = write_bmp(FILE *fp, BMPImage *image, char **error);



		const char* bitmapFileName = "D:\\TestImageSmall.bmp";
		const char* bufferFileName = "D:\\TestImageSmall_buffer.bmp";
		const char* reverseBufferFileName = "D:\\TestImageSmall_buffer_reverse.bmp";
		const char* pngFileName = "D:\\TestImageSmall.png";
		const char* jpegFileName = "D:\\TestImageSmall.jpg";


		uint8_t* reverseBuffer = memcpyrev((uint8_t*)buffer, width, height, 4); // OK
		uint8_t* bmpData = upside_down( (uint8_t*) buffer, 4, width, height); // OK
		
		
		// uint8_t* pngData = argb2rgba((uint8_t*)reverseBuffer, 4 * width*height);
		// uint8_t* pngData = pngMagic((uint8_t*)buffer, width, height, 4);
		//uint8_t* pngData = pngMagic2((uint8_t*)buffer, width, height, 4);
		uint8_t* pngData = pngMagic3((uint8_t*)buffer, width, height, 4);
		


		WriteBitmapToFile(bufferFileName, width, height, buffer);
		WriteBitmapToFile(reverseBufferFileName, width, height, reverseBuffer);
		WriteBitmapToFile(bitmapFileName, width, height, bmpData);

		encodePng(pngFileName, (uint8_t*)pngData, width, height);
		encodeJpg(jpegFileName, bmpData, width, height);


		printf("\n\nfinished rendering image.\n");
		printf("Note:\n");
		printf("If the webpage-background has no background-color set, it is rendered as black, because transparency is not implemented.\n");
		printf("If the image is vertically flipped, this is because negative stride has not been handled.\n");

		printf("\n\n");
		_hasBeenRendered = true;
	}


} // namespace cefpdf
