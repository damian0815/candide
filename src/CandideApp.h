//
//  CandideApp.h
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CandideApp__
#define __candide__CandideApp__

#include <iostream>

class CandideApp
{
public:
	CandideApp( int argc, const char* argv[] );
	
	static CandideApp* getInstance();
	
	int run();
	
private:
	
};

#endif /* defined(__candide__CandideApp__) */
