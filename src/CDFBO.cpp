//
//  CDFBO.cpp
//  candide
//
//  Created by damian on 20/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "CDFBO.h"
#include "CDUtilities.h"
#include "CDAppException.h"

#include <fstream>

using namespace std;

CDFBO::CDFBO( GLuint width, GLuint height )
: width(width), height(height)
{
	// Create framebuffer
	glGenFramebuffers( 1, &framebuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
	
	// create texture
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// define texture with RGB8 format
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// attach 2D texture to framebuffer
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0 );
	
	/*
	// depth
	glGenRenderbuffers( 1, &depthRenderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, depthRenderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	// attach depth buffer
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer );
	*/
	
	// good?
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		throw new CDAppException("Framebuffer incomplete");
	}
	
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

CDFBO::~CDFBO()
{
	glDeleteTextures(1, &texture);
	// bind framebuffer 0, ie unbind fbo
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffers(1, &framebuffer);
}

void CDFBO::bind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,width,height);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	GLenum error = glGetError();
	if ( error != GL_NO_ERROR ) {
		CDLog << "OpenGL error: " << error << " " << gluErrorString(error);
	}
}

void CDFBO::unbind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glPopAttrib();
}


void CDFBO::saveToTGA( const std::string& path )
{
	glFlush();
	// from http://www.flashbang.se/archives/155
	
	long imageSize = width*height*3;
	unsigned char *data = new unsigned char[imageSize];
	
	GLenum error = glGetError();
	if ( error != GL_NO_ERROR ) {
		CDLog << "OpenGL error: " << error << " " << gluErrorString(error);
	}
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	
	glReadPixels(0,0,width,height, GL_BGR, GL_UNSIGNED_BYTE,data);// split x and y sizes into bytes
	
	error = glGetError();
	if ( error != GL_NO_ERROR ) {
		CDLog << "OpenGL error: " << error << " " << gluErrorString(error);
	}
	
	int xa= width % 256;
	int xb= (width-xa)/256;
	int ya= height % 256;
	int yb= (height-ya)/256;
	//assemble the header
	unsigned char header[18]={0,0,2,0,0,0,0,0,0,0,0,0,(unsigned char)xa,(unsigned char)xb,(unsigned char)ya,(unsigned char)yb,24,0};
	
	// write header and data to file
	ofstream outfile(path.c_str(), ios::out | ios::binary);
	outfile.write (reinterpret_cast<char *>(header), sizeof (char)*18);
	outfile.write (reinterpret_cast<char *>(data), sizeof (char)*imageSize);
	outfile.close();
	
	delete[] data;
	data=NULL;
}


