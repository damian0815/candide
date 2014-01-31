//
//  CDImageLoader.cpp
//  candide
//
//  Created by damian on 31/01/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDImageLoader.h"
using namespace std;

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <Fl/Fl_Shared_Image.H>
#include <Fl/Fl_PNG_Image.H>

#include <png.h>
#include <cstdio>
#include <string>
#include "CDAppException.h"


using namespace std;

GLuint CDImageLoader::createOpenGLTextureFromImage(const string& filename, int &width, int &height)
{
	Fl_Shared_Image* image = Fl_Shared_Image::get(filename.c_str());
	
	
	if ( image && image->w() && image->h() ) {
		
		width = image->w();
		height = image->h();
		const char* const imageData = image->data()[0];
		// make sure width is an even number of pixels
		int outPadding = width%2;
		
		char* imageDataRepacked = (char*)malloc(3*(width+outPadding)*height);
		for ( int i=0; i<height; i++ ) {
			
			unsigned int sourceRowStart = i*(width*image->d() + image->ld());
			unsigned int targetRowStart = i*((width+outPadding)*3);
			
			// convert RGBA to RGB as necessary
			if ( image->d() == 3 ) {
				memcpy(imageDataRepacked + targetRowStart, imageData + sourceRowStart, width*3);
			} else {
				for ( int j=0; j<width; j++ ) {
					imageDataRepacked[targetRowStart + j*3] = imageData[sourceRowStart + j*image->d()];
					imageDataRepacked[targetRowStart + j*3 + 1] = imageData[sourceRowStart + j*image->d() + 1];
					imageDataRepacked[targetRowStart + j*3 + 2] = imageData[sourceRowStart + j*image->d() + 2];
				}
			}
		}
		
		// generate the OpenGL texture object
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
					 GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) imageDataRepacked);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glBindTexture(GL_TEXTURE_2D,0);
		
		return texture;
	} else {
		return 0;
	}
	
}


#define TEXTURE_LOAD_ERROR 0

GLuint stupid(const string& filename, int &width, int &height)
{
	
	//header for testing if it is a png
	png_byte header[8];
	
	//open file as binary
	FILE *fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		return TEXTURE_LOAD_ERROR;
	}
	
	//read the header
	fread(header, 1, 8, fp);
	
	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		fclose(fp);
		return TEXTURE_LOAD_ERROR;
	}
	
	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
												 NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		return (TEXTURE_LOAD_ERROR);
	}
	
	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return (TEXTURE_LOAD_ERROR);
	}
	
	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return (TEXTURE_LOAD_ERROR);
	}
	
	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return (TEXTURE_LOAD_ERROR);
	}
	
	//init png reading
	png_init_io(png_ptr, fp);
	
	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);
	
	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);
	
	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;
	
	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
				 NULL, NULL, NULL);
	
	//update width and height based on png info
	width = twidth;
	height = theight;
	
	// work out bpp
	int bpp = 3;
	if ( color_type == PNG_COLOR_TYPE_RGB ) {
		bpp = 3;
	} else if ( color_type == PNG_COLOR_TYPE_RGB_ALPHA ) {
		bpp = 4;
	} else {
		throw new CDAppException("unrecognized color type in PNG");
	}
	
	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);
	
	// Row size in bytes.
	png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	
	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return TEXTURE_LOAD_ERROR;
	}
	
	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		return TEXTURE_LOAD_ERROR;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = image_data + i * rowbytes;
	
	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);
	
	//Now generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);
	
	return texture;
}
