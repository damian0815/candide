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

using namespace std;

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
	ifstream infile(path);
	if ( !infile.is_open() || infile.bad() ) {
		throw CDAppException("failed to load "+path);
	}
	
	istringstream line = loadNextNonComment(infile);
	
	// load the vertex count
	int vertCount;
	line >> vertCount;
	
	for ( int i=0; i<vertCount; i++ ) {
		// load vertices
		float x, y, z;
		infile >> x >> y >> z;
	}
	
	
	
}

