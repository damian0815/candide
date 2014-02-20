//
//  CDScene.cpp
//  candide
//
//  Created by damian on 06/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDScene.h"
#include "CDApp.h"
#include "CDAssimpLoader.h"
#include "CDUtilities.h"
#include "CDTextureBaker.h"
#include "CDMeshObjWriter.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;
using namespace picojson;

CDScene::CDScene()
: phi(0), backgroundMeshIsBaked(false)
{
}

void CDScene::connectToFaceData( CDFaceData& faceData )
{
	faceData.controlMeshChangedSignal.connect(sigc::mem_fun(this,&CDScene::faceDataMeshChanged));
}

void CDScene::clear()
{
	backgroundMesh.clear();
	backgroundMeshTransform = mat4();
	backgroundMeshPath = "";
	backgroundMeshIsBaked = false;
	deformer.clear();
}

void CDScene::update(float dt)
{
	phi += dt;
}

void CDScene::draw()
{
	if ( backgroundMesh.getNumVertices() ) {
		glPushMatrix();
		
		glEnable(GL_DEPTH_TEST);
		
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_BLEND);

		glEnable(GL_LIGHT0);
		// in front-on candide view
		// x = left/right (<0 = right)
		// y = up/down (<0 = up)
		// z = front/back (<0 = toward camera)
		GLfloat lightPos[4] = { 2.5, -2.0, -2.5, .1 };
		GLfloat lightAmbient[4] = { 0.1, 0.1, 0.1, 1 };
		GLfloat lightDiffuse[4] = { 0.8, 0.6, 0.6, 1.0 };
		GLfloat lightSpecular[4] = { 1, 0.8, 0.9, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001);
		
		glEnable(GL_LIGHT1);
		GLfloat light2Pos[4] = { -.5, -.5, -1.0, .1 };
		GLfloat light2Ambient[4] = { 0.1, 0.1, 0.1, 1 };
		GLfloat light2Diffuse[4] = { 0.8, 0.8, 0.7, 1.0 };
		GLfloat light2Specular[4] = { 0.95, 1.0, 0.9, 1.0 };
		glLightfv(GL_LIGHT1, GL_POSITION, light2Pos);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light2Ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light2Diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light2Specular);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001);
			

		
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 96 );
		
		if ( backgroundMeshIsBaked ) {
			deformer.getDeformedMesh().draw();
		} else {
			glMultMatrixf(&backgroundMeshTransform[0][0]);
			backgroundMesh.draw();
			// also draw bounding box
			backgroundMesh.drawBoundingBox();
		}
		
		glDisable(GL_DEPTH_TEST);
		
		glPopMatrix();
	}

	// draw the actual facedata

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// draw once with depth test disabled
	glColor4f(1,1,1,0.1);
	glDisable(GL_DEPTH_TEST);
	CDApp::getInstance()->getFaceData().draw();
	
	// draw again with depth test enabled
	glColor4f(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
	CDApp::getInstance()->getFaceData().draw();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CDScene::loadBackgroundMesh(const std::string& modelFilename)
{
	backgroundMesh.clear();
	backgroundMeshPath = modelFilename;
	if ( modelFilename.size()==0 ) {
		return;
	}
	
	CDAssimpLoader loader;
	bool success = loader.loadModel(modelFilename);
	if ( !success ) {
		CDLog << "couldn't load 3d model from " << modelFilename;
	} else {
		backgroundMesh = loader.getLoadedMesh();
		backgroundMesh.updateNormals();
		
		// build a default background mesh transform matrix
		mat4 bgMeshTransformDefault;
		
		// scale to fit in screen bounds
		vec3 meshBBSize = backgroundMesh.getBoundingBoxSize();
		float screenAspectRatio = 1.0f;
		float scaleX = (2.0f*screenAspectRatio)/meshBBSize.x;
		float scaleY = 2.0f/meshBBSize.y;
		float scale = std::min(scaleX,scaleY);
		bgMeshTransformDefault = glm::scale(bgMeshTransformDefault, vec3(scale,scale,scale));

		// offset so that center=0,0,0
		vec3 meshBBCenter = backgroundMesh.getBoundingBoxCenter();
		bgMeshTransformDefault = glm::translate(bgMeshTransformDefault, -meshBBCenter);
		
		backgroundMeshTransform = bgMeshTransformDefault;
		
		
		
		/*
		// setup the mesh deformer
		
		loader.loadModel("/Volumes/hdd/Users/damian/2.current/candide/data/candide-closed.dae");
		meshDeformer.setupDeformation(backgroundMesh, loader.getLoadedMesh());
		meshDeformer.updateDeformation(loader.getLoadedMesh());
		//meshDeformer.setupDeformation(backgroundMesh, CDApp::getInstance()->getFaceData().getDistortedMesh());
		 */
		
	}
}

void CDScene::setBackgroundMeshTransform(glm::mat4 transform)
{
	backgroundMeshTransform = transform;
}

#pragma mark - Baking background mesh

void CDScene::deserializeBakedBackgroundMesh(const picojson::value& value)
{
	// in order to bake the background mesh, we need the mesh itself, a transform to apply to it, and a FaceData instance with shape & anim units set to the given values
	
	object root = value.get<object>();
	string meshPath = root["meshPath"].get<string>();
	loadBackgroundMesh(meshPath);
	mat4 meshTransform = picojson_decodeMat4(root["meshTransform"]);
	setBackgroundMeshTransform(meshTransform);
	
	// construct faceData object
	CDFaceData faceData = CDApp::getInstance()->getFaceData();
	faceData.deserialize(root["faceData"]);
	
	// bake
	bakeBackgroundMesh( faceData.getControlMeshForMeanValueDeformation() );
	
	serializedBakedBackgroundMesh = value;
	faceDataMeshChanged();
}

void CDScene::serializeBakedBackgroundMeshInternal( const CDFaceData& faceData )
{
	object root;
	
	root["meshPath"] = value(backgroundMeshPath);
	root["meshTransform"] = picojson_encodeMat4(backgroundMeshTransform);
	root["faceData"] = faceData.serialize();
	
	serializedBakedBackgroundMesh = value(root);
}

void CDScene::clearBakedBackgroundMesh()
{
	backgroundMeshIsBaked = false;
	deformer.clear();
}

void CDScene::bakeBackgroundMesh()
{
	CDFaceData& faceData = CDApp::getInstance()->getFaceData();
	bakeBackgroundMesh( faceData.getControlMeshForMeanValueDeformation() );
	serializeBakedBackgroundMeshInternal(faceData);
}

void CDScene::bakeBackgroundMesh( const CDMesh& faceControlMeshForMVD )
{
	if ( backgroundMeshIsBaked ) {
		clearBakedBackgroundMesh();
	}
	
	// bake the background mesh's transform
	CDMesh backgroundMeshWithTransformBaked = CDMeshOperation::transform( backgroundMesh, backgroundMeshTransform);

	// setup the deformer
	deformer.setupDeformation( backgroundMeshWithTransformBaked, faceControlMeshForMVD );
		
	backgroundMeshIsBaked = true;
	
}

void CDScene::faceDataMeshChanged()
{
	deformer.updateDeformation( CDApp::getInstance()->getFaceData().getControlMeshForMeanValueDeformation() );
}

#pragma mark Baking textures

void CDScene::bakeTexturesToBakedBackgroundMesh( const std::string& frontWindowTexturePath, const glm::mat4& frontWindowTextureTransform, const std::string& sideWindowTexturePath, const glm::mat4& sideWindowTextureTransform )
{
	CDMesh bakedBackgroundMesh = deformer.getDeformedMesh();
	CDTextureBaker textureBaker( 1024, 1024 );
	textureBaker.setFrontImage( frontWindowTexturePath, frontWindowTextureTransform );
	textureBaker.setLeftImage( sideWindowTexturePath, sideWindowTextureTransform );
	CDMesh baked = textureBaker.bake( bakedBackgroundMesh, "/tmp/testbake.tga" );
	
	CDMeshObjWriter::writeObj( baked, "/tmp/testbake.obj" );
}

#pragma mark - Serialization

picojson::value CDScene::serialize() const
{
	object root;
	
	object bgMesh;
	bgMesh["path"] = value(backgroundMeshPath);
	bgMesh["transform"] = picojson_encodeMat4( backgroundMeshTransform );
	
	root["bgMesh"] = value(bgMesh);
	
	return value(root);
}

void CDScene::deserialize( const picojson::value& source )
{
	clear();
	
	object root = source.get<object>();
	
	object bgMesh = root["bgMesh"].get<object>();
	
	string path = bgMesh["path"].get<string>();
	loadBackgroundMesh(path);
	
	backgroundMeshIsBaked = false;
	backgroundMeshTransform = picojson_decodeMat4(bgMesh["transform"]);
		
}

