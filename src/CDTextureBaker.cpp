//
//  CDTextureBaker.cpp
//  candide
//
//  Created by damian on 17/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDTextureBaker.h"
#include "CDFBO.h"
#include "CDImageLoader.h"
#include <FL/Fl_Shared_Image.H>
#include <glm/gtc/matrix_transform.hpp>
#include "CDAppException.h"

using namespace glm;
using namespace std;

CDTextureBaker::CDTextureBaker( int width, int height )
: outputWidth(width), outputHeight(height), leftImage(0), frontImage(0)
{
	
}

CDTextureBaker::~CDTextureBaker()
{
}

void CDTextureBaker::setFrontImage(const std::string &path, const glm::mat4 &transform)
{
	frontImage = Fl_Shared_Image::get(path.c_str());
	frontImageTransform = transform;
}
void CDTextureBaker::setLeftImage(const std::string &path, const glm::mat4 &transform)
{
	leftImage = Fl_Shared_Image::get(path.c_str());
	leftImageTransform = transform;
}

CDMesh CDTextureBaker::bake( const CDMesh& sourceMeshBaked, const std::string& textureOutputPath )
{
	// copy the source mesh
	CDMesh mesh(sourceMeshBaked);
	mesh.updateNormals();
	
	// for now we just use the front image
	

	
	// make FBO and bind
	float aspect = outputWidth/outputHeight;
	CDFBO fbo( outputWidth, outputHeight );
	fbo.bind();
	
	glClearColor( 0, 0, 0, 0 );
	glClear(GL_COLOR_BUFFER_BIT);
	
	// make ortho matrix
	glMatrixMode(GL_PROJECTION);
	glOrtho( 0, aspect, 0, 1, -1000, 1000);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// draw the thing
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	if ( frontImage && frontImage->w() )
		drawUVMap( mesh, frontImage, frontImageTransform, mat4() );
	if ( leftImage && leftImage->w() )
		drawUVMap( mesh, leftImage, leftImageTransform, glm::rotate( mat4(), -90.0f, vec3(0,1,0) ) );
	
	
	// save
	fbo.saveToTGA(textureOutputPath);
	
	fbo.unbind();
	
	
	return mesh;
}

void CDTextureBaker::drawUVMap( const CDMesh& mesh, Fl_Shared_Image* image, const glm::mat4& imageTransform, const glm::mat4& modelTransform )
{
	// bind front texture
	GLuint tex = CDImageLoader::createOpenGLTextureFromImage(image);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );
	
	// image goes from -0.5*imageAspect..-0.5*imageAspect in x
	// and from -0.5..0.5 in y
	
	// generate front tex coords
	vector< vec2 > bakeyTexCoords;
	
	float textureAspect = (float)image->w()/(float)image->h();
	float sx = 1.0f/textureAspect;
	float sy = 1.0f;
	float ox = 0;
	float oy = 0;
	for ( size_t i=0; i<mesh.getNumVertices(); i++ ) {
		// transform each vertex in the mesh by the modelTransform
		vec3 v = mesh.getVertex(i);
		vec4 v4(v.x,v.y,v.z,1);
		v4 = v4*modelTransform;
		v = vec3(v4.x,v4.y,v4.z);
		
		// calculate s,t
		float s = (v.x+ox);
		float t = (v.y+oy);
		vec4 st4( s, t, 0, 1 );
		// apply inverse image transform
		st4 = glm::inverse(imageTransform)*st4;
		// apply scale to compensate for image aspect
		vec2 st(st4.x*sx+0.5,1.0f-st4.y*sy+0.5);
		
		bakeyTexCoords.push_back(st);
	}
	
	// go through the triangles
	vector< vec2 > uvMapTriangleVertices;
	vector< vec2 > uvMapTexCoords;
	vector< vec4 > uvMapColors;
	if ( mesh.getNumTextureCoordinates()<mesh.getNumVertices() ) {
		throw new CDAppException("not enough texture coordinates");
	}
	vec4 refVector(0,0,-1,1);
	refVector = refVector*modelTransform;
	for ( int i=0; i<mesh.getNumTriangles(); i++ ) {
		// get st for each corner
		const CDMesh::Triangle& t = mesh.getTriangle(i);

		for ( int j=0; j<3; j++ ) {
			size_t vIdx = t.v[j];
			vec2 st = mesh.getTextureCoordinate(vIdx);
			uvMapTriangleVertices.push_back(st);
			uvMapTexCoords.push_back(bakeyTexCoords.at(vIdx));
			
			// get alpha by finding the dot product of the normal with a front-on vector
			vec3 normal = mesh.getNormal(vIdx);
			// kill the y component
			normal.y = 0;
			// renormalize
			normal = normalize(normal);
			// go
			float alpha = glm::dot(vec3(refVector.x,refVector.y,refVector.z),normal);
			
			uvMapColors.push_back(vec4(1,1,1,alpha));
		}
	}
	
	
	glColor4f(1,1,1,1);
	// draw
	glEnableClientState(GL_VERTEX_ARRAY);
	/*
	GLfloat testTri[6] = { 0.1, 0.1, 0.5, 0.9, 0.9, 0.1 };
	glVertexPointer(2, GL_FLOAT, 0, testTri);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	*/
	
	glVertexPointer(2, GL_FLOAT, 0, &uvMapTriangleVertices.at(0)[0]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvMapTexCoords.at(0)[0]);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, &uvMapColors.at(0)[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)uvMapTriangleVertices.size());
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &tex);
	
}
