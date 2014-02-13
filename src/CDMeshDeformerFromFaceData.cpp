//
//  CDMeshDeformerFromFaceData.cpp
//  candide
//
//  Created by damian on 12/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDMeshDeformerFromFaceData.h"
#include "CDAssimpLoader.h"
#include "CDUtilities.h"

using namespace glm;
using namespace std;

CDMeshDeformerFromFaceData::CDMeshDeformerFromFaceData()
{
}

void CDMeshDeformerFromFaceData::clear()
{
	controlMesh.clear();
	deformer.clear();
	faceDataToControlMeshVertexMapping.clear();
}

void CDMeshDeformerFromFaceData::setup( const std::string& pathToDeformControlMeshRest, const CDFaceData& faceData, const CDMesh &meshToDeform )
{
	clear();
	
	CDAssimpLoader loader;
	bool loaded = loader.loadModel(pathToDeformControlMeshRest);
	assert(loaded);
	CDMesh deformControlMeshRest = loader.getLoadedMesh();
	
	controlMesh = deformControlMeshRest;
	
	meshMapper = faceData.getControlMeshMapper();
	
	
	// setup vertex mapping between control mesh and face data mesh
	//assert(faceDataMesh.getNumVertices() == controlMesh.getNumVertices());
	const CDMesh& faceDataMesh = faceData.getOriginalMesh();
	for ( size_t i=0; i<faceDataMesh.getNumVertices(); i++ ) {
		vec3 target = faceDataMesh.getVertex(i);
		// find the nearest vertex
		float bestLength = 0;
		size_t bestIndex = 0;
		for ( size_t j=0; j<deformControlMeshRest.getNumVertices(); j++ ) {
			vec3 test = deformControlMeshRest.getVertex(j);
			// check if they're the same
			float testLength = length(target-test);
			if ( j==0 || testLength<bestLength ) {
				bestLength = testLength;
				bestIndex = j;
			}
		}
		
		faceDataToControlMeshVertexMapping[i].push_back(bestIndex);
		//CDLog << i << ": best length: "<<bestLength;
	}

	// update the controlMesh based on faceDataMeshDistorted
	updateControlMesh( faceData.getDistortedMesh() );
	
	// setup the deformer
	deformer.setupDeformation( meshToDeform, controlMesh );
}


void CDMeshDeformerFromFaceData::updateControlMesh(const CDMesh& faceDataMesh)
{
	if ( !faceDataToControlMeshVertexMapping.size() )
		return;
	
	// update our local control mesh
	for ( size_t i=0; i<faceDataMesh.getNumVertices(); i++ ) {
		// get the face data vertex
		vec3 pos = faceDataMesh.getVertex(i);
		// find index on control mesh to max
		for ( size_t controlMeshIdx: faceDataToControlMeshVertexMapping.at(i) ) {
			// apply
			controlMesh.setVertex(controlMeshIdx, pos);
		}
	}
}

void CDMeshDeformerFromFaceData::updateDeformedMesh(const CDMesh &faceDataMesh)
{
	updateControlMesh(faceDataMesh);
	
	// update
	deformer.updateDeformation(controlMesh);
}


