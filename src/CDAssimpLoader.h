//
//  CDAssimpLoader.h
//  candide
//
//  Created by damian on 03/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDAssimpLoader__
#define __candide__CDAssimpLoader__

#include <iostream>

#include "CDMesh.h"

class CDAssimpLoader
{
public:
	
	bool loadModel( const std::string& path, bool joinIdenticalVertices=true );
	
	const CDMesh& getLoadedMesh() { return loadedMesh; }

private:
	
	CDMesh loadedMesh;
	
	
};

#endif /* defined(__candide__CDAssimpLoader__) */
