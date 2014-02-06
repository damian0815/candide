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
, faceData(_faceData), backgroundTexture(0), bgImageScale(1.0f)
{
	valid(0);
}

CDFaceWindow::~CDFaceWindow()
{
	if ( backgroundTexture ) {
		glDeleteTextures(1, &backgroundTexture);
		backgroundTexture = 0;
	}
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
	glClear( GL_COLOR_BUFFER_BIT );
	
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
		
		glMultMatrixf(&backgroundMeshTransform[0][0]);
		backgroundMesh.draw(true);
		// also draw bounding box
		
		glPopMatrix();
	}

	// draw the actual facedata
	faceData->draw();
	
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
			
			// calculate rest distance to centre of view
			vec2 screenCenter = vec2(bgImageTranslate.x/screenUnitScale + w()/2,
									 -bgImageTranslate.y/screenUnitScale + h()/2);
			
			float prevScreenCenterDistance = glm::length(dragPrev-screenCenter);
			float currentScreenCenterDistance = glm::length(dragCurr-screenCenter);
			if ( currentScreenCenterDistance>0 && currentScreenCenterDistance>0 ) {
				float scale = currentScreenCenterDistance/prevScreenCenterDistance;
				bgImageScale *= scale;
			}
			dragPrev = dragCurr;
			
		} else if ( Fl::event_command() ) {
			
			vec2 dragDelta = dragCurr-dragPrev;
			float angleX=0, angleY=0, angleZ=0;
			float angleScaleFactor = 3.0f; // 1 pixel = 3 degrees
			angleY = dragDelta.x*angleScaleFactor;
			angleX = dragDelta.y*angleScaleFactor;
			
			mat4 rotation = rotate(angleZ,vec3(0,0,1))*rotate(angleY,vec3(0,1,0))*rotate(angleX,vec3(1,0,0));
			backgroundMeshTransform = rotation*backgroundMeshTransformAtDragStart;
			
			// signal to listeners
			CDLog << this << " sent transform " << backgroundMeshTransform[0][0];
			backgroundMeshTransformUpdatedSignal.emit( backgroundMeshTransform );
			
			
			
		} else {
			
			vec2 dragDelta = dragCurr-dragPrev;
			// glOrtho(-aspectRatio, aspectRatio, -1, 1, -100, 100);
			// therefore
			dragDelta *= screenUnitScale;
			bgImageTranslate += vec3(dragDelta.x,-dragDelta.y,0);
			dragPrev = dragCurr;
		}
			
		redraw();

		return 1;
	}
	
	// fallback
	return Fl_Gl_Window::handle(code);
		
		
}

void CDFaceWindow::connectToBackgroundMeshTransformUpdatedSignal( CDFaceWindow* otherWindow )
{
	backgroundMeshTransformUpdatedSignal.connect( sigc::mem_fun(otherWindow, &CDFaceWindow::backgroundMeshTransformUpdatedInOtherWindow) );
}

void CDFaceWindow::backgroundMeshTransformUpdatedInOtherWindow(glm::mat4 transform)
{
	CDLog << this << " got transform " << transform[0][0];
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
	
	return value(root);
}

void CDFaceWindow::deserialize(const picojson::value &source)
{
	object root = source.get<object>();
	
	object bgImage = root["bgImage"].get<object>();
	
	backgroundTexturePath = bgImage["path"].get<string>();
	bgImageScale = bgImage["scale"].get<double>();
	bgImageTranslate = picojson_decodeVector3(bgImage["translate"]);
	
	redraw();
}





