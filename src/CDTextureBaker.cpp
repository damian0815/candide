//
//  CDTextureBaker.cpp
//  candide
//
//  Created by damian on 17/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDTextureBaker.h"

using namespace glm;
using namespace std;

CDMesh CDTextureBaker::bake( const CDMesh& sourceMeshBaked, const glm::mat4& textureTransform, float textureAspect, const std::string& textureOutputPath )
{
	// copy the source mesh
	CDMesh output(sourceMeshBaked);
	
	// for now we just use the front image
	
	// image goes from -0.5*imageAspect..-0.5*imageAspect in x
	// and from -0.5..0.5 in y

	float sx = 1.0f/textureAspect;
	float sy = 1.0f;

	float ox = 0;
	float oy = 0;
	
	
	for ( size_t i=0; i<output.getNumVertices(); i++ ) {
		vec3 v = output.getVertex(i);
		
		float s = (v.x+ox)*sx;
		float t = (v.y+oy)*sy;
		vec4 st4( s, t, 0, 1 );
		st4 = glm::inverse(textureTransform)*st4;
		vec2 st(st4.x+0.5,st4.y+0.5);
		output.addTextureCoordinate(st);
		
	}
	
	
	return output;
}
