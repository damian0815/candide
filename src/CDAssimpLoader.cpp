//
//  CDAssimpLoader.cpp
//  candide
//
//  Created by damian on 03/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#include "CDAssimpLoader.h"

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

using namespace std;
using namespace glm;

#include "CDAppException.h"
#include "CDUtilities.h"

bool CDAssimpLoader::loadModel( const std::string& path )
{
	loadedMesh.clear();
	Assimp::Importer importer;
	
	// load
	const unsigned int flags =  aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_RemoveComponent;
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_NORMALS );
	
	const aiScene* scene = importer.ReadFile(path.c_str(),flags);
	
	if ( !scene ) {
		CDLog << "failed to load scene from " << path << ": error is " << importer.GetErrorString();
		return false;
	}
	
	// process
	if ( scene->mNumMeshes==0 ) {
		CDLog << "No meshes found in loaded scene";
		return false;
	}
	
	if ( scene->mNumMeshes>1 ) {
		CDLog << ">1 meshes detected ("<<scene->mNumMeshes<<"): ";
		for ( int i=0; i<scene->mNumMeshes; i++ ) {
			aiMesh* mesh = scene->mMeshes[i];
			CDLog << i << ": '" << mesh->mName.C_Str() <<"' ("<<mesh->mNumVertices<<" verts, "<<mesh->mNumFaces<<" faces)" << (i==0?" (loading this one)":"");
		}
	}
	
	aiMesh* firstMesh = scene->mMeshes[0];
	CDLog << "Loading mesh '" << firstMesh->mName.C_Str() <<"' ("<<firstMesh->mNumVertices<<" verts, "<<firstMesh->mNumFaces<<" faces)";
	
	for ( int i=0; i<firstMesh->mNumVertices; i++ ) {
		aiVector3D v = firstMesh->mVertices[i];
		loadedMesh.addVertex(vec3(v.x,v.y,v.z));
		// also load normals
		if ( firstMesh->HasNormals() ) {
			v = firstMesh->mNormals[i];
			loadedMesh.addVertexNormal(vec3(v.x,v.y,v.z));
		}
	}
	
	
	for ( int i=0; i<firstMesh->mNumFaces; i++ ) {
		aiFace f = firstMesh->mFaces[i];
		if ( f.mNumIndices != 3 ) {
			throw new CDAppException("can only load meshes with 3 vertices");
		}
		loadedMesh.addFace(f.mIndices[0], f.mIndices[1], f.mIndices[2]);
	}
	
	return true;
	
}



