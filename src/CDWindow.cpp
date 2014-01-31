//
//  CDWindow.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDWindow.h"
#include "CDCommon.h"
#include "CDApp.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Fl/Fl_Choice.H>
#include <Fl/Fl_Hor_Value_Slider.H>

using namespace glm;
using namespace std;

static void sliderCallback(Fl_Widget* widget, void* userData)
{
	CDWindow* cdWindow = static_cast<CDWindow*>(userData);
	if ( cdWindow ) {
		Fl_Slider* slider = dynamic_cast<Fl_Slider*>(widget);
		cdWindow->sliderChanged( slider->label(), slider->value() );
	}
}

CDWindow::CDWindow(int w, int h, const char* label, CDFaceData* faceData )
: Fl_Window(w, h, label)
{
	int interfaceWidth = 300;
	
	// create the face windows
	Fl_Group* faceWindows = new Fl_Group( 10, 10, w-interfaceWidth-30, h-20 );
	faceWindows->clip_children(true);
	resizable(faceWindows);
	
	int faceWindowGap = 5;
	int faceWindowWidth = (w-interfaceWidth-30-faceWindowGap)/2;
	faceWindowFront = new CDFaceWindow(10,10,faceWindowWidth,h-20, faceData);
	faceWindowSide = new CDFaceWindow(10+faceWindowWidth+faceWindowGap,10,faceWindowWidth,h-20, faceData);
	// make a 90 degree rotation about the y axis
	mat4 sideTransform = rotate(mat4(), 90.0f, vec3(0,1,0));
	faceWindowSide->setTransform( sideTransform );
	
	faceWindows->end();
	
	// create buttons
	int interfaceStartX = w-interfaceWidth-10;
	Fl_Group* interfaceElements = new Fl_Group( interfaceStartX, 10, interfaceWidth, h-20 );
	interfaceElements->resizable(0);
	interfaceElements->clip_children(true);
	// make a drop-down and a slider for the shape units
	vector<string> suNames = faceData->getShapeUnitNames();
	Fl_Choice* suDropdown = new Fl_Choice( interfaceStartX, 10, interfaceWidth, 20, "Shape Unit" );
	suDropdown->labeltype(FL_NORMAL_LABEL);
	suDropdown->align(FL_ALIGN_LEFT);
	suDropdown->callback(&CDWindow::_dropdownChanged, this);
	for ( const string& suName: suNames ) {
		string nameEdited = suName;
		replace(nameEdited.begin(),nameEdited.end(),'/','|');
		suDropdown->add( nameEdited.c_str(), 0, 0 );
	}
	
	if ( suNames.size()>0 ) {
		Fl_Value_Slider* slider = new Fl_Hor_Value_Slider( interfaceStartX, 35, interfaceWidth, 20, "SUSlider" );
		slider->labeltype(FL_NO_LABEL);
		slider->align(FL_ALIGN_LEFT);
		slider->callback(&sliderCallback, this );
		slider->value(0);
		slider->bounds(-1,1);
		shapeUnitSlider = slider;
		
		suDropdown->value(0);
		dropdownChanged(suDropdown->label(), suDropdown->value());
	}
	
	vector<string> auNames = faceData->getAnimationUnitNames();
	Fl_Choice* auDropdown = new Fl_Choice( interfaceStartX, 60, interfaceWidth, 20, "Animation Unit" );
	auDropdown->align(FL_ALIGN_LEFT);
	auDropdown->callback(&CDWindow::_dropdownChanged, this);
	for ( const string& auName: auNames ) {
		string nameEdited = auName;
		replace(nameEdited.begin(),nameEdited.end(),'/','\\');
		auDropdown->add( nameEdited.c_str(), 0, 0 );
	}
	
	if ( auNames.size()>0 ) {
		Fl_Value_Slider* slider = new Fl_Hor_Value_Slider( interfaceStartX, 85, interfaceWidth, 20, "AUSlider" );
		slider->labeltype(FL_NO_LABEL);
		slider->align(FL_ALIGN_LEFT);
		slider->callback(&sliderCallback, this );
		slider->value(0);
		slider->bounds(-1,1);
		animationUnitSlider = slider;
		
		auDropdown->value(0);
		dropdownChanged(auDropdown->label(), auDropdown->value());
	}
	
	interfaceElements->end();

	
	
	end();
}

CDWindow::~CDWindow()
{
	delete faceWindowFront;
	delete faceWindowSide;
	delete animationUnitSlider;
	delete shapeUnitSlider;
}

void CDWindow::resize(int x, int y, int w, int h)
{
	Fl_Window::resize(x,y,w,h);

	// resize the face window
	//faceWindow->position(w/2-faceWindow->w()/2, h/2-faceWindow->h()/2);
}

void CDWindow::dropdownChanged( string dropdownName, int index )
{
	CDApp* app = CDApp::getInstance();
	if ( dropdownName == "Shape Unit" ) {
		selectedShapeUnitName = app->getFaceData().getShapeUnitNames().at(index);
		shapeUnitSlider->value(app->getFaceData().getShapeUnitValue(selectedShapeUnitName));
	} else if ( dropdownName == "Animation Unit" ) {
		selectedAnimationUnitName = app->getFaceData().getAnimationUnitNames().at(index);
		animationUnitSlider->value(app->getFaceData().getAnimationUnitValue(selectedAnimationUnitName));
	}
	
}

void CDWindow::_dropdownChanged( Fl_Widget* widget, void* p )
{
	CDWindow* window = (CDWindow*)p;//dynamic_cast<CDWindow*>(p);
	if ( window ) {
		Fl_Choice* choice = (Fl_Choice*)widget;
		window->dropdownChanged(choice->label(), choice->value());
	}
}

void CDWindow::sliderChanged( string sliderName, double newValue )
{
	CDApp* app = CDApp::getInstance();
	if ( sliderName == "AUSlider" ) {
		app->getFaceData().setAnimationUnitValue(selectedAnimationUnitName, newValue);
	} else if ( sliderName == "SUSlider" ) {
		app->getFaceData().setShapeUnitValue(selectedShapeUnitName, newValue);
	}
	faceWindowFront->redraw();
	faceWindowSide->redraw();
}

