//
//  CDFaceWindow.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__FaceWindow__
#define __candide__FaceWindow__

#include <iostream>
#include <FL/Fl.h>

#include <FL/gl.h>
#include <FL/Fl_Gl_Window.h>

#include <glm/glm.hpp>

#include "CDFaceData.h"



class CDFaceWindow : public Fl_Gl_Window
{
public:
	CDFaceWindow( int x, int y, int w, int h, CDFaceData* faceData);
	~CDFaceWindow();
	
	void setBackgroundImage( const std::string& pngFilename );
	
	void setTransform( glm::mat4 _transform ) { transform = _transform; }
	void draw();
	
protected:
	
private:
	CDFaceData* faceData;
	
	std::string backgroundTexturePath;
	GLuint backgroundTexture;
	int backgroundTextureW, backgroundTextureH;
	
	glm::mat4 transform;
};

#endif /* defined(__candide__FaceWindow__) */
