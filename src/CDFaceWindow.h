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
	
	/*! @abstract Use model as background 3D model. Does not take ownership. */
	void setBackground3DModel( CDMesh* model );
	/*! @abstract Load model and take ownership. */
	void loadBackground3DModel( const std::string& modelFilename );
	/*! @abstract Return background model without changing ownership. */
	CDMesh* getBackground3DModel() { return backgroundMesh; }
	
	void setModelTransform( glm::mat4 _transform ) { transform = _transform; }
	void draw();
	
	/*! @abstract Handle mouse clicks (from Fl_Gl_Window) */
	int handle(int code);
	
protected:
	
private:
	CDFaceData* faceData;
	
	CDMesh* backgroundMesh;
	bool ownsBackgroundMesh;
	
	std::string backgroundTexturePath;
	GLuint backgroundTexture;
	int backgroundTextureW, backgroundTextureH;
	
	glm::mat4 transform;
	
	glm::vec3 bgImageTranslate;
	float bgImageScale;
	
	glm::vec2 dragPrev;
};

#endif /* defined(__candide__FaceWindow__) */
