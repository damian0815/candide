//
//  CDUtilities.cpp
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDUtilities.h"

glm::vec3 picojson_decodeVector3( const picojson::value& source )
{
	assert( source.is<picojson::array>() );
	picojson::array vdata = source.get<picojson::array>();
	float x = vdata[0].get<double>();
	float y = vdata[1].get<double>();
	float z = vdata[2].get<double>();
	return glm::vec3(x,y,z);
}

picojson::value picojson_encodeVector3( const glm::vec3& vec )
{
	picojson::array vdata;
	vdata.push_back(picojson::value(vec.x));
	vdata.push_back(picojson::value(vec.y));
	vdata.push_back(picojson::value(vec.z));
	return picojson::value(vdata);
}



