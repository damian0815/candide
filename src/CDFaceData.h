//
//  CDFaceData.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CandideFaceData__
#define __candide__CandideFaceData__

#include <iostream>

#include <string>

class CDFaceData
{
public:
	/// filename is in wfm format, see candide3
	CDFaceData( const std::string &path );
	
private:
	
	void load( const std::string& path );
	
};


#endif /* defined(__candide__CandideFaceData__) */
