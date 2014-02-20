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
class CDMesh;

class CDMeshOperation
{
public:
	/*! @abstract Transform vertices in targetMesh by transform; rebuild normals if necessary. */
	static CDMesh transform( const CDMesh& mesh, const glm::mat4 transform );
};

class CDMesh
{
	friend class CDFaceDistortionUnit;
	
public:
	CDMesh() {};
	virtual ~CDMesh() {};
	
	void clear() { vertices.clear(); triangles.clear(); }
	
	void addVertex( const glm::vec3 &v ) { vertices.push_back(v); }
	void addVertexNormal( const glm::vec3 &v ) { vertexNormals.push_back(v); }
	void addTextureCoordinate( const glm::vec2 &coord ) { textureCoordinates.push_back(coord); }
	void addFace( int v0, int v1, int v2 );
	
	void draw() const;
	void drawBoundingBox() const;
	
	struct Triangle {
		GLushort v[3];
		Triangle(): Triangle(0,0,0) {};
		Triangle( short v0, short v1, short v2 ) { v[0] = v0; v[1] = v1; v[2] = v2; }
	};
	
	size_t getNumVertices() const { return vertices.size(); }
	const glm::vec3& getVertex(size_t which) const { return vertices.at(which); }
	void setVertex(size_t which, const glm::vec3& v) { vertices[which] = v; }
	
	size_t getNumTriangles() const { return triangles.size(); }
	const Triangle& getTriangle(size_t which) const { return triangles.at(which); }
	
	size_t getNumNormals() const { return vertexNormals.size(); }
	const glm::vec3& getNormal(size_t which) const { return vertexNormals.at(which); }
	
	size_t getNumTextureCoordinates() const { return textureCoordinates.size(); }
	const glm::vec2& getTextureCoordinate( size_t i ) const { return textureCoordinates.at(i); }
	void removeAllTextureCoordinates() { textureCoordinates.clear(); }
	
	glm::vec3 getBoundingBoxCenter() const;
	glm::vec3 getBoundingBoxSize() const;
	void getBoundingBox( glm::vec3& minCornerOut, glm::vec3& maxCornerOut ) const;

	void updateNormals();
	
protected:
	
	virtual void setupArrays() const;
	virtual void teardownArrays() const;
	
private:
	
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> textureCoordinates;

	std::vector<Triangle> triangles;
	

};

#endif /* defined(__candide__CDMesh__) */
