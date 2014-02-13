//
//  CDRemappedMesh.cpp
//  candide
//
//  Created by damian on 13/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDMeshMapper.h"
#include <set>
#include <map>

using namespace glm;
using namespace std;


// functor to compare triangles
struct TriangleCompare
{
	void sortVertices( int out[3] )
	{
		int &a = out[0];
		int &b = out[1];
		int &c = out[2];
		// bubble sort :-)
		if (a > b)
			swap(a,b);
		if (b > c)
			swap(b,c);
		if (a > b)
			swap(a,b);
	}
	
	bool operator()( const CDMesh::Triangle& a, const CDMesh::Triangle& b )
	{
		// sort vertices
		int myVerts[3] = { a.v[0], a.v[1], a.v[2] };
		sortVertices(myVerts);
		int theirVerts[3] = { b.v[0], b.v[1], b.v[2] };
		sortVertices(theirVerts);
		
		if ( myVerts[0]<theirVerts[0] ) {
			return true;
		} else if ( myVerts[0]==theirVerts[0] ) {
			if ( myVerts[1]<theirVerts[1] ) {
				return true;
			} else if ( myVerts[1]==theirVerts[1] ) {
				if ( myVerts[2]<theirVerts[2] ) {
					return true;
				}
			}
		}
		return false;
	}
};


void CDMeshMapper::setup(const CDMesh &_sourceMesh, const CDMesh &targetMesh)
{
	sourceMesh = _sourceMesh;
	
	// map vertices in sourceMesh to vertices in targetMesh
	targetToSourceVertexMapping.clear();
	for ( size_t i=0; i<targetMesh.getNumVertices(); i++ ) {
		vec3 target = targetMesh.getVertex(i);
		// find lengths to source vertices
		targetToSourceVertexMapping[i].clear();
		for ( size_t j=0; j<sourceMesh.getNumVertices(); j++ ) {
			vec3 test = sourceMesh.getVertex(j);
			float testLength = length(target-test);
			// store vertices at the same pos
			if ( testLength<1e-5 ) {
				targetToSourceVertexMapping[i].insert(j);
			}
		}
		//assert(targetToSourceVertexMapping.at(i).size()>0);
	}
	
	
	/*
	
	// now we know where to find vertices in the same place
	map<size_t, size_t> sourceToTargetVertexMapping;
	
	for ( size_t i=0; i<targetMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = targetMesh.getTriangle(i);
		// find a triangle in sourceMesh that matches this
		set<size_t> potentialTargetVerts[3];
		for ( int i=0; i<3; i++ ) {
			size_t sourceVertex = t.v[i];
			potentialSourceVerts[i] = targetToSourceVertexMapping[sourceVertex];
		}
		
		// find triangles that match
		for ( size_t j=0; j<sourceMesh.getNumTriangles(); j++ ) {
			for ( int k=0; k<3; k++ ) {
				
			if ( triangleIsPossible( sourceMesh.getTriangle(j), potentialSourceVerts ) ) {
				
			}
		}
		
		
	}
		
	
	
	// map triangles in sourceMesh to triangles in targetMesh
	assert(sourceMesh.getNumTriangles()>=targetMesh.getNumTriangles());

	set<CDMesh::Triangle,TriangleCompare> targetTriangles;
	// first put all target triangles into targetTriangles set
	for ( size_t i=0; i<targetMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = targetMesh.getTriangle(i);
		assert(!targetTriangles.count(t));
		targetTriangles.insert(t);
	}
	
	// map of vertex to target triangle membership
	multimap<size_t, CDMesh::Triangle> sourceVertexToTargetTriangleMap;
	
	// for each source triangle, find its equivalent in targetMesh
	for ( size_t i=0; i<sourceMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = sourceMesh.getTriangle(i);
		if ( targetTriangles.count(t) ) {
			for ( int j=0; j<3; j++ ) {
				sourceVertexToTargetTriangleMap.insert( t.v[0],  )
		}
		assert(targetTriangles.count(t));
	}
	
	*/
}

void CDMeshMapper::updateSourceMeshFromTargetMesh(const CDMesh &targetMesh)
{
	assert(targetToSourceVertexMapping.size());
	// update our source mesh
	
	// walk through target vertices
	for ( size_t i=0; i<targetMesh.getNumVertices(); i++ ) {
		// get the face data vertex
		vec3 pos = targetMesh.getVertex(i);
		// find indices on source mesh to match
		for ( size_t sourceMeshIdx: targetToSourceVertexMapping.at(i) ) {
			// apply
			sourceMesh.setVertex(sourceMeshIdx, pos);
		}
	}
}

