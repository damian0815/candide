//
//  CandideApp.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CandideApp.h"
#include "CandideWindow.h"

#include <Fl/Fl.h>
#include <assert.h>

static CandideApp* instance = NULL;

CandideApp* CandideApp::getInstance()
{
	assert(instance);
	return instance;
}

CandideApp::CandideApp( int argc, const char* argv[] )
{
	instance = this;
}


static void exitCallback( Fl_Widget* widget )
{
	// disable escape key
	if (Fl::event()==FL_SHORTCUT && Fl::event_key()==FL_Escape) {
		// do nothing
	} else {
		exit(0);
	}
}

int CandideApp::run()
{
	Fl_Window window(300,330);
	
	CandideWindow cw(0,0,300,300);
	window.resizable(cw);
	
	window.callback(&exitCallback);
	
	window.end();
	window.show(0,NULL);
	
	return Fl::run();
}


