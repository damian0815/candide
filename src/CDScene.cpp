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
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;
using namespace picojson;

CDScene::CDScene()
{
}

void CDScene::clear()
{
	backgroundMesh.clear();
	backgroundMeshTransform = mat4();
	backgroundMeshPath = "";
}

void CDScene::draw()
{
	if ( backgroundMesh.getNumVertices() ) {
		glPushMatrix();
		
		glEnable(GL_DEPTH_TEST);
		
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHT0);
		GLfloat lightPos[4] = { -5, 2, 8, 1 };
		GLfloat lightAmbient[4] = { 0.2, 0.2, 0.2, 1 };
		GLfloat lightSpecular[4] = { 1, 0.8, 0.9, 1 };
		GLfloat lightDiffuse[4] = { 0.8, 0.6, 0.6, 1 };
		
		
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
		
		glEnable(GL_LIGHT1);
		lightPos[0] = -5;
		lightPos[1] = 15;
		lightPos[2] = 0;
		lightDiffuse[0] = 0.7;
		lightDiffuse[1] = 0.8;
		lightDiffuse[2] = 0.5;
		lightSpecular[0] = 0.9;
		lightSpecular[1] = 1.0;
		lightSpecular[2] = 0.8;
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
		
		//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
		//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, -1.0);
		
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 64 );
		
		glMultMatrixf(&backgroundMeshTransform[0][0]);
		//backgroundMesh.draw(false);
		// also draw bounding box
		backgroundMesh.drawBoundingBox();
		
		// draw deformed mesh
		stupidMeshDeformer.updateDeformation(CDApp::getInstance()->getFaceData().getDistortedMesh());
		stupidMeshDeformer.getDeformedMesh().draw(false);
		
		glDisable(GL_DEPTH_TEST);
		
		glPopMatrix();
	}

	// draw the actual facedata
	CDApp::getInstance()->getFaceData().draw();
	
}

void CDScene::setBackgroundMeshPath(const std::string& modelFilename)
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
		
		
		// setup the mesh deformer
		stupidMeshDeformer.setupDeformation(backgroundMesh, CDApp::getInstance()->getFaceData().getDistortedMesh());
		
		/*
		
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

picojson::value CDScene::serialize()
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
	object root = source.get<object>();
	
	object bgMesh = root["bgMesh"].get<object>();
	
	string path = bgMesh["path"].get<string>();
	setBackgroundMeshPath(path);
	backgroundMeshTransform = picojson_decodeMat4(bgMesh["transform"]);
		
}

