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
	assert( vdata.size()==3 );
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

glm::mat4 picojson_decodeMat4( const picojson::value& source )
{
	assert( source.is<picojson::array>() );
	picojson::array mdata = source.get<picojson::array>();
	assert( mdata.size()==16 );
	return glm::mat4(mdata[0].get<double>(), mdata[1].get<double>(), mdata[2].get<double>(), mdata[3].get<double>(),
					 mdata[4].get<double>(), mdata[5].get<double>(), mdata[6].get<double>(), mdata[7].get<double>(),
					 mdata[8].get<double>(), mdata[9].get<double>(), mdata[10].get<double>(), mdata[11].get<double>(),
					 mdata[12].get<double>(), mdata[13].get<double>(), mdata[14].get<double>(), mdata[15].get<double>() );
}

picojson::value picojson_encodeMat4( const glm::mat4& mat )
{
	picojson::array mdata;
	for ( int i=0; i<16; i++ ) {
		mdata.push_back(picojson::value(mat[i/4][i%4]));
	}
	return picojson::value(mdata);
}



