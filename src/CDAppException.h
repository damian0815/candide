//
//  CDAppException.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CandideAppException__
#define __candide__CandideAppException__

#include <iostream>

class CDAppException : public std::runtime_error
{
public:
	CDAppException( std::string description ) : std::runtime_error("CDAppException: "+description) {};
	
};

#endif /* defined(__candide__CandideAppException__) */
