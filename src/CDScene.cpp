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
			deformer.getDeformedMesh().draw(false);
		} else {
			glMultMatrixf(&backgroundMeshTransform[0][0]);
			backgroundMesh.draw(false);
			// also draw bounding box
			backgroundMesh.drawBoundingBox();
		}
		
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


void CDScene::bakeBackgroundMesh()
{
	
	string deformControlMeshPath = "data/candide-closed.dae";
	CDAssimpLoader loader;
	loader.loadModel(deformControlMeshPath);
	
	const CDMesh& controlMeshUndistorted = loader.getLoadedMesh();
	const CDMesh& faceDataMesh = CDApp::getInstance()->getFaceData().getOriginalMesh();
	const CDMesh& faceDataMeshDistorted = CDApp::getInstance()->getFaceData().getDistortedMesh();
	
	CDMesh backgroundMeshBaked = CDMeshOperation::transform( backgroundMesh, backgroundMeshTransform);
	
	deformer.setup( controlMeshUndistorted, faceDataMesh, faceDataMeshDistorted, backgroundMeshBaked );
	
	backgroundMeshIsBaked = true;
}


void CDScene::faceDataMeshChanged()
{
	deformer.updateDeformedMesh( CDApp::getInstance()->getFaceData().getDistortedMesh() );
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

