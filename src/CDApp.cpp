//
//  CDApp.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDApp.h"
#include "CDWindow.h"

#include <Fl/Fl.h>
#include <assert.h>

static CDApp* instance = NULL;

CDApp* CDApp::getInstance()
{
	assert(instance);
	return instance;
}

CDApp::CDApp( int argc, const char* argv[] )
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

int CDApp::run()
{
	CDWindow window(300,330,"Candide");

	
	window.callback(&exitCallback);
	
	window.end();
	window.show(0,NULL);
	
	return Fl::run();
}


