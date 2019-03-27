#ifndef BITMAPLION_BITMAPINFORMATION_H
#define BITMAPLION_BITMAPINFORMATION_H


#ifdef __cplusplus
	// #include <iostream>
	// #include <fstream>
	#include <cstdio>
	#include <cstdlib>
	#include <cstdint>
	#include <cstring>
#else
	#include <stdio.h> // printf 
	#include <stdlib.h> // for malloc
	#include <stdint.h> // for int32_t, int8_t, etc. 
	#include <stdbool.h> // true, false in plain old C 
	#include <string.h>  // for strlen, strcopy
#endif


#if ( defined (__WIN32__) || defined (__WIN32) || defined (_WIN32) || defined (WIN32)  )
// windows specific code goes here
#pragma warning(disable:4458)

#pragma comment (lib,"gdiplus.lib")

#include <Windows.h>
#include <ObjIdl.h>
#include <minmax.h>
#include <gdiplus.h>
// #include <gdiplusheaders.h>
// #include <wingdi.h>
// #include <gdiplusbitmap.h>
// #include <gdiplusflat.h>
// #include <Gdipluspixelformats.h>


// using namespace Gdiplus;

#pragma warning(default:4458)
#endif



#define BMP_HEADER_SIZE 54
#define DIB_HEADER_SIZE 40

// Correct values for the header
#define MAGIC_VALUE         0x4D42
#define NUM_PLANE           1
#define COMPRESSION         0
#define NUM_COLORS          0
#define IMPORTANT_COLORS    0
#define BITS_PER_BYTE 8
// #define BITS_PER_PIXEL 24
#define BITS_PER_PIXEL 32


#ifdef _MSC_VER
#pragma pack(push)  // save the original data alignment
#pragma pack(1)     // Set data alignment to 1 byte boundary
#endif


typedef struct
#ifndef _MSC_VER
__attribute__((packed))
#endif
{
	uint16_t type;              // Magic identifier: 0x4d42
	uint32_t size;              // File size in bytes
	uint16_t reserved1;         // Not used
	uint16_t reserved2;         // Not used
	uint32_t offset;            // Offset to image data in bytes from beginning of file
	uint32_t dib_header_size;   // DIB Header size in bytes
	int32_t  width_px;          // Width of the image
	int32_t  height_px;         // Height of image
	uint16_t num_planes;        // Number of color planes
	uint16_t bits_per_pixel;    // Bits per pixel
	uint32_t compression;       // Compression type
	uint32_t image_size_bytes;  // Image size in bytes
	int32_t  x_resolution_ppm;  // Pixels per meter
	int32_t  y_resolution_ppm;  // Pixels per meter
	uint32_t num_colors;        // Number of colors
	uint32_t important_colors;  // Important colors
} BMPHeader;


#ifdef _MSC_VER
#pragma pack(pop)  // restore the previous pack setting
#endif



typedef struct
{
	BMPHeader header;
	// unsigned char* data;
	// It is more informative and will force a necessary compiler error
	// on a rare machine with 16-bit char.
	uint8_t* data;
} BMPImage;


// #define USE_GDI true 

#ifndef USE_GDI
BMPImage * CreateBitmapFromScan0(int32_t w, int32_t h, uint8_t* scan0);
#endif 

void WriteBitmapToFile(const char *filename, int width, int height, const void* buffer);



#endif //BITMAPLION_BITMAPINFORMATION_H
