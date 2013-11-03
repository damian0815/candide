//
//  CDApp.h
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CandideApp__
#define __candide__CandideApp__

#include <iostream>

class CDApp
{
public:
	CDApp( int argc, const char* argv[] );
	
	static CDApp* getInstance();
	
	int run();
	
private:
	
};

#endif /* defined(__candide__CandideApp__) */
