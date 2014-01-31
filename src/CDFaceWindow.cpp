//
//  CDFaceWindow.cpp
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDFaceWindow.h"
#include <Fl/math.h>
#include <glm/glm.hpp>

#include "CDImageLoader.h"

using namespace glm;
using namespace std;

CDFaceWindow::CDFaceWindow(int x, int y, int w, int h, CDFaceData* _faceData )
: Fl_Gl_Window(x, y, w, h, "FaceGL")
, faceData(_faceData), backgroundTexture(0)
{
}

CDFaceWindow::~CDFaceWindow()
{
	if ( backgroundTexture ) {
		glDeleteTextures(1, &backgroundTexture);
		backgroundTexture = 0;
	}
}

void CDFaceWindow::setBackgroundImage( const string& path )
{
	backgroundTexturePath = path;
}

void CDFaceWindow::draw()
{
	if ( !valid() ) {
		valid(1);
		// set viewport size
		glViewport(0,0,w(),h());
		
		// load orthogonal projection matrix
		float aspectRatio = (float)w()/(float)h();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-aspectRatio, aspectRatio, -1, 1, -100, 100);
		
	}
	
	if ( backgroundTexture==0 && !backgroundTexturePath.empty() ) {
		// load the texture
		backgroundTexture = CDImageLoader::createOpenGLTextureFromImage(backgroundTexturePath, backgroundTextureW, backgroundTextureH);
		if ( 0==backgroundTexture ) {
			cerr<<"couldn't load texture from '"<<backgroundTexturePath<<"'"<<endl;
			backgroundTexturePath = "";
		}
	}
	
	glClearColor(0,0,0,0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glColor4f(1,1,1,0.5);
	
	if ( backgroundTexture ) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);
		
		// draw the background image
		
		float imageAspect = ((float)backgroundTextureW/(float)backgroundTextureH);
		GLfloat imageW = 1.0f*imageAspect;
		GLfloat imageH = 1.0f;
		
		glBegin(GL_QUADS);
		glVertex2f(imageW/2, -imageH/2);
		glTexCoord2f(0, 1);
		glVertex2f(-imageW/2, -imageH/2);
		glTexCoord2f(0, 0);
		glVertex2f(-imageW/2, imageH/2);
		glTexCoord2f(1, 0);
		glVertex2f(imageW/2, imageH/2);
		glTexCoord2f(1, 1);
		glEnd();
		
		glDisable(GL_TEXTURE_2D);
	}
		
	// draw the actual facedata

	
	glLoadMatrixf(&transform[0][0]);
	faceData->draw(vec3(0,0,0), vec3(0.95,0.95,0.95));

	
	
}


