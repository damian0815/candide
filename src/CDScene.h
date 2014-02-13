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
#include "CDFaceData.h"

#include "CDMeanValueMeshDeformer.h"

#include <glm/glm.hpp>

class CDScene
{
public:
	
	CDScene();
	void connectToFaceData( CDFaceData& faceData );
	
	void clear();
	
	void update(float dt);
	void draw();
	
	void setBackgroundMeshPath( const std::string& path );
	
	glm::mat4 getBackgroundMeshTransform() { return backgroundMeshTransform; }
	void setBackgroundMeshTransform( glm::mat4 transform );
	
	void deserialize( const picojson::value& source );
	picojson::value serialize();
	
	
	void bakeBackgroundMesh();
	
private:
	
	void faceDataMeshChanged();
	
	bool backgroundMeshIsBaked;
	
	CDMesh backgroundMesh;
	std::string backgroundMeshPath;
	glm::mat4 backgroundMeshTransform;
	
	CDMeanValueMeshDeformer deformer;
	
	float phi;
	
};

#endif /* defined(__candide__CDScene__) */
