//
//  CDWindow.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDWindow.h"
#include <fstream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Fl/Fl_Choice.H>
#include <Fl/Fl_Menu_Bar.H>
#include <Fl/Fl_Hor_Value_Slider.H>
#include <Fl/Fl_Native_File_Chooser.H>

#include "CDCommon.h"
#include "CDApp.h"

using namespace glm;
using namespace std;
using namespace picojson;

void CDWindow::_sliderChanged(Fl_Widget* widget, void* userData)
{
	CDWindow* cdWindow = static_cast<CDWindow*>(userData);
	if ( cdWindow ) {
		Fl_Slider* slider = dynamic_cast<Fl_Slider*>(widget);
		cdWindow->sliderChanged( slider->label(), slider->value() );
	}
}

void CDWindow::_menuChanged(Fl_Widget* widget, void* userData)
{
	CDWindow* cdWindow = static_cast<CDWindow*>(userData);
	if ( cdWindow ) {
		Fl_Menu_Bar* menu = dynamic_cast<Fl_Menu_Bar*>(widget);
		cdWindow->menuChanged( menu, menu->mvalue() );
	}
}

CDWindow::CDWindow(int w, int h, const char* label, CDFaceData* faceData )
: Fl_Window(w, h, label)
{
	int interfaceWidth = 300;
	int menuHeight = 25;
	
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0,0,w,menuHeight);
	menu->callback(&CDWindow::_menuChanged, this);
	menu->add("File/Open");
	menu->add("File/Save as...");
	menu->add("File/");
	menu->add("File/Quit");
	menu->add("View/Load front image...");
	menu->add("View/Load side image...");
	menu->add("View/Load 3d model...");
	
	
	// create the face windows
	Fl_Group* faceWindows = new Fl_Group( 10, menuHeight+10, w-interfaceWidth-30, h-menuHeight-20 );
	faceWindows->clip_children(true);
	resizable(faceWindows);
	
	int faceWindowGap = 5;
	int faceWindowWidth = (w-interfaceWidth-30-faceWindowGap)/2;
	faceWindowFront = new CDFaceWindow(10,menuHeight+10,faceWindowWidth,h-menuHeight-20, faceData);
	faceWindowSide = new CDFaceWindow(10+faceWindowWidth+faceWindowGap,menuHeight+10,faceWindowWidth,h-menuHeight-20, faceData);
	// make a 90 degree rotation about the y axis
	mat4 sideTransform = rotate(mat4(), 90.0f, vec3(0,1,0));
	faceWindowSide->setModelTransform( sideTransform );
	// cross-connect 3d model signals
	faceWindowFront->connectToBackgroundMeshTransformUpdatedSignal(faceWindowSide);
	faceWindowSide->connectToBackgroundMeshTransformUpdatedSignal(faceWindowFront);
	
	faceWindows->end();
	
	// create buttons
	int interfaceStartX = w-interfaceWidth-10;
	Fl_Group* interfaceElements = new Fl_Group( interfaceStartX, 10+menuHeight, interfaceWidth, h-20 );
	interfaceElements->resizable(0);
	interfaceElements->clip_children(true);
	// make a drop-down and a slider for the shape units
	vector<string> suNames = faceData->getShapeUnitNames();
	Fl_Choice* suDropdown = new Fl_Choice( interfaceStartX, 10+menuHeight, interfaceWidth, 20, "Shape Unit" );
	suDropdown->labeltype(FL_NORMAL_LABEL);
	suDropdown->align(FL_ALIGN_LEFT);
	suDropdown->callback(&CDWindow::_dropdownChanged, this);
	for ( const string& suName: suNames ) {
		string nameEdited = suName;
		replace(nameEdited.begin(),nameEdited.end(),'/','|');
		suDropdown->add( nameEdited.c_str(), 0, 0 );
	}
	
	if ( suNames.size()>0 ) {
		Fl_Value_Slider* slider = new Fl_Hor_Value_Slider( interfaceStartX, 35+menuHeight, interfaceWidth, 20, "SUSlider" );
		slider->labeltype(FL_NO_LABEL);
		slider->align(FL_ALIGN_LEFT);
		slider->callback(&CDWindow::_sliderChanged, this );
		slider->value(0);
		slider->bounds(-1,1);
		shapeUnitSlider = slider;
		
		suDropdown->value(0);
		dropdownChanged(suDropdown->label(), suDropdown->value());
	}
	
	vector<string> auNames = faceData->getAnimationUnitNames();
	Fl_Choice* auDropdown = new Fl_Choice( interfaceStartX, 60+menuHeight, interfaceWidth, 20, "Animation Unit" );
	auDropdown->align(FL_ALIGN_LEFT);
	auDropdown->callback(&CDWindow::_dropdownChanged, this);
	for ( const string& auName: auNames ) {
		string nameEdited = auName;
		replace(nameEdited.begin(),nameEdited.end(),'/','\\');
		auDropdown->add( nameEdited.c_str(), 0, 0 );
	}
	
	if ( auNames.size()>0 ) {
		Fl_Value_Slider* slider = new Fl_Hor_Value_Slider( interfaceStartX, 85+menuHeight, interfaceWidth, 20, "AUSlider" );
		slider->labeltype(FL_NO_LABEL);
		slider->align(FL_ALIGN_LEFT);
		slider->callback(&CDWindow::_sliderChanged, this );
		slider->value(0);
		slider->bounds(-1,1);
		animationUnitSlider = slider;
		
		auDropdown->value(0);
		dropdownChanged(auDropdown->label(), auDropdown->value());
	}
	
	interfaceElements->end();

	
	
	end();

	// force windows to get redrawn
	faceWindowFront->redraw();
	faceWindowSide->redraw();
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

static string showFileChooser( const string& title, enum Fl_Native_File_Chooser::Type type, const string& filter )
{
	Fl_Native_File_Chooser fnfc;
	fnfc.title(title.c_str());
	fnfc.type(type);
	fnfc.filter(filter.c_str());
	// show
	string selectedFile = "";
	switch( fnfc.show() ) {
		case -1:
			throw new CDAppException(fnfc.errmsg());
		case 1:
			break;
		default:
			selectedFile = fnfc.filename();
	}
	
	return selectedFile;
}

void CDWindow::menuChanged(Fl_Menu_Bar *menu, const Fl_Menu_Item *selectedItem)
{
	string selection = selectedItem->label();
	
	if ( selection == "Load front image..." || selection == "Load side image..." ) {
		string selectedFile = showFileChooser( "Select image", Fl_Native_File_Chooser::BROWSE_FILE, "Images\t*.{ping,jpg,jpeg}");
		
		if ( selectedFile.size() ) {
			if ( selection == "Load front image..." ) {
				faceWindowFront->setBackgroundImage(selectedFile);
			} else {
				faceWindowSide->setBackgroundImage(selectedFile);
			}
		}
	}
	else if ( selection == "Load 3d model..." ) {
		string selectedFile = showFileChooser( "Select 3d model", Fl_Native_File_Chooser::BROWSE_FILE, "3D Models\t*.{3ds,obj,dae,blend,ase,ifc,xgl,zgl,ply,dxf,lwo,lws,lxo,stl,x,ac,ms3d,cob,scn");
		
		if ( selectedFile.size() ) {
			faceWindowFront->setBackground3DModel(selectedFile);
			faceWindowSide->setBackground3DModel(selectedFile);
		}
		
	}
	
	else if ( selection == "Save as..." ) {
		string path = showFileChooser("Save as...", Fl_Native_File_Chooser::BROWSE_SAVE_FILE, "Candide files\t*.candide");
		if ( path.size() ) {
			
			string forceExtension = ".candide";
			if ( path.size()<forceExtension.size() || path.substr(path.size()-forceExtension.size())!=forceExtension ) {
				path += forceExtension;
			}
			
			// serialize
			value root = serialize();
			
			ofstream outfile(path);
			outfile << root.serialize();
			
			outfile.close();
			
		}
		
	}
	
	else if ( selection == "Open" ) {
		string path = showFileChooser("Open", Fl_Native_File_Chooser::BROWSE_FILE, "Candide files\t*.candide");
		if ( path.size() ) {
			ifstream infile(path);
			value root;
			infile >> root;
			infile.close();
			
			deserialize(root);
		}
	}
	
	else if ( selection == "Quit" ) {
		exit(0);
	}
		
}


value CDWindow::serialize()
{
	object root;
	
	root["faceData"] = CDApp::getInstance()->getFaceData().serialize();
	
	root["frontWindow"] = faceWindowFront->serialize();
	root["sideWindow"] = faceWindowSide->serialize();

	return value(root);
}

void CDWindow::deserialize( const value& source )
{
	object root = source.get<object>();
	
	CDApp::getInstance()->getFaceData().deserialize(root["faceData"]);
	
	faceWindowFront->deserialize(root["frontWindow"]);
	faceWindowSide->deserialize(root["sideWindow"]);
	
}


