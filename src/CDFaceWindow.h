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
	
	void setBackground3dModelTransform( const glm::mat4& transform );
	
	void setModelTransform( glm::mat4 _transform ) { transform = _transform; }
	
	void clear();
	
	void draw();
	
	/*! @abstract Handle mouse clicks (from Fl_Gl_Window) */
	int handle(int code);
	
	sigc::signal<void, CDFaceWindow*, glm::mat4> backgroundMeshTransformUpdatedSignal;
protected:
	
private:
	
	static void _updateCallback( void* windowPtr ) { ((CDFaceWindow*)windowPtr)->update(); Fl::repeat_timeout(1.0f/60.0f, &CDFaceWindow::_updateCallback, windowPtr); }
	void update();
	
	CDFaceData* faceData;
	
	CDMesh backgroundMesh;
	std::string backgroundMeshPath;
	glm::mat4 backgroundMeshTransform;
	glm::mat4 backgroundMeshTransformAtDragStart;
	
	
	std::string backgroundTexturePath;
	GLuint backgroundTexture;
	int backgroundTextureW, backgroundTextureH;
	
	glm::mat4 transform;
	
	glm::vec3 bgImageTranslate;
	float bgImageScale;
	
	float lightX;
	
	glm::vec2 dragPrev;
	
};

#endif /* defined(__candide__FaceWindow__) */
