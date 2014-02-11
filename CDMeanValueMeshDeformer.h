//
//  CDMeshMeanValueDeformation.h
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDMeshMeanValueDeformation__
#define __candide__CDMeshMeanValueDeformation__

#include <iostream>
#include "CDMesh.h"
#include <map>

class CDMeanValueMeshDeformer
{
public:
	CDMeanValueMeshDeformer();
	
	void setupDeformation( const CDMesh& originalMesh, const CDMesh& controlMesh );
	
	void updateDeformation( const CDMesh& newControlMesh );
	const CDMesh& getDeformedMesh() { return deformedMesh; }
	
private:
	
	void calculateWeights( const CDMesh& controlMesh );
	
	std::vector< std::map<int,float> > weights;
	
	CDMesh originalMesh;
	CDMesh deformedMesh;
	
};



#endif /* defined(__candide__CDMeshMeanValueDeformation__) */
