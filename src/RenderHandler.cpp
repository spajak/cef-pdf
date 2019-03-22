
#include "RenderHandler.h"
#include "Bmp.h"

namespace cefpdf {

	bool _hasBeenRendered;


	RenderHandler::RenderHandler() {
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
		

#ifdef USE_GDI
		const char* filename = "D:\\TestImageSmall.png";
#else 
		const char* filename = "D:\\TestImageSmall.bmp";
#endif 

		WriteBitmapToFile(filename, width, height, buffer);
		
		printf("\n\nfinished rendering image.\n");
		printf("Note:\n");
		printf("If the webpage-background has no background-color set, it is rendered as black, because transparency is not implemented.\n");
		printf("If the image is vertically flipped, this is because negative stride has not been handled.\n");

		printf("\n\n");
		_hasBeenRendered = true;
	}


} // namespace cefpdf
