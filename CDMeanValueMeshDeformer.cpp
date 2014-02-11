//
//  CDMeanValueDeformation.cpp
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDMeanValueMeshDeformer.h"
#include "CDUtilities.h"

#include <glm/glm.hpp>

using namespace glm;
using namespace std;


CDMeanValueMeshDeformer::CDMeanValueMeshDeformer()
{
}

void CDMeanValueMeshDeformer::setupDeformation( const CDMesh& _originalMesh, const CDMesh& controlMesh )
{
	originalMesh = _originalMesh;
	deformedMesh = _originalMesh;
	calculateWeights(controlMesh);
}

//static const size_t interestingVertex = 5;

void CDMeanValueMeshDeformer::calculateWeights( const CDMesh& controlMesh )
{
	// from Ju, Schaefer, Warren "Mean Value Coordinates for Closed Triangular Meshes" fig. 4
	weights.clear();
	weights.resize(originalMesh.getNumVertices());
	
	for ( size_t originalMeshVIdx=0; originalMeshVIdx<originalMesh.getNumVertices(); originalMeshVIdx++ ) {
		vec3 x = originalMesh.getVertex(originalMeshVIdx);
		
		vector<vec3> u;
		vector<float> d;
		u.resize(controlMesh.getNumVertices());
		d.resize(controlMesh.getNumVertices());
		
		// get reference to the weights for this original mesh vertex
		map<int,float>& xWeights = weights.at(originalMeshVIdx);
		
		bool done = false;
		for ( size_t j=0; j<controlMesh.getNumVertices(); j++ ) {
			const vec3& Pj = controlMesh.getVertex(j);
			d[j] = length(Pj-x);
			if ( d[j]<FLT_EPSILON ) {
				// Pj == x, so bail out
				/*
				if ( originalMeshVIdx==interestingVertex ) {
					CDLog<<"Pj == x, bailing out";
				}*/
				xWeights[(int)j] = 1.0f;
				done = true;
			}
			u[j] = (Pj-x)/d[j];
		}
		if ( done ) {
			continue;
		}
		
		// next
		// for each triangle with virtices p1, p2, p3
		for ( size_t j=0; j<controlMesh.getNumTriangles(); j++ ) {
			const CDMesh::Triangle& triangle = controlMesh.getTriangle(j);
			int vIdx[3];
			for ( int k=0; k<3; k++ ) {
				vIdx[k] = triangle.v[k];
			}
#define nextK ((k+1)%3)
#define prevK ((k+2)%3)
#define nextVIdx (vIdx[nextK])
#define prevVIdx (vIdx[prevK])
			
			double l[3], theta[3];
			// h <- (sum theta)/2
			double h = 0.0f;
			for ( int k=0; k<3; k++ ) {
				l[k] = length(u[nextVIdx]-u[prevVIdx]);
				theta[k] = 2.0*asin(l[k]*0.5);
				h += theta[k];
			}
			h *= 0.5;
			
			if ( fabsf(M_PI-h) < FLT_EPSILON ) {
				/*
				if ( originalMeshVIdx==interestingVertex ) {
					CDLog<<"using 2d barycentric coords";
				}*/
				// x lies on t, use 2D barycentric coordinates
				for ( int k=0; k<3; k++ ) {
					xWeights[vIdx[k]] = sin(theta[k])*d[prevVIdx]*d[nextVIdx];
				}
				// done
				continue;
			}
			
			double c[3], s[3];
			done = false;
			for ( int k=0; k<3; k++ ) {
				c[k] = (2.0*sin(h)*sin(h-theta[k])) / (sin(theta[nextK])*sin(theta[prevK])) - 1.0;
				float det = determinant(mat3(u[vIdx[0]],u[vIdx[1]],u[vIdx[2]]));
				float sign = (det>=0?1.0f:-1.0f);
				s[k] = sign * sqrt(std::max(1.0-c[k]*c[k],0.0));
				assert( !std::isnan(s[k]) );
				
				if ( fabsf(s[k]) <= FLT_EPSILON ) {
					// x lies outside t on the same plane, ignore t
					/*
					if ( originalMeshVIdx==interestingVertex ) {
						CDLog<<"x lies outside t on the same plane->ignoring";
					}*/
					done = true;
					break;
				}
			}
			if ( done ) {
				continue;
			}
			
			for ( int k=0; k<3; k++ ) {
				float dTerm = d[vIdx[k]]*sinf(theta[nextK])*s[prevK];
				float wK = (theta[k]-c[nextK]*theta[prevK]-c[prevK]*theta[nextK]) / dTerm;
				assert( !std::isnan(wK) );
				/*
				if ( originalMeshVIdx==interestingVertex ) {
					CDLog<<"adding "<<wK;
				}*/
				xWeights[vIdx[k]] += wK;
			}
#undef nextK
#undef prevK
#undef nextVIdx
#undef prevVIdx
		}
	}
}


void CDMeanValueMeshDeformer::updateDeformation(const CDMesh &newControlMesh)
{
	assert( originalMesh.getNumVertices() == weights.size() );
	
	for ( size_t i=0; i<originalMesh.getNumVertices(); i++ ) {
		
		const map<int,float>& xWeights = weights.at(i);
		if ( xWeights.empty() ) {
			continue;
		}
		
		vec3 vDeformed(0,0,0);
		float totalWeight = 0.0f;
		
		// vDeformed = sum for all j { w[j] * pDeformed[j] } / sum for all j { w[j] }
		for ( const auto jt: xWeights ) {
			int vIdx = jt.first;
			float w = jt.second;
			
			const vec3& pDeformedJ = newControlMesh.getVertex((size_t)vIdx);
			vDeformed += w*pDeformedJ;
			totalWeight += w;
		}
		
		vDeformed /= totalWeight;
		
		assert( !std::isnan(vDeformed.x));
		assert( !std::isnan(vDeformed.y));
		assert( !std::isnan(vDeformed.z));
		deformedMesh.setVertex( i, vDeformed );
	}
	
	deformedMesh.updateNormals();
}

void CDMeanValueMeshDeformer::compareDeformedWithOriginal()
{
	for ( size_t i=0; i<originalMesh.getNumVertices(); i++ ) {
		vec3 origPos = originalMesh.getVertex(i);
		vec3 deformedPos = deformedMesh.getVertex(i);
		float length = glm::length(origPos-deformedPos);
		if ( length > 1e-5 ) {
			CDLog<<i<<": "<<length;
		}
	}
}

