//
//  CDMVMDTestWindow.h
//  candide
//
//  Created by damian on 10/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDMVMDTestWindow__
#define __candide__CDMVMDTestWindow__

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>

#include "CDMeanValueMeshDeformer.h"

class CDMVMDTestGlWindow;
class Fl_Slider;

class CDMVMDTestWindow: public Fl_Window
{
public:
	CDMVMDTestWindow( int w, int h, const char* label );
	~CDMVMDTestWindow() {};
	
	void idle();
	
	void resize( int x, int y, int w, int h );
	

private:
	
	static void _sliderChanged( Fl_Widget* slider, void* data );
	void sliderChanged( const std::string& slidername, float value );
	
	void updateDeformation();
	
	CDMeanValueMeshDeformer deformer;

	CDMesh originalMesh;
	CDMesh controlMesh;
	CDMVMDTestGlWindow* glWindow;
	
};


class CDMVMDTestGlWindow: public Fl_Gl_Window
{
public:
	CDMVMDTestGlWindow( int x, int y, int w, int h, CDMesh& controlMesh );
	
	void setDeformedMesh( const CDMesh& mesh ) { deformedMesh = mesh; }
	void draw();

	void update();
	
private:

	
	float angle;
	CDMesh deformedMesh;
	CDMesh& controlMesh;
};



#endif /* defined(__candide__CDMVMDTestWindow__) */
