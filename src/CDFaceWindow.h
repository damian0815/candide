//
//  CDFaceWindow.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__FaceWindow__
#define __candide__FaceWindow__

#include <iostream>
#include <FL/Fl.h>

#include <FL/gl.h>
#include <FL/Fl_Gl_Window.h>

#include <glm/glm.hpp>
#include <sigc++/sigc++.h>

#include "picojson.h"

#include "CDFaceData.h"



class CDFaceWindow : public Fl_Gl_Window
{
public:
	CDFaceWindow( int x, int y, int w, int h, CDFaceData* faceData);
	~CDFaceWindow();
	
	void deserialize( const picojson::value& source );
	picojson::value serialize();
	
	void setBackgroundImage( const std::string& pngFilename );
	void setBackground3DModel( const std::string& modelFilename );
	
	void setModelTransform( glm::mat4 _transform ) { transform = _transform; }
	void draw();
	
	/*! @abstract Handle mouse clicks (from Fl_Gl_Window) */
	int handle(int code);
	
	void connectToBackgroundMeshTransformUpdatedSignal( CDFaceWindow* otherWindow );
protected:
	
private:
	
	sigc::signal<void, glm::mat4> backgroundMeshTransformUpdatedSignal;
	void backgroundMeshTransformUpdatedInOtherWindow( glm::mat4 transform );
	
	CDFaceData* faceData;
	
	CDMesh backgroundMesh;
	glm::mat4 backgroundMeshTransform;
	glm::mat4 backgroundMeshTransformAtDragStart;
	
	
	std::string backgroundTexturePath;
	GLuint backgroundTexture;
	int backgroundTextureW, backgroundTextureH;
	
	glm::mat4 transform;
	
	glm::vec3 bgImageTranslate;
	float bgImageScale;
	
	glm::vec2 dragPrev;
	
};

#endif /* defined(__candide__FaceWindow__) */
