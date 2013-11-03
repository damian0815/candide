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

#include "CDFaceWindow.h"

class CDWindow : public Fl_Window
{
public:
	CDWindow( int w, int h, const char* label );
	~CDWindow();
	
	void resize( int x, int y, int w, int h);
	
	void sliderChanged( std::string sliderName, double newValue );
	
private:
	CDFaceWindow* faceWindow;
};



#endif /* defined(__candide__CandideWindow__) */
