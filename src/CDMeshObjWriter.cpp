//
//  CDMeshObjWriter.cpp
//  candide
//
//  Created by damian on 07/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDMeshObjWriter.h"
#include "CDMesh.h"
#include <fstream>

using namespace glm;
using namespace std;

void CDMeshObjWriter::writeObj(const CDMesh& mesh, const std::string &path)
{
	// obj is just
	// v <x> <y> <z>
	// ...
	// f <i0> <i1> <i2>
	
	ofstream objOut(path);
	
	for ( size_t i=0; i<mesh.getNumVertices(); i++ ) {
		vec3 v = mesh.getVertex(i);
		objOut << "v " << v.x <<" " <<v.y << " " <<v.z <<endl;
	}

	for ( size_t i=0; i<mesh.getNumNormals(); i++ ) {
		vec3 v = mesh.getNormal(i);
		objOut << "vn " << v.x <<" " <<v.y << " " <<v.z <<endl;
	}
	
	bool gotTexCoords = false;
	if ( mesh.getNumTextureCoordinates()==mesh.getNumVertices() ) {
		gotTexCoords = true;
		for ( size_t i=0; i<mesh.getNumTextureCoordinates(); i++ ) {
			vec2 st = mesh.getTextureCoordinate(i);
			objOut << "vt " << st.x <<" " << st.y << endl;
		}
	}
	
	for ( size_t i=0; i<mesh.getNumTriangles(); i++ ) {
		const CDMesh::Triangle& t = mesh.getTriangle(i);
		// in OBJ file, face indices are 1-indexed
		GLushort a=t.v[0]+1;
		GLushort b=t.v[1]+1;
		GLushort c=t.v[2]+1;
		if ( gotTexCoords ) {
			objOut << "f " << a<<"/"<<a << " " << b<<"/"<<b << " " << c<<"/"<<c <<endl;
		} else {
			objOut << "f " << a << " " << b << " " << c <<endl;
		}
	}
	objOut.close();
		
}
