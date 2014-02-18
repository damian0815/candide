//
//  CDTextureBaker.cpp
//  candide
//
//  Created by damian on 17/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDTextureBaker.h"

using namespace glm;

CDMesh CDTextureBaker::bake( const CDMesh& sourceMeshBaked, const std::string& textureOutputPath )
{
	// use an identity texture transform for now
	mat4 textureTransform;
	
	CDMesh output(sourceMeshBaked);
	
	// for now we just use the front image
	
	// setup scaling factor
	vec3 bbSize = sourceMeshBaked.getBoundingBoxSize();
	float width = bbSize.x;
	float height = bbSize.y;
	float sx = 1.0f/width;
	float sy = 1.0f/height;
	float scale = min(sx,sy);
	
	vec3 bbCenter = sourceMeshBaked.getBoundingBoxCenter();
	float ox = -bbCenter.x;
	float oy = -bbCenter.y;
	
	
	for ( size_t i=0; i<output.getNumVertices(); i++ ) {
		vec3 v = output.getVertex(i);
		
		float s = (v.x+ox)*scale + 0.5f;
		float t = (v.y+oy)*scale + 0.5f;
		output.addTextureCoordinate(vec2(s,t));
		
		/*calculate s,t :
		s = x * kX + ox;
		t = y * kY + oY;
		 where kX, kY are scaling factors to bring CDMesh space to frontImageSpace, and oX and oY centre the image at (0,0) applying the transform*/
	}
	
	
	return output;
}
