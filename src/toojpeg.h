//
// Created by root on 27.03.19.
//

#ifndef TOOJPEG_H
#define TOOJPEG_H

#include <functional> // for std::function/lambda 
#include <cstdint> // for int32_t, int8_t, int16_t


// //////////////////////////////////////////////////////////
// toojpeg.h
// written by Stephan Brumme, 2018
// see https://create.stephan-brumme.com/toojpeg/
//

// This is a compact baseline JPEG/JFIF writer, written in C++ (but looks like C for the most part).
// Its interface has only one function: writeJpeg() - and that's it !
//
// basic example:
// => create an image with any content you like, e.g. 1024x768, RGB = 3 bytes per pixel
// auto pixels = new unsigned char[1024*768*3];
// => you need to define a callback that receives the compressed data byte-by-byte from my JPEG writer
// void cbOutput(unsigned char oneByte) { fputc(oneByte, myFileHandle); } // save byte to file
// => let's go !
// writeJpeg(cbOutput, mypixels, 1024, 768);


namespace TooJpeg
{
	// write one byte (to disk, memory, ...)
	// typedef void (*WRITE_ONE_BYTE)(unsigned char);
	using WRITE_ONE_BYTE = std::function<void(unsigned char)>;



	// this callback is called for every byte generated by the encoder and behaves similar to fputc
	// if you prefer stylish C++11 syntax then it can be a lambda, too:
	// auto myCallback = [](unsigned char oneByte) { fputc(oneByte, output); };

	// handle       - callback that stores a single byte (writes to disk, memory, ...)
	// width,height - image size
	// pixels       - stored in RGB format or grayscale, stored from upper-left to lower-right
	// isRGB        - true if RGB format (3 bytes per pixel); false if grayscale (1 byte per pixel)
	// quality      - between 1 (worst) and 100 (best)
	// downSample   - if true then YCbCr 4:2:0 format is used (smaller size, minor quality loss) instead of 4:4:4, not relevant for grayscale
	// comment      - optional JPEG comment (0/NULL if no comment)
	bool writeJpeg(WRITE_ONE_BYTE output, const void* pixels, unsigned short width, unsigned short height,
		bool isRGB = true, unsigned char quality = 90, bool downSample = false, const char* comment = 0
	);
} // namespace TooJpeg

// My main inspiration was Jon Olick's Minimalistic JPEG writer
// ( https://www.jonolick.com/code.html => direct link is https://www.jonolick.com/uploads/7/9/2/1/7921194/jo_jpeg.cpp ).
// However, his code documentation is quite sparse - probably because it wasn't written from scratch and is "based on a javascript jpeg writer"
// (most likely Andreas Ritter's code: https://github.com/eugeneware/jpeg-js/blob/master/lib/encoder.js )
//
// I felt that compression could be a bit better for a few cases, therefore I added the following features:
// a) YCbCr 4:2:0 downsampling is often about 20% more efficent than the default 4:4:4 with only little visual loss
// b) grayscale images contain a single channel, no need to save the redundant Cb + Cr channels
// c) optional: insert a JPEG comment
//
// On top of that, I added tons of comments, especially describing where all those magic numbers come from,
// and removed the need for external includes.
// Yes, that's right: my library has no (!) includes at all, not even #include <stdlib.h>
//
// Depending on your callback TOOJPEG_WRITE_ONE_BYTE, the library writes either to disk, or in-memory, or wherever you wish.
// Moreover, no dynamic memory allocation are performed, just a few bytes on the stack (less than 10k).
//
// Your C++ compiler needs to support a reasonable subset of C++11 (g++ 4.7 or Visual C++ 2013 are sufficient).
// I haven't tested the code on ARM chips, big-endian systems or anything that smells like an apple.
//
// USE AT YOUR OWN RISK. Because you are a brave soul :-)



#endif // TOOJPEG_H
