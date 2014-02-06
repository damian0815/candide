//
//  CDFaceWindow.cpp
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include "CDFaceWindow.h"
#include <Fl/math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <sigc++/sigc++.h>

#include "CDUtilities.h"
#include "CDImageLoader.h"
#include "CDAssimpLoader.h"

using namespace glm;
using namespace std;
using namespace picojson;

using namespace sigc;


CDFaceWindow::CDFaceWindow(int x, int y, int w, int h, CDFaceData* _faceData )
: Fl_Gl_Window(x, y, w, h, "FaceGL")
, faceData(_faceData), backgroundTexture(0), bgImageScale(1.0f), lightX(0)
{
	valid(0);

	Fl::add_timeout(1.0f/60.0f, &CDFaceWindow::_updateCallback, this );
	
}

CDFaceWindow::~CDFaceWindow()
{
	if ( backgroundTexture ) {
		glDeleteTextures(1, &backgroundTexture);
		backgroundTexture = 0;
	}
}

void CDFaceWindow::clear()
{
	setBackgroundImage("");
	setBackground3DModel("");
	backgroundMeshTransform = mat4();
	bgImageScale = 1.0f;
	bgImageTranslate = vec3();
}

void CDFaceWindow::setBackgroundImage( const string& path )
{
	if ( backgroundTexture ) {
		glDeleteTextures(1, &backgroundTexture);
		backgroundTexture = 0;
	}
	backgroundTexturePath = path;
	if ( path.size() ) {
		setBackground3DModel("");
	}
	redraw();
}

void CDFaceWindow::setBackground3DModel(const std::string &modelFilename)
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
		setBackgroundImage("");
		
		backgroundMesh = loader.getLoadedMesh();
		
		// build a default background mesh transform matrix
		mat4 bgMeshTransformDefault;
		
		// scale to fit in screen bounds
		vec3 meshBBSize = backgroundMesh.getBoundingBoxSize();
		float screenAspectRatio = (float)w()/(float)h();
		float scaleX = (2.0f*screenAspectRatio)/meshBBSize.x;
		float scaleY = 2.0f/meshBBSize.y;
		float scale = std::min(scaleX,scaleY);
		bgMeshTransformDefault = glm::scale(bgMeshTransformDefault, vec3(scale,scale,scale));

		// offset so that center=0,0,0
		vec3 meshBBCenter = backgroundMesh.getBoundingBoxCenter();
		bgMeshTransformDefault = glm::translate(bgMeshTransformDefault, -meshBBCenter);
		
		backgroundMeshTransform = bgMeshTransformDefault;
		
	}
	redraw();
}



void CDFaceWindow::draw()
{
	if ( !valid() ) {
		valid(1);
		// set viewport size
		glViewport(0,0,w(),h());
		
		// load orthogonal projection matrix
		float aspectRatio = (float)w()/(float)h();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-aspectRatio, aspectRatio, -1, 1, -100, 100);
		
	}
	
	if ( backgroundTexture==0 && !backgroundTexturePath.empty() ) {
		// load the texture
		backgroundTexture = CDImageLoader::createOpenGLTextureFromImage(backgroundTexturePath, backgroundTextureW, backgroundTextureH);
		if ( 0==backgroundTexture ) {
			cerr<<"couldn't load texture from '"<<backgroundTexturePath<<"'"<<endl;
			backgroundTexturePath = "";
		}
	}
	
	glClearColor(0,0,0,0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glColor4f(1,1,1,0.5);
	
	
	// translate and scale to fit
	vec3 translate;
	float scale;
	faceData->calculateCompensatoryTranslateScale(vec3(0,0,0), vec3(0.95,0.95,0.95), translate, scale);
	glTranslatef(translate.x,translate.y,translate.z);
	glScalef(scale,scale,scale);
	
	if ( backgroundTexture ) {
		
		glPushMatrix();
		
		glTranslatef(bgImageTranslate.x, bgImageTranslate.y, bgImageTranslate.z);
		glScalef(bgImageScale, bgImageScale, 1);
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);
		
		// draw the background image
		
		float imageAspect = ((float)backgroundTextureW/(float)backgroundTextureH);
		GLfloat imageW = 1.0f*imageAspect;
		GLfloat imageH = 1.0f;
		
		glBegin(GL_QUADS);
		glVertex2f(imageW/2, -imageH/2);
		glTexCoord2f(0, 1);
		glVertex2f(-imageW/2, -imageH/2);
		glTexCoord2f(0, 0);
		glVertex2f(-imageW/2, imageH/2);
		glTexCoord2f(1, 0);
		glVertex2f(imageW/2, imageH/2);
		glTexCoord2f(1, 1);
		glEnd();
		
		glDisable(GL_TEXTURE_2D);
		
		glPopMatrix();
	}
		
	// apply compensatory translate and scale
	glMultMatrixf(&transform[0][0]);
	
	
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
		backgroundMesh.draw(false);
		backgroundMesh.drawBoundingBox();
		// also draw bounding box
		
		glDisable(GL_DEPTH_TEST);
		
		glPopMatrix();
	}
	

	// draw the actual facedata
	faceData->draw();
	
}

void CDFaceWindow::update()
{
	if ( backgroundMesh.getNumVertices() ) {
		lightX += 0.01f;
	}
	redraw();
}



int CDFaceWindow::handle(int code)
{
	if ( code == FL_PUSH ) {
		// store drag start
		dragPrev = vec2(Fl::event_x(), Fl::event_y());
		backgroundMeshTransformAtDragStart = backgroundMeshTransform;
		return 1;
	}
	
	if ( code == FL_DRAG ) {
		vec2 dragCurr = vec2(Fl::event_x(), Fl::event_y());
		
		// convert to screen units
		float screenUnitScale = 2.0f/h();
		
		
		// alt down means do a zoom
		if ( Fl::event_alt() ) {
			
			if ( backgroundTexture ) {
				// calculate distance to centre of image
				vec2 screenCenter = vec2(bgImageTranslate.x/screenUnitScale + w()/2,
										 -bgImageTranslate.y/screenUnitScale + h()/2);
				
				float prevScreenCenterDistance = glm::length(dragPrev-screenCenter);
				float currentScreenCenterDistance = glm::length(dragCurr-screenCenter);
				if ( currentScreenCenterDistance>0 && currentScreenCenterDistance>0 ) {
					float scale = currentScreenCenterDistance/prevScreenCenterDistance;
					bgImageScale *= scale;
				}
				dragPrev = dragCurr;
			} else {
				// calculate distance to centre of view
				vec2 screenCenter = vec2(w()/2,h()/2);
				
				float prevScreenCenterDistance = glm::length(dragPrev-screenCenter);
				float currentScreenCenterDistance = glm::length(dragCurr-screenCenter);
				if ( currentScreenCenterDistance>0 && currentScreenCenterDistance>0 ) {
					float scale = currentScreenCenterDistance/prevScreenCenterDistance;
					mat4 scaleMat = glm::scale(scale,scale,scale);
					backgroundMeshTransform = scaleMat*backgroundMeshTransform;
					backgroundMeshTransformUpdatedSignal.emit( this, backgroundMeshTransform );
				}
				dragPrev = dragCurr;
			}
			
		} else if ( Fl::event_command() ) {
			
			vec2 dragDelta = dragCurr-dragPrev;
			float angleX=0, angleY=0, angleZ=0;
			float angleScaleFactor = 2.0f; // 2 pixel = 2 degrees
			// round to nearest whole number of pixels
			angleY = roundf(dragDelta.x/2.0f)*2.0f*angleScaleFactor;
			angleX = roundf(dragDelta.y/2.0f)*2.0f*angleScaleFactor;
			
			mat4 rotation = rotate(angleZ,vec3(0,0,1))*rotate(angleY,vec3(0,1,0))*rotate(angleX,vec3(1,0,0));
			backgroundMeshTransform = rotation*backgroundMeshTransformAtDragStart;
			
			// signal to listeners
			backgroundMeshTransformUpdatedSignal.emit( this, backgroundMeshTransform );
			
			
			
		} else {
			
			vec2 dragDelta = dragCurr-dragPrev;
			// glOrtho(-aspectRatio, aspectRatio, -1, 1, -100, 100);
			// therefore
			dragDelta *= screenUnitScale;
			
			vec3 translationToAdd( dragDelta.x, -dragDelta.y, 0 );
			bgImageTranslate += translationToAdd;
			
			// for the background mesh, we need to transform the translation by this window's transform
			vec4 transformedTranslation = vec4(translationToAdd.x,translationToAdd.y,translationToAdd.z,1.0)*transform;
			mat4 translate = glm::translate( vec3(transformedTranslation.x,transformedTranslation.y,transformedTranslation.z) );
			backgroundMeshTransform = translate*backgroundMeshTransform;
			backgroundMeshTransformUpdatedSignal.emit( this, backgroundMeshTransform );
			
			dragPrev = dragCurr;
		}
			
		redraw();

		return 1;
	}
	
	// fallback
	return Fl_Gl_Window::handle(code);
		
		
}

void CDFaceWindow::setBackground3dModelTransform(const glm::mat4 &transform)
{
	backgroundMeshTransform = transform;
	//CDLog << "Got";
	redraw();
}

#pragma mark - Serialization/deserialization

value CDFaceWindow::serialize()
{
	object root;
	
	// background texture
	object bgImage;
	bgImage["path"] = value(backgroundTexturePath);
	bgImage["translate"] = picojson_encodeVector3( bgImageTranslate );
	bgImage["scale"] = value(bgImageScale);
	
	root["bgImage"] = value(bgImage);
	
	object bgMesh;
	bgMesh["path"] = value(backgroundMeshPath);
	bgMesh["transform"] = picojson_encodeMat4( backgroundMeshTransform );
	
	root["bgMesh"] = value(bgMesh);
	
	
	return value(root);
}

void CDFaceWindow::deserialize(const picojson::value &source)
{
	clear();
	object root = source.get<object>();
	
	// load bg image
	object bgImage = root["bgImage"].get<object>();
	backgroundTexturePath = bgImage["path"].get<string>();
	bgImageScale = bgImage["scale"].get<double>();
	bgImageTranslate = picojson_decodeVector3(bgImage["translate"]);
	
	// also load bgmesh, if any
	if ( root.count("bgMesh") ) {
		object bgMesh = root["bgMesh"].get<object>();
		
		string path = bgMesh["path"].get<string>();
		setBackground3DModel(path);
		backgroundMeshTransform = picojson_decodeMat4(bgMesh["transform"]);
	}
	
	redraw();
}






