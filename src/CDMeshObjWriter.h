//
//  CDMeshObjWriter.h
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDMeshObjWriter__
#define __candide__CDMeshObjWriter__

#include <iostream>
#include "CDMesh.h"


class CDMeshObjWriter
{
public:
	static void writeObj( const CDMesh& mesh, const std::string& path );
};


#endif /* defined(__candide__CDMeshObjWriter__) */
