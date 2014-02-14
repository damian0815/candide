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


void CDMeshMapper::setup(const CDMesh &_targetMesh, const CDMesh &sourceMesh)
{
	targetMesh = _targetMesh;
	
	// map vertices in targetMesh to vertices in sourceMesh
	sourceToTargetVertexMapping.clear();
	for ( size_t i=0; i<sourceMesh.getNumVertices(); i++ ) {
		vec3 source = sourceMesh.getVertex(i);
		// find lengths to target vertices
		sourceToTargetVertexMapping[i].clear();
		for ( size_t j=0; j<targetMesh.getNumVertices(); j++ ) {
			vec3 test = targetMesh.getVertex(j);
			float testLength = length(source-test);
			// store vertices at the same pos
			if ( testLength<1e-5 ) {
				sourceToTargetVertexMapping[i].insert(j);
			}
		}
		//assert(sourceToTargetVertexMapping.at(i).size()>0);
	}
	
	
	/*
	
	// now we know where to find vertices in the same place
	map<size_t, size_t> targetTosourceVertexMapping;
	
	for ( size_t i=0; i<sourceMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = sourceMesh.getTriangle(i);
		// find a triangle in targetMesh that matches this
		set<size_t> potentialsourceVerts[3];
		for ( int i=0; i<3; i++ ) {
			size_t targetVertex = t.v[i];
			potentialtargetVerts[i] = sourceToTargetVertexMapping[targetVertex];
		}
		
		// find triangles that match
		for ( size_t j=0; j<targetMesh.getNumTriangles(); j++ ) {
			for ( int k=0; k<3; k++ ) {
				
			if ( triangleIsPossible( targetMesh.getTriangle(j), potentialtargetVerts ) ) {
				
			}
		}
		
		
	}
		
	
	
	// map triangles in targetMesh to triangles in sourceMesh
	assert(targetMesh.getNumTriangles()>=sourceMesh.getNumTriangles());

	set<CDMesh::Triangle,TriangleCompare> sourceTriangles;
	// first put all source triangles into sourceTriangles set
	for ( size_t i=0; i<sourceMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = sourceMesh.getTriangle(i);
		assert(!sourceTriangles.count(t));
		sourceTriangles.insert(t);
	}
	
	// map of vertex to source triangle membership
	multimap<size_t, CDMesh::Triangle> targetVertexTosourceTriangleMap;
	
	// for each target triangle, find its equivalent in sourceMesh
	for ( size_t i=0; i<targetMesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = targetMesh.getTriangle(i);
		if ( sourceTriangles.count(t) ) {
			for ( int j=0; j<3; j++ ) {
				targetVertexTosourceTriangleMap.insert( t.v[0],  )
		}
		assert(sourceTriangles.count(t));
	}
	
	*/
}

void CDMeshMapper::updateTargetMeshFromSourceMesh(const CDMesh &sourceMesh)
{
	assert(sourceToTargetVertexMapping.size());
	// update our target mesh
	
	// walk through source vertices
	for ( size_t i=0; i<sourceMesh.getNumVertices(); i++ ) {
		// get the face data vertex
		vec3 pos = sourceMesh.getVertex(i);
		// find indices on target mesh to match
		for ( size_t targetMeshIdx: sourceToTargetVertexMapping.at(i) ) {
			// apply
			targetMesh.setVertex(targetMeshIdx, pos);
		}
	}
}

