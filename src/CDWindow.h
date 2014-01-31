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

class Fl_Value_Slider;

#include "CDFaceWindow.h"

class CDWindow : public Fl_Window
{
public:
	CDWindow( int w, int h, const char* label, CDFaceData* faceData );
	~CDWindow();
	
	void resize( int x, int y, int w, int h);
	
	
	void sliderChanged( std::string sliderName, double newValue );
	
private:
	void dropdownChanged( std::string dropdownName, int index );
	static void _dropdownChanged( Fl_Widget* widget, void* param );

	std::string selectedAnimationUnitName, selectedShapeUnitName;
	
	Fl_Value_Slider *animationUnitSlider, *shapeUnitSlider;
	
	CDFaceWindow *faceWindowFront, *faceWindowSide;
	
};



#endif /* defined(__candide__CandideWindow__) */
