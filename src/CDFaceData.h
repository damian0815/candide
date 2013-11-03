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
#include "CDMesh.h"

class CDFaceData
{
public:
	/// filename is in wfm format, see candide3
	CDFaceData( const std::string &path );
	

	void draw( const glm::vec3& centerPos, const glm::vec3& fitSize );
	
private:
	
	
	void load( const std::string& path );
	
	CDMesh meshAtRest;
};


#endif /* defined(__candide__CandideFaceData__) */
