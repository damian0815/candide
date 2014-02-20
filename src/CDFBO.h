//
//  CDFBO.h
//  candide
//
//  Created by damian on 20/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDFBO__
#define __candide__CDFBO__

#include <iostream>
#include <OpenGL/gl.h>

class CDFBO
{
public:
	CDFBO( GLuint width, GLuint height );
	~CDFBO();
	
	float getAspectRatio() { return (float)width/(float)height; }
	
	void bind();
	void unbind();
	
	void saveToTGA( const std::string& path );
	
private:
	
	GLuint width, height;
	GLuint texture;
	GLuint framebuffer;
	
	float aspect;
	
};

#endif /* defined(__candide__CDFBO__) */
