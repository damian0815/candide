//
//  CDRemappedMesh.h
//  candide
//
//  Created by damian on 13/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDRemappedMesh__
#define __candide__CDRemappedMesh__

#include <iostream>
#include <map>
#include <set>
#include "CDMesh.h"

/*! @abstract Accessor for a mesh whose vertices are mapped to another mesh's vertices. */
class CDMeshMapper
{
public:
	
	void clear() { targetMesh.clear(); sourceToTargetVertexMapping.clear(); }
	
	bool isSetup() { return targetMesh.getNumVertices()>0; }
	
	/*! @abstract Setup with the given meshes. 
	 @discussion 
		targetMesh's vertices will be mapped to sourceMesh's vertices. */
	void setup( const CDMesh& targetMesh, const CDMesh& sourceMesh );
	
	/*! @abstract Move vertices in targetMesh based on sourceMesh and based on the previously setup mapping. */
	void updateTargetMeshFromSourceMesh( const CDMesh& sourceMesh );
	
	/*! @abstract Return the target mesh at its current state. */
	const CDMesh& getTargetMesh() { return targetMesh; }
	
private:
	
	// map from source vertices (the Candide control mesh) to target vertices (the control mesh for the mean value deformer)
	std::map<size_t, std::set<size_t> > sourceToTargetVertexMapping;
	
	CDMesh targetMesh;
};

#endif /* defined(__candide__CDRemappedMesh__) */
