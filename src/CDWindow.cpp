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

#include <Fl/Fl_Button.H>
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

CDWindow::CDWindow(int w, int h, const char* label/*, CDFaceData* faceData*/ )
: Fl_Window(w, h, label)
{
	int interfaceWidth = 300;
	int menuHeight = 25;
	
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0,0,w,menuHeight);
	menu->callback(&CDWindow::_menuChanged, this);
	menu->add("File/New");
	menu->add("File/Open...");
	menu->add("File/Save");
	menu->add("File/Save as...");
	menu->add("File/");
	menu->add("File/Quit");
	
	menu->add("View/Load front image...");
	menu->add("View/Load side image...");
	menu->add("View/Load 3d model...");
	menu->add("View/Bake 3d model");
	
	// create the face windows
	Fl_Group* faceWindows = new Fl_Group( 10, menuHeight+10, w-interfaceWidth-30, h-menuHeight-20 );
	faceWindows->clip_children(true);
	resizable(faceWindows);
	
	int faceWindowGap = 5;
	int faceWindowWidth = (w-interfaceWidth-30-faceWindowGap)/2;
	faceWindowFront = new CDFaceWindow(10,menuHeight+10,faceWindowWidth,h-menuHeight-20/*, faceData*/);
	faceWindowSide = new CDFaceWindow(10+faceWindowWidth+faceWindowGap,menuHeight+10,faceWindowWidth,h-menuHeight-20/*, faceData*/);
	// make a 90 degree rotation about the y axis
	mat4 sideTransform = rotate(mat4(), 90.0f, vec3(0,1,0));
	faceWindowSide->setModelTransform( sideTransform );

	// catch 3d model signals
	faceWindowFront->backgroundMeshTransformUpdatedSignal.connect(sigc::mem_fun(this,&CDWindow::faceWindow3DModelTransformUpdated));
	faceWindowSide->backgroundMeshTransformUpdatedSignal.connect(sigc::mem_fun(this,&CDWindow::faceWindow3DModelTransformUpdated));
	/*
	for ( auto it: faceWindowFront->backgroundMeshTransformUpdatedSignal.slots() )
	{
		CDLog << it;
	}
	for ( auto it: faceWindowSide->backgroundMeshTransformUpdatedSignal.slots() )
	{
		CDLog << it;
	}*/
	
	faceWindows->end();
	
	// create buttons
	int interfaceStartX = w-interfaceWidth-10;
	Fl_Group* interfaceElements = new Fl_Group( interfaceStartX, 10+menuHeight, interfaceWidth, h-20 );
	interfaceElements->resizable(0);
	interfaceElements->clip_children(true);
	// make a drop-down and a slider for the shape units
	CDFaceData* faceData = &CDApp::getInstance()->getFaceData();
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
	shapeUnitDropdown = suDropdown;
	
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
	animationUnitDropdown = auDropdown;
	
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

void CDWindow::clear()
{
	CDApp::getInstance()->getFaceData().clearUnitValues();
	dropdownChanged("Shape Unit", shapeUnitDropdown->value());
	dropdownChanged("Animation Unit", animationUnitDropdown->value());
	
	faceWindowFront->clear();
	faceWindowSide->clear();
	CDApp::getInstance()->getScene().clear();
	
	lastPath = "";
	
	label("Candide");
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
}

static string showFileChooser( const string& title, enum Fl_Native_File_Chooser::Type type, const string& filter, const string& defaultFilename="" )
{
	Fl_Native_File_Chooser fnfc;
	fnfc.title(title.c_str());
	fnfc.type(type);
	fnfc.filter(filter.c_str());
	fnfc.preset_file(defaultFilename.c_str());
	if ( type==Fl_Native_File_Chooser::BROWSE_SAVE_FILE ) {
		fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	}
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
	
	// if we have no last path, convert save to save as..
	if ( selection == "Save" && lastPath.size()==0 )
	{
		selection = "Save as...";
	}
	
	
	if ( selection == "Load front image..." || selection == "Load side image..." ) {
		string selectedFile = showFileChooser( "Select image", Fl_Native_File_Chooser::BROWSE_FILE, "Images\t*.{ping,jpg,jpeg}");
		
		if ( selectedFile.size() ) {
			if ( selection == "Load front image..." ) {
				faceWindowFront->setBackgroundImage(selectedFile);
			} else {
				faceWindowSide->setBackgroundImage(selectedFile);
			}
			CDApp::getInstance()->getScene().setBackgroundMeshPath("");
		}
	}
	else if ( selection == "Load 3d model..." ) {
		string selectedFile = showFileChooser( "Select 3d model", Fl_Native_File_Chooser::BROWSE_FILE, "3D Models\t*.{3ds,obj,dae,blend,ase,ifc,xgl,zgl,ply,dxf,lwo,lws,lxo,stl,x,ac,ms3d,cob,scn");
		
		if ( selectedFile.size() ) {
			CDApp::getInstance()->getScene().setBackgroundMeshPath(selectedFile);
			faceWindowFront->setBackgroundImage("");
			faceWindowSide->setBackgroundImage("");
			
		}
		
	} else if ( selection == "Bake 3d model" ) {
		CDApp::getInstance()->getScene().bakeBackgroundMesh();
	}
	
	else if ( selection == "Save" ) {
		
		if ( lastPath.size() ) {
			serializeToFile(lastPath);
		}
	}
			
	
	else if ( selection == "Save as..." ) {
		string defaultPath = (lastPath.size()?lastPath:"Untitled.candide");
		string path = showFileChooser("Save as...", Fl_Native_File_Chooser::BROWSE_SAVE_FILE, "Candide files\t*.candide", defaultPath);
		if ( path.size() ) {
			
			string forceExtension = ".candide";
			if ( path.size()<forceExtension.size() || path.substr(path.size()-forceExtension.size())!=forceExtension ) {
				path += forceExtension;
			}
			
			serializeToFile(path);
	
			label((string("Candide (")+path+string(")")).c_str());
			
			lastPath = path;
		}
		
	}
	
	else if ( selection == "Open..." ) {
		string path = showFileChooser("Open...", Fl_Native_File_Chooser::BROWSE_FILE, "Candide files\t*.candide");
		if ( path.size() ) {
			ifstream infile(path);
			value root;
			infile >> root;
			infile.close();
			
			deserialize(root);
			
			label((string("Candide (")+path+string(")")).c_str());
			
			lastPath = path;
		}
	}
	
	else if ( selection == "New" ) {
		clear();
		
	}
		
	
	else if ( selection == "Quit" ) {
		exit(0);
	}
		
}

#pragma mark - Signal handling

void CDWindow::faceWindow3DModelTransformUpdated( const std::string& sourceWindow, glm::mat4 transform )
{
	//CDLog << "got transform from " << sourceWindow << ": " << transform[0][0];
	
	CDApp::getInstance()->getScene().setBackgroundMeshTransform( transform* CDApp::getInstance()->getScene().getBackgroundMeshTransform() );
	faceWindowSide->redraw();
	faceWindowFront->redraw();
	/*if ( sourceWindow == faceWindowFront ) {
		faceWindowSide->setBackground3dModelTransform(transform);
	} else {
		faceWindowFront->setBackground3dModelTransform(transform);
	}*/
}

void CDWindow::draw()
{
	Fl_Window::draw();
	faceWindowFront->redraw();
	faceWindowSide->redraw();
}

#pragma mark - Serialization

void CDWindow::serializeToFile(const string& path )
{
	// serialize
	value root = serialize();
	
	ofstream outfile(path);
	outfile << root.serialize();
	
	outfile.close();
}
		
value CDWindow::serialize()
{
	object root;
	
	root["faceData"] = CDApp::getInstance()->getFaceData().serialize();
	root["scene"] = CDApp::getInstance()->getScene().serialize();
	
	root["frontWindow"] = faceWindowFront->serialize();
	root["sideWindow"] = faceWindowSide->serialize();

	return value(root);
}

void CDWindow::deserialize( const value& source )
{
	clear();
	
	object root = source.get<object>();
	
	CDApp::getInstance()->getFaceData().deserialize(root["faceData"]);
	CDApp::getInstance()->getScene().deserialize(root["scene"]);
	
	faceWindowFront->deserialize(root["frontWindow"]);
	faceWindowSide->deserialize(root["sideWindow"]);
	
}


