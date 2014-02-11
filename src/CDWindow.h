//
//  CDWindow.h
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CandideWindow__
#define __candide__CandideWindow__

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "picojson.h"

class Fl_Value_Slider;
class Fl_Menu_Bar;
class Fl_Menu_Item;
class Fl_Choice;

#include "CDFaceWindow.h"

class CDWindow : public Fl_Window
{
public:
	CDWindow( int w, int h, const char* label/*, CDFaceData* faceData */);
	~CDWindow();
	
	void resize( int x, int y, int w, int h);
	
	void clear();
	
	picojson::value serialize();
	void deserialize( const picojson::value& source );

private:
	void sliderChanged( std::string sliderName, double newValue );
	static void _sliderChanged( Fl_Widget* widget, void* param );
	
	void menuChanged( Fl_Menu_Bar* menu, const Fl_Menu_Item* selectedItem );
	static void _menuChanged( Fl_Widget* widget, void* param );
	
	void dropdownChanged( std::string dropdownName, int index );
	static void _dropdownChanged( Fl_Widget* widget, void* param );

	std::string selectedAnimationUnitName, selectedShapeUnitName;
	
	Fl_Value_Slider *animationUnitSlider, *shapeUnitSlider;
	Fl_Choice* animationUnitDropdown, *shapeUnitDropdown;
	
	CDFaceWindow *faceWindowFront, *faceWindowSide;
	void faceWindow3DModelTransformUpdated( const std::string& source, glm::mat4 transform );
	
};



#endif /* defined(__candide__CandideWindow__) */
