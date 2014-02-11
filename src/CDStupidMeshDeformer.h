//
//  CDStupidMeshDeformer.h
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDStupidMeshDeformer__
#define __candide__CDStupidMeshDeformer__

#include <iostream>
#include "CDMesh.h"

class CDStupidMeshDeformer
{
public:
	CDStupidMeshDeformer() {};
	
	void setupDeformation( const CDMesh& originalMesh, const CDMesh& controlMesh );
	void updateDeformation( const CDMesh& controlMeshDeformed );
	
	const CDMesh& getDeformedMesh() { return deformedMesh; }

private:
	
	CDMesh originalMesh;
	CDMesh deformedMesh;
	
	std::vector< std::vector<float> > weights;
	
};

#endif /* defined(__candide__CDStupidMeshDeformer__) */
