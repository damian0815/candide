//
//  CDMVMDTestWindow.cpp
//  candide
//
//  Created by damian on 10/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDMVMDTestWindow.h"
#include "CDAssimpLoader.h"
#include <GLUT/GLUT.h>
#include <Fl/Fl_Box.H>
#include <Fl/Fl_Hor_Slider.H>
#include <Fl/Fl_Menu_Bar.H>

#include "CDMeshObjWriter.h"

static const float framerate = 1.0f/60.0f;

using namespace glm;
using namespace std;

CDMVMDTestGlWindow::CDMVMDTestGlWindow( int x, int y, int w, int h, CDMesh& _controlMesh, CDMesh& _originalMesh )
: Fl_Gl_Window( x, y, w, h ), angle(0), controlMesh(_controlMesh), originalMesh(_originalMesh)
{
	valid(0);
	
	end();
}

void CDMVMDTestGlWindow::update()
{
	angle += framerate*M_PI*2.0;
}

void CDMVMDTestGlWindow::draw()
{
	if ( !valid() ) {
		valid(1);
		// set viewport size
		glViewport(0,0,w(),h());
		
		// load orthogonal projection matrix
		float aspectRatio = (float)w()/(float)h();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		float fov = 45.0f;
		float zNear = 0.1f;
		float zFar = 100;
		gluPerspective( fov, aspectRatio, zNear, zFar );
		gluLookAt(-4, 0, 0,
				  0, 0, 0,
				  0, 1, 0);
		//glOrtho(-aspectRatio, aspectRatio, -1, 1, -100, 100);
	}
		
	glClearColor(0,0,0,0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glScalef(1, 1, 1);
	glRotatef(angle, 0, 1, 0);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glColor4f(1.0,0.2,0.2,0.5);
	originalMesh.draw();
	glColor4f(0.2,1.0,1.0,0.5);
	deformedMesh.draw();
	
	glColor4f(1,1,1,1);
	controlMesh.draw();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
}

void idleCallback( void* data )
{
	CDMVMDTestWindow* window = (CDMVMDTestWindow*)data;
	window->idle();
	Fl::repeat_timeout( framerate, &idleCallback, data );
}





CDMVMDTestWindow::CDMVMDTestWindow( int w, int h, const char* label )
: Fl_Window( w, h, label )
{

	// create sliders on the right
	int interfaceWidth = 300;
	int interfaceStartX = w-(interfaceWidth+10);
	int menuHeight = 0;
	Fl_Hor_Slider* slider = new Fl_Hor_Slider( interfaceStartX, 30+menuHeight, interfaceWidth-100, 20, "XSlider" );
	slider->labeltype(FL_NORMAL_LABEL);
	slider->align(FL_ALIGN_RIGHT);
	slider->callback(&CDMVMDTestWindow::_sliderChanged, this );
	slider->value(0);
	slider->bounds(-1,1);

	slider = new Fl_Hor_Slider( interfaceStartX, 70+menuHeight, interfaceWidth-100, 20, "YSlider" );
	slider->labeltype(FL_NORMAL_LABEL);
	slider->align(FL_ALIGN_RIGHT);
	slider->callback(&CDMVMDTestWindow::_sliderChanged, this );
	slider->value(0);
	slider->bounds(-1,1);
	

	
	
	CDAssimpLoader loader;
	bool loaded = loader.loadModel("data/male_head_3ds-st.dae");
	assert(loaded);
	originalMesh = loader.getLoadedMesh();
	originalMesh.updateNormals();
	
	loaded = loader.loadModel("data/candide-closed.dae");
	assert(loaded);
	controlMesh = loader.getLoadedMesh();
	controlMesh.updateNormals();
	
	// test deformer
	deformer.setupDeformation(originalMesh, controlMesh);
	
	if ( glWindow ) {
		glWindow->redraw();
	}
	
	
	// create opengl window
	glWindow = new CDMVMDTestGlWindow( 10, 10, w-interfaceWidth-30, h-20, controlMesh, originalMesh );
	resizable(glWindow);
	glWindow->end();


	// done
	end();
		
	// setup idle
	Fl::add_timeout( framerate, &idleCallback, this );
		 
	// call first draw
	updateDeformation();
	deformer.compareDeformedWithOriginal();
	
	//CDMeshObjWriter writer;
	//writer.writeObj( deformer.getDeformedMesh(), "/tmp/deformed.obj");
}

void CDMVMDTestWindow::resize(int x, int y, int w, int h)
{
	Fl_Window::resize(x,y,w,h);
	
	// resize the face window
	//faceWindow->position(w/2-faceWindow->w()/2, h/2-faceWindow->h()/2);
}

void CDMVMDTestWindow::_sliderChanged(Fl_Widget *slider, void *data)
{
	CDMVMDTestWindow* window = (CDMVMDTestWindow*)data;
	window->sliderChanged( slider->label(), ((Fl_Slider*)slider)->value() );
}

void CDMVMDTestWindow::sliderChanged(const std::string &slidername, float value)
{
	size_t whichVertex = 0;
	vec3 v = controlMesh.getVertex(whichVertex);
	
	if ( slidername=="XSlider" ) {
		v.x = value;
		controlMesh.setVertex(whichVertex, v);
	} else if ( slidername=="YSlider" ) {
		v.y = value;
		controlMesh.setVertex(whichVertex, v);
	}
	updateDeformation();
}


void CDMVMDTestWindow::updateDeformation()
{
	deformer.updateDeformation(controlMesh);
	if ( glWindow ) {
		glWindow->setDeformedMesh(deformer.getDeformedMesh());
		glWindow->redraw();
	}
	
}

void CDMVMDTestWindow::idle()
{
	if ( glWindow ) {
		glWindow->update();
		glWindow->redraw();
	}
}

