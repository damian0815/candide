//
//  CandideWindow.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CandideWindow.h"
#include <math.h>

CandideWindow::CandideWindow(int x, int y, int w, int h)
: Fl_Gl_Window(x, y, w, h, "Candide")
{
}

void CandideWindow::draw()
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
	
	
	glColor3f(.5,.6,.7);
	glBegin(GL_POLYGON);
	int sides = 3;
	for (int j=0; j<sides; j++) {
		double ang = j*2*M_PI/sides;
		glVertex3f(cos(ang),sin(ang),0);
	}
	glEnd();
	
}

