//
//  CDImageLoader.h
//  candide
//
//  Created by damian on 31/01/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDImageLoader__
#define __candide__CDImageLoader__

#include <iostream>
#include <string>
#include <OpenGL/OpenGL.h>
class Fl_Shared_Image;

class CDImageLoader
{
public:
	static GLuint createOpenGLTextureFromImage( const std::string &path, int &width, int &height );
	static GLuint createOpenGLTextureFromImage( Fl_Shared_Image* image );
};

#endif /* defined(__candide__CDImageLoader__) */
