//
//  CDFaceData.cpp
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDFaceData.h"
#include "CDCommon.h"

#include <fstream>

#include <Fl/gl.h>

using namespace std;
using namespace glm;

CDFaceData::CDFaceData( const string& path )
{
	load(path);
	
}

static istringstream loadNextNonComment( ifstream& infile )
{
	string line;
	while ( true ) {
		line = "";
		// read a line
		bool got = bool(getline(infile,line));
		// none left
		if ( !got ) {
			break;
		}
		
		// if non-empty, check if it's a comment
		if ( !line.empty() ) {
			string::size_type nonWSPos = line.find_first_not_of(" \r\n\t");
			if ( nonWSPos != string::npos ) {
				if ( line[nonWSPos] != '#' ) {
					break;
				}
			}
		}
	}
	return istringstream(line);
}

void CDFaceData::load( const string& path )
{
	meshAtRest.clear();
	
	ifstream infile(path);
	if ( !infile.is_open() || infile.bad() ) {
		throw CDAppException("failed to load "+path);
	}
	
	istringstream line;
	
	// load vertices
	
	// load the vertex count
	line = loadNextNonComment(infile);
	int vertCount;
	line >> vertCount;
	for ( int i=0; i<vertCount; i++ ) {
		line = loadNextNonComment(infile);
		// load vertex
		float x, y, z;
		line >> x >> y >> z;
		// store
		meshAtRest.addVertex(vec3(x,y,z));
	}
	
	// load faces
	line = loadNextNonComment(infile);
	int faceCount;
	line >> faceCount;
	for ( int i=0; i<faceCount; i++) {
		line = loadNextNonComment(infile);
		int v0, v1, v2;
		line >> v0 >> v1 >> v2;
		//CDLog << v0 <<"," << v1 <<"," << v2;
		// store
		meshAtRest.addFace(v0,v1,v2);
	}
}


void CDFaceData::draw( const vec3& center, const vec3& fitSize )
{
	glPushMatrix();
	
	// translate
	vec3 meshCenter = meshAtRest.getBoundingBoxCenter();
	vec3 targetCenter = center;
	vec3 compensate = targetCenter-meshCenter;
	glTranslatef(compensate.x,compensate.y,compensate.z);
	
	// work out a scale
	vec3 size = meshAtRest.getBoundingBoxSize();
	vec3 targetSize = fitSize;
	float scale = std::min(std::min(targetSize.x/size.x, targetSize.y/size.y), targetSize.z/size.z);
	if ( std::isnan(scale) || !isfinite(scale) ) {
		CDLog << "mesh size is likely 0 in one dimension, using scale of 1";
		scale = 1.0f;
	} else {
		// double the scale since we are scaling a box, or something
		scale *= 2.0f;
	}
	glScalef(scale,scale,scale);
	
	meshAtRest.draw();
	
	glPopMatrix();
}

