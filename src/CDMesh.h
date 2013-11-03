//
//  CDMesh.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDMesh__
#define __candide__CDMesh__

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <Fl/gl.h>

class CDFaceDistortionUnit;

class CDMesh
{
	friend class CDFaceDistortionUnit;
	
public:
	CDMesh(): wireframe(true) {};
	
	void clear() { vertices.clear(); triangles.clear(); }
	
	void addVertex( const glm::vec3 &v ) { vertices.push_back(v); }
	void addFace( int v0, int v1, int v2 );
	void draw();
	
	struct Triangle {
		GLushort v[3];
		Triangle(): Triangle(0,0,0) {};
		Triangle( short v0, short v1, short v2 ) { v[0] = v0; v[1] = v1; v[2] = v2; }
	};
	
	size_t getNumVertices() { return vertices.size(); }
	
	glm::vec3 getBoundingBoxCenter();
	glm::vec3 getBoundingBoxSize();
	void getBoundingBox( glm::vec3& minCornerOut, glm::vec3& maxCornerOut );

private:
	
	
	bool wireframe;
	std::vector<glm::vec3> vertices;
	std::vector<Triangle> triangles;
	
};

#endif /* defined(__candide__CDMesh__) */
