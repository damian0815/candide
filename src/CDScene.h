//
//  CDScene.h
//  candide
//
//  Created by damian on 06/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDScene__
#define __candide__CDScene__

#include <iostream>
#include "CDMesh.h"
#include "picojson.h"

#include "CDMeanValueMeshDeformer.h"
#include "CDStupidMeshDeformer.h"

#include <glm/glm.hpp>

class CDScene
{
public:
	
	CDScene();
	
	void clear();
	
	void draw();
	
	void setBackgroundMeshPath( const std::string& path );
	
	glm::mat4 getBackgroundMeshTransform() { return backgroundMeshTransform; }
	void setBackgroundMeshTransform( glm::mat4 transform );
	
	void deserialize( const picojson::value& source );
	picojson::value serialize();
	
private:
	
	CDMesh backgroundMesh;
	std::string backgroundMeshPath;
	glm::mat4 backgroundMeshTransform;
	
	CDStupidMeshDeformer stupidMeshDeformer;
	CDMeanValueMeshDeformer meshDeformer;
	
};

#endif /* defined(__candide__CDScene__) */
