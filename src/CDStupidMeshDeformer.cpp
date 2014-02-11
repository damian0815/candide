//
//  CDStupidMeshDeformer.cpp
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDStupidMeshDeformer.h"

using namespace std;
using namespace glm;

void CDStupidMeshDeformer::setupDeformation(const CDMesh &_originalMesh, const CDMesh &controlMesh)
{
	originalMesh = _originalMesh;
	deformedMesh = _originalMesh;
	
	weights.clear();
	weights.resize(originalMesh.getNumVertices());
	
	for ( int i=0; i<originalMesh.getNumVertices(); i++ ) {
		
		vec3 x = originalMesh.getVertex(i);
		
		auto& xWeights = weights.at(i);
		xWeights.resize(controlMesh.getNumVertices());
		
		float totalWeight = 0;
		for ( int j=0; j<controlMesh.getNumVertices(); j++ ) {
			vec3 p = controlMesh.getVertex(j);
			
			// use delta length as weight
			vec3 delta = x-p;
			float d = length(delta);
			if ( d<FLT_EPSILON ) {
				// x==p
				for ( int k=0; k<controlMesh.getNumVertices(); k++ ) {
					xWeights[k] = 0;
				}
				xWeights[j] = 1.0f;
				totalWeight = 1.0f;
				// done
				break;
			}
					
			float weight = 1.0f/(d*d);
			xWeights[j] = weight;
			totalWeight += weight;
		}
		
		// normalise weights
		for ( int j=0; j<xWeights.size(); j++ ) {
			xWeights[j] /= totalWeight;
		}
	}
}

void CDStupidMeshDeformer::updateDeformation(const CDMesh &controlMeshDeformed)
{
	for ( int i=0; i<originalMesh.getNumVertices(); i++ ) {
		
		vec3 xDeformed(0,0,0);
		const auto& xWeights = weights.at(i);
		
		for ( int j=0; j<controlMeshDeformed.getNumVertices(); j++ ) {
			xDeformed += xWeights.at(j)*controlMeshDeformed.getVertex(j);
		}
		
		deformedMesh.setVertex(i, xDeformed);
	}
}

