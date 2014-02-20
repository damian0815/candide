//
//  CDTextureBaker.h
//  candide
//
//  Created by damian on 17/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDTextureBaker__
#define __candide__CDTextureBaker__

/*! @abstract Bakes a pair of textures (front/side) onto a mesh */

#include <iostream>
#include <glm/glm.hpp>
#include "CDMesh.h"

class Fl_Shared_Image;

class CDTextureBaker
{
public:
	
	void setLeftImage( const std::string& path, const glm::mat4& transform );
	void setFrontImage( const std::string& path, const glm::mat4& transform );
	
	/*! @abstract Bakes the images into the mesh and outputs a combined texture */
	CDMesh bake( const CDMesh& mesh, const glm::mat4& textureTransform, float textureAspect, const std::string& textureOutputPath );
	
private:
	
	Fl_Shared_Image* leftImage;
	Fl_Shared_Image* frontImage;
	
	glm::mat4 leftTransform;
	glm::mat4 frontTransform;
	
};


#endif /* defined(__candide__CDTextureBaker__) */
