//
//  CDWindow.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDWindow.h"
#include "CDCommon.h"
#include <math.h>

#include <Fl/Fl_Hor_Slider.H>

using namespace std;

static const string CANDIDE_FILE_PATH = "candide3.wfm";

static void sliderCallback(Fl_Widget* widget, void* userData)
{
	CDWindow* cdWindow = static_cast<CDWindow*>(userData);
	if ( cdWindow ) {
		Fl_Slider* slider = dynamic_cast<Fl_Slider*>(widget);
		cdWindow->sliderChanged( slider->label(), slider->value() );
	}
}

CDWindow::CDWindow(int w, int h, const char* label )
: Fl_Window(w, h, label)
{
	
	CDFaceData faceData( CANDIDE_FILE_PATH );
	
	

	// make a slider for the first shape unit
	vector<string> suNames = faceData.getShapeUnitNames();
	if ( suNames.size()>0 ) {
		string suName = suNames[0];
		Fl_Slider* slider = new Fl_Slider( w-30, 10, 20, h-20, "Mouth bending" );
		slider->labeltype(FL_NO_LABEL);
		slider->align(FL_ALIGN_LEFT);
		slider->callback(&sliderCallback, this );
		slider->value(0);
		slider->bounds(1,0);
	}
	resizable(this);
	
	// create the face window
	faceWindow = new CDFaceWindow(10,10,w-120,h-20, faceData);
	resizable(faceWindow);
		
	end();
}

CDWindow::~CDWindow()
{
	delete faceWindow;
}

void CDWindow::resize(int x, int y, int w, int h)
{
	Fl_Window::resize(x,y,w,h);

	// resize the face window
	//faceWindow->position(w/2-faceWindow->w()/2, h/2-faceWindow->h()/2);
}

void CDWindow::sliderChanged( string sliderName, double newValue )
{
	CDLog << sliderName << ": " << newValue;
	faceWindow->setShapeUnit(sliderName, newValue);
	faceWindow->redraw();
}

