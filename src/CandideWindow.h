//
//  CandideWindow.h
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

#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>


class CandideWindow : public Fl_Gl_Window
{
public:
	CandideWindow( int x, int y, int w, int h);
	
	void draw();
};



#endif /* defined(__candide__CandideWindow__) */
