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
	
	const std::string& getBackgroundMeshPath() const { return backgroundMeshPath; }
	void loadBackgroundMesh( const std::string& path );
	
	glm::mat4 getBackgroundMeshTransform() const { return backgroundMeshTransform; }
	void setBackgroundMeshTransform( glm::mat4 transform );
	
	void deserialize( const picojson::value& source );
	picojson::value serialize() const;
	
	/*! @abstract Create a baked background mesh using the current background mesh and the current FaceData settings.
	 @discussion A 'baked' background mesh is a background mesh that will deform along with the FaceData deformation. */
	void bakeBackgroundMesh();
	picojson::value serializeBakedBackgroundMesh() { return serializedBakedBackgroundMesh; }
	void deserializeBakedBackgroundMesh( const picojson::value& value );

	void clearBakedBackgroundMesh();
	
	/*! @brief Bake front and side textures into the current bakedBackgroundMesh. */
	void bakeTexturesToBakedBackgroundMesh( const std::string& frontWindowTexturePath, const glm::mat4& frontWindowTextureTransform, const std::string& sideWindowTexturePath, const glm::mat4& sideWindowTextureTransform );
	
private:
	
	void bakeBackgroundMesh( const CDMesh& faceControlMeshForMVD );
	void serializeBakedBackgroundMeshInternal( const CDFaceData& faceData );
	// we cache this so that stuff
	picojson::value serializedBakedBackgroundMesh;
	
	void faceDataMeshChanged();
	
	bool backgroundMeshIsBaked;
	
	CDMesh backgroundMesh;
	std::string backgroundMeshPath;
	glm::mat4 backgroundMeshTransform;
	
	CDMeanValueMeshDeformer deformer;
	
	float phi;
	
};

#endif /* defined(__candide__CDScene__) */
