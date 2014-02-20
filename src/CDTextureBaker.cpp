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
	
	// setup scaling factor
	//vec3 bbSize = sourceMeshBaked.getBoundingBoxSize();
	
	// image goes from -0.5*imageAspect..-0.5*imageAspect in x
	// and from -0.5..0.5 in y
	
	
	
	/*float width = bbSize.x;
	float height = bbSize.y;
	float sx = 1.0f/width;
	float sy = 1.0f/height;
	float scale = min(sx,sy);
	if ( textureAspect>1.0f ) {
		sx = scale/textureAspect;
		sy = scale;
	} else {
		sx = scale;
		sy = scale*textureAspect;
	}*/
	float sx = 1.0f/textureAspect;
	float sy = 1.0f;
	
	/*vec3 bbCenter = sourceMeshBaked.getBoundingBoxCenter();
	float ox = -bbCenter.x;
	float oy = -bbCenter.y;*/
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
		if ( (i%50)==0 ) {
			cout << "from "<<v.x<<","<<v.y<<" got " <<st.x<<","<<st.y<<endl;
		}
		
		/*calculate s,t :
		s = x * kX + ox;
		t = y * kY + oY;
		 where kX, kY are scaling factors to bring CDMesh space to frontImageSpace, and oX and oY centre the image at (0,0) applying the transform*/
	}
	
	
	return output;
}
