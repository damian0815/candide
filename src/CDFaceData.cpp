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
#include <iostream>
#include <sstream>
#include <iterator>
#include <numeric>

#include <Fl/gl.h>

using namespace std;
using namespace glm;

CDFaceDistortionUnit::CDFaceDistortionUnit( const string& _name )
: name(_name)
{
}

void CDFaceDistortionUnit::addVertexOffset(unsigned short vIdx, glm::vec3 offset)
{
	offsets[vIdx] = offset;
}

void CDFaceDistortionUnit::apply( float amount, CDMesh& target ) const
{
	for ( const auto it: offsets ) {
		size_t vIdx = it.first;
		vec3 offset = it.second;
		target.vertices.at(vIdx) += offset*amount;
	}
}


CDFaceData::CDFaceData( const string& path )
{
	load(path);
	
}

static istringstream loadNextLine( ifstream& infile, bool skipComments=true )
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
		
		// if non-empty, check if it's a comment (if we should)
		if ( !line.empty() ) {

			string::size_type nonWSPos = line.find_first_not_of(" \r\n\t");
			if ( nonWSPos != string::npos ) {
				// comments can start with "#" or "//"
				if ( !skipComments || (line[nonWSPos] != '#' && line[nonWSPos] != '/') ) {
					break;
				}
			}
		}
	}
	return istringstream(line);
}

static void loadDistortionUnit( CDFaceDistortionUnit& unit, ifstream& infile )
{
	istringstream line;
	// name, if any, is in a comment
	// get a default name as a fallback
	line = loadNextLine(infile, false);
	string lineStr = line.str();
	string::size_type p = lineStr.find_first_of("#/");
	if ( p != string::npos ) {
		// this is a comment, it's the name of the animation unit
		if ( lineStr[p] == '/' ) {
			p++;
		}
		p = lineStr.find_first_not_of(" \t", p+1);
		string auName = lineStr.substr(p);
		unit.setName(auName);
		
		// next line
		line = loadNextLine(infile);
	}
	// targets
	int targetCount;
	line >> targetCount;
	CDLog << unit.getName() << ": " << targetCount;
	for ( int j=0; j<targetCount; j++ ) {
		line = loadNextLine(infile);
		int v;
		float x, y, z;
		line >> v >> x >> y >> z;
		CDLog << v <<": " <<x<<","<<y<<","<<z;
		unit.addVertexOffset(v,vec3(x,y,z));
	}
}

static size_t countTokens( const string& source )
{
	istringstream sourceStream(source);
	istream_iterator<string> it(sourceStream);
	// with istream_iterator, end of stream is the default constructed iterator
	istream_iterator<string> endOfStream;
	size_t count = 0;
	while ( it != endOfStream ) {
		count++;
		++it;
	}
	return count;
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
	line = loadNextLine(infile);
	int vertCount;
	line >> vertCount;
	for ( int i=0; i<vertCount; i++ ) {
		line = loadNextLine(infile);
		// load vertex
		float x, y, z;
		line >> x >> y >> z;
		// store
		meshAtRest.addVertex(vec3(x,y,z));
	}
	
	// load faces
	line = loadNextLine(infile);
	int faceCount;
	line >> faceCount;
	for ( int i=0; i<faceCount; i++) {
		line = loadNextLine(infile);
		int v0, v1, v2;
		line >> v0 >> v1 >> v2;
		//CDLog << v0 <<"," << v1 <<"," << v2;
		// store
		meshAtRest.addFace(v0,v1,v2);
	}
	
	// load animation units
	line = loadNextLine(infile);
	int auCount;
	line >> auCount;
	animationUnits.resize(auCount);
	for ( int i=0; i<auCount; i++ ) {
		animationUnits[i].setName("unnamed-"+to_string(i));
		loadDistortionUnit( animationUnits[i], infile );
	}
	
	/*
	// jump to shape units
	int suCount;
	while (true) {
		line = loadNextLine(infile);
		if ( countTokens(line.str())==1 ) {
			break;
		}
		line >> suCount;
	}*/
	
	line = loadNextLine(infile);
	int suCount;
	line >> suCount;
	// load shape units
	shapeUnits.resize(suCount);
	for ( int i=0; i<suCount; i++ ) {
		shapeUnits[i].setName("unnamed-"+to_string(i));
		loadDistortionUnit( shapeUnits[i], infile );
	}
			
	
}


void CDFaceData::draw( const vec3& center, const vec3& fitSize )
{
	glPushMatrix();
	
	CDMesh mesh = getDistortedMesh();
	
	// translate
	vec3 meshCenter = mesh.getBoundingBoxCenter();
	vec3 targetCenter = center;
	vec3 compensate = targetCenter-meshCenter;
	glTranslatef(compensate.x,compensate.y,compensate.z);
	
	// work out a scale
	vec3 size = mesh.getBoundingBoxSize();
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
	
	mesh.draw();
	
	glPopMatrix();
}

vector<string> CDFaceData::getShapeUnitNames()
{
	vector<string> names;
	for ( const auto su: shapeUnits ) {
		names.push_back(su.getName());
	}
	return names;
}

size_t CDFaceData::getIndexOfShapeUnit(const std::string &name)
{
	for ( int i=0; i<shapeUnits.size(); i++ ) {
		if ( name == shapeUnits[i].getName() ) {
			return i;
		}
	}
	throw CDAppException("shape unit "+name+" not found");
}

void CDFaceData::setShapeUnitValue( std::string suName, float value )
{
	size_t idx = getIndexOfShapeUnit(suName);
	shapeUnitSettings[idx] = value;
}


CDMesh CDFaceData::getDistortedMesh()
{
	// apply the shape units to a copy
	CDMesh distorted = meshAtRest;
	
	for ( const auto it: shapeUnitSettings ) {
		size_t unitIdx = it.first;
		float amount = it.second;
		const CDFaceDistortionUnit& unit = shapeUnits.at(unitIdx);
		unit.apply(amount,distorted);
	}
	
	return distorted;
}

