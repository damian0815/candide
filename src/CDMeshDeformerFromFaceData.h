//
//  CDMeshDeformerFromFaceData.h
//  candide
//
//  Created by damian on 12/02/14.
//  Copyright (c) 2014 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDMeshDeformerFromFaceData__
#define __candide__CDMeshDeformerFromFaceData__

#include <iostream>
#include "CDMeanValueMeshDeformer.h"

#include "CDFaceData.h"

class CDMeshDeformerFromFaceData
{
public:
	CDMeshDeformerFromFaceData();
	
	void clear();
	
	void setup( const std::string& pathTodeformControlMeshRest, const CDFaceData& faceData, const CDMesh& meshToDeform  );
	
	void updateDeformedMesh( const CDMesh& faceDataMesh );
	const CDMesh& getDeformedMesh() { return deformer.getDeformedMesh(); }
	
	picojson::value serialize();
	
private:

	/*! @abstract Update our control mesh based on the positions in faceDataMesh */
	void updateControlMesh( const CDMesh& faceDataMesh );
	
	// The control mesh for deforming. Must be closed and with all normals pointing in the same direction.
	CDMesh controlMesh;

	// map from face data vertices to control mesh vertices
	CDMeshMapper faceDataToControlMeshVertexMapper;
	
	CDMeanValueMeshDeformer deformer;
	
};

#endif /* defined(__candide__CDMeshDeformerFromFaceData__) */
