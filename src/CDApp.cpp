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
#include <Fl/Fl_Shared_Image.H>
#include <assert.h>

#include "CDMeshObjWriter.h"
#include "CDMeanValueMeshDeformer.h"
#include "CDAssimpLoader.h"
#include "CDUtilities.h"
#include "CDMVMDTestWindow.h"

#define TEST_CDMEANVALUEMESHDEFORMER

using namespace glm;

static CDApp* instance;

CDApp* CDApp::getInstance()
{
	assert(instance);
	return instance;
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

CDApp::CDApp( int argc, const char* argv[] )
: faceData( CANDIDE_FILE_PATH )
{
	instance = this;
	
#ifdef TEST_CDMEANVALUEMESHDEFORMER
	window = new CDMVMDTestWindow(1000,630, "test");
#else
	window = new CDWindow(1000,630,"Candide");
#endif
	window->callback(&exitCallback);
	window->end();
	
	// register image types
	fl_register_images();
	
	// setup OpenGL
	Fl::gl_visual(FL_RGB);
	
}


void runMeanValueMeshDeformerTest()
{
	
	
	CDAssimpLoader loader;
	bool loaded = loader.loadModel("data/tetra-small.dae");
	assert(loaded);
	CDMesh original = loader.getLoadedMesh();
	
	loaded = loader.loadModel("data/tetra-big.dae");
	assert(loaded);
	CDMesh control = loader.getLoadedMesh();

	// test deformer
	CDMeanValueMeshDeformer deformer;
	deformer.setupDeformation(original, control);
	deformer.updateDeformation(control);
	CDMesh deformed = deformer.getDeformedMesh();
	
	for ( int i=0; i<deformed.getNumVertices(); i++ ) {
		vec3 vOriginal = original.getVertex(i);
		vec3 vDeformed = deformed.getVertex(i);
		
		float distance = length(vOriginal-vDeformed);
		assert( distance < 0.0001f );
	}
	
}

int CDApp::run()
{
	window->show(0,NULL);
	
	runMeanValueMeshDeformerTest();
	
	return Fl::run();
}


