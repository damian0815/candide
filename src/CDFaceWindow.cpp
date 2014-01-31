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
using namespace glm;

CDFaceWindow::CDFaceWindow(int x, int y, int w, int h, CDFaceData* _faceData )
: Fl_Gl_Window(x, y, w, h, "FaceGL")
, faceData(_faceData)
{
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
	
	glClearColor(0,0,0,0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&transform[0][0]);
	
	glEnable(GL_BLEND);
	
	glColor4f(1,1,1,0.5);
	faceData->draw(vec3(0,0,0), vec3(0.95,0.95,0.95));
	
/*
	
	glColor4f(.5,.6,.7,.3);
	glBegin(GL_POLYGON);
	int sides = 3;
	for (int j=0; j<sides; j++) {
		double ang = j*2*M_PI/sides;
		glVertex3f(cos(ang),sin(ang),0);
	}
	glEnd();*/
	
}


