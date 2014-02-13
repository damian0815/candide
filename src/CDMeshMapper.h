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
	
	bool isSetup() { return sourceMesh.getNumVertices()>0; }
	
	/*! @abstract Setup with the given meshes. 
	 @discussion 
		sourceMesh's vertices will be mapped to targetMesh's vertices. */
	void setup( const CDMesh& sourceMesh, const CDMesh& targetMesh );
	
	/*! @abstract Move vertices in sourceMesh based on targetMesh and based on the previously setup mapping. */
	void updateSourceMeshFromTargetMesh( const CDMesh& targetMesh );
	
	/*! @abstract Return the source mesh at its current state. */
	const CDMesh& getSourceMesh() { return sourceMesh; }
	
private:
	
	// map from target vertices (the Candide control mesh) to source vertices (the control mesh for the mean value deformer)
	std::map<size_t, std::set<size_t> > targetToSourceVertexMapping;
	
	CDMesh sourceMesh;
};

#endif /* defined(__candide__CDRemappedMesh__) */
