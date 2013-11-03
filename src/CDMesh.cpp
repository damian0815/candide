//
//  CDMesh.cpp
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDMesh.h"

#include <Fl/gl.h>

using namespace glm;

void CDMesh::addFace( int v0, int v1, int v2 )
{
	assert(v0<vertices.size() && "v0 out of bounds");
	assert(v1<vertices.size() && "v1 out of bounds");
	assert(v2<vertices.size() && "v2 out of bounds");
	
	triangles.push_back(Triangle(v0,v1,v2));
	
}

void CDMesh::draw()
{
	
	/*
	glBegin(GL_LINES);
	for ( int i=0; i<triangles.size(); i++ ) {
		const Triangle& t = triangles[i];
		for ( int j=0; j<3; j++ ) {
			const vec3& u = vertices[t.v[j]];
			glVertex3f( u.x, u.y, u.z );
			const vec3& v = vertices[t.v[(j+1)%3]];
			glVertex3f( v.x, v.y, v.z );
		}
	}
	glEnd();
	
	return;*/
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0][0] );
	
	if ( wireframe ) {
		short lines[triangles.size()*3*2];
		for ( int i=0; i<triangles.size(); i++ ) {
			lines[i*6+0] = triangles[i].v[0];
			lines[i*6+1] = triangles[i].v[1];
			lines[i*6+2] = triangles[i].v[1];
			lines[i*6+3] = triangles[i].v[2];
			lines[i*6+4] = triangles[i].v[2];
			lines[i*6+5] = triangles[i].v[0];
		}
		glDrawElements( GL_LINES, (GLsizei)triangles.size()*3*2, GL_UNSIGNED_SHORT, &lines[0] );
	} else {
		glDrawElements( GL_TRIANGLES, (GLsizei)triangles.size()*3, GL_UNSIGNED_SHORT, &triangles[0].v[0]);
	}
	
	glDisableClientState( GL_VERTEX_ARRAY );
}

void CDMesh::getBoundingBox( vec3& minCorner, vec3& maxCorner )
{
	for ( int i=0; i<vertices.size(); i++ ) {
		const vec3& v = vertices[i];
		if ( i==0 ) {
			minCorner = v;
			maxCorner = v;
		} else {
			minCorner.x = min(v.x,minCorner.x);
			minCorner.y = min(v.y,minCorner.y);
			minCorner.z = min(v.z,minCorner.z);
			maxCorner.x = max(v.x,maxCorner.x);
			maxCorner.y = max(v.y,maxCorner.y);
			maxCorner.z = max(v.z,maxCorner.z);
		}
	}
}

vec3 CDMesh::getBoundingBoxCenter()
{
	vec3 minCorner, maxCorner;
	getBoundingBox(minCorner,maxCorner);
	return (minCorner+maxCorner)*0.5f;
}

vec3 CDMesh::getBoundingBoxSize()
{
	vec3 minCorner, maxCorner;
	getBoundingBox(minCorner,maxCorner);
	return maxCorner-minCorner;
}

