//
// Lighthouse3D.com Code Sample
//
// Converting 3D model files to  JSON
//
// Uses:
//  Assimp lybrary for model loading
//		http://assimp.sourceforge.net/
//
// Some parts of the code are strongly based on the Assimp 
// SimpleTextureOpenGL sample that comes with the Assimp 
// distribution, namely the code that relates to loading the model.
//
// The code was updated and modified to be compatible with 
// OpenGL 3.3 CORE version
//
// This demo was built for learning purposes only. 
// Some code could be severely optimised, but I tried to 
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want. 
//
// If you do use it I would love to hear about it. Just post a comment
// at Lighthouse3D.com

// Have Fun :-)

#ifdef _WIN32
#pragma comment(lib,"assimp.lib")
#endif



// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"


#include <math.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <limits>




// Create an instance of the Importer class
Assimp::Importer importer;

// the global Assimp scene object
const aiScene* scene = NULL;

// ----------------------------------------------------------------------------

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void get_bounding_box_for_node (const aiNode* nd, 
	aiVector3D* min, 
	aiVector3D* max)
	
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max);
	}
}


void get_bounding_box (aiVector3D* min, aiVector3D* max)
{

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode,min,max);
}

const aiScene *Import3DFromFile( const std::string& pFile)
{

	//check if file exists
	std::ifstream fin(pFile.c_str());
	if(!fin.fail()) {
		fin.close();
	}
	else{
		printf("Couldn't open file: %s\n", pFile.c_str());
		printf("%s\n", importer.GetErrorString());
		return NULL;
	}

	scene = importer.ReadFile( pFile, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if( !scene)
	{
		printf("%s\n", importer.GetErrorString());
		return NULL;
	}

	// Now we can access the file's contents.
	printf("Import of scene %s succeeded.\n",pFile.c_str());

	aiVector3D scene_min, scene_max, scene_center;
	get_bounding_box(&scene_min, &scene_max);
	float tmp;
	tmp = scene_max.x-scene_min.x;
	tmp = scene_max.y - scene_min.y > tmp?scene_max.y - scene_min.y:tmp;
	tmp = scene_max.z - scene_min.z > tmp?scene_max.z - scene_min.z:tmp;

	// We're done. Everything will be cleaned up by the importer destructor
	return scene;
}








void genJSON(const aiScene *sc, std::string outputFile) {

	std::ofstream f;
	float minX, minY, minZ, maxX, maxY, maxZ;
	float TminX,TminY,TminZ; 
	float TmaxX,TmaxY,TmaxZ;
	TminX = sc->mMeshes[0]->mVertices[0].x; TmaxX = TminX;
	TminY = sc->mMeshes[0]->mVertices[0].y; TmaxY = TminY;
	TminZ = sc->mMeshes[0]->mVertices[0].z; TmaxZ = TminZ;
	f.open(outputFile);
	f << "{\n \"model\" : [\n"; 
	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		f << "\t{\n\t\t";
		const aiMesh* mesh = sc->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		f << "\"indices\" : \n\t\t\t[";
		f << mesh->mFaces[0].mIndices[0] << "," << mesh->mFaces[0].mIndices[1] << "," << mesh->mFaces[0].mIndices[2];
		for (unsigned int t = 1; t < mesh->mNumFaces; ++t) {
			
			f << "," <<mesh->mFaces[t].mIndices[0] << "," << mesh->mFaces[t].mIndices[1] << "," << mesh->mFaces[t].mIndices[2];

		}
		f << "],\n";

		f << "\t\t\"position\" : \n\t\t\t[";

        f << mesh->mVertices[0].x << "," << mesh->mVertices[0].y << "," << mesh->mVertices[0].z <<","<< 1;
		minX = maxX = mesh->mVertices[0].x;
		minY = maxY = mesh->mVertices[0].y;
		minZ = maxZ = mesh->mVertices[0].z;
		for (unsigned int i = 1; i < mesh->mNumVertices; ++i) {

			minX = aisgl_min(minX, mesh->mVertices[i].x); maxX = aisgl_max(maxX, mesh->mVertices[i].x);
			minY = aisgl_min(minY, mesh->mVertices[i].y); maxY = aisgl_max(maxY, mesh->mVertices[i].y);
			minZ = aisgl_min(minZ, mesh->mVertices[i].z); maxZ = aisgl_max(maxZ, mesh->mVertices[i].z);
			f << ", " << mesh->mVertices[i].x << "," << mesh->mVertices[i].y << "," << mesh->mVertices[i].z << "," << 1;
		}
		TminX = aisgl_min(TminX, minX); TmaxX = aisgl_max(TmaxX, maxX);
		TminY = aisgl_min(TminY, minY); TmaxY = aisgl_max(TmaxY, maxY);
		TminZ = aisgl_min(TminZ, minZ); TmaxZ = aisgl_max(TmaxZ, maxZ);
		f << "],\n";

		if (mesh->HasNormals()) {
			f << "\t\t\"normal\" : \n\t\t\t[";

			f << mesh->mNormals[0].x << "," << mesh->mNormals[0].y << "," << mesh->mNormals[0].z ;
			for (unsigned int i = 1; i < mesh->mNumVertices; ++i)
				f << ", " << mesh->mNormals[i].x << "," << mesh->mNormals[i].y << "," << mesh->mNormals[i].z ;

			f << "],\n";
		}
		if (mesh->HasTextureCoords(0)) {
			f << "\t\t\"texCoord\" : \n\t\t\t[";

			f << mesh->mTextureCoords[0][0].x << "," << mesh->mTextureCoords[0][0].y ;
			for (unsigned int i = 1; i < mesh->mNumVertices; ++i)
				f << ", " << mesh->mTextureCoords[0][i].x << "," <<mesh->mTextureCoords[0][i].y  ;


			f << "],\n";
		}

		f << "\t\t\"material\" : \n\t\t\t{";

		aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];
		aiString texPath;	//contains filename of texture
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
				
			f << "\"texture\" : \"" << texPath.data << "\" ," ;
		}
		aiColor4D color;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
			f << "\"diffuse\" : [" << color.r << ","<< color.g << ","<< color.b << ","<< color.a << "]";
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
			f << ",\"specular\" : [" << color.r << ","<< color.g << ","<< color.b << ","<< color.a << "]";
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &color))
			f << ",\"ambient\" : [" << color.r << ","<< color.g << ","<< color.b << ","<< color.a << "]";
		float shininess = 0.0;
		unsigned int max;
		if(AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max))
			f << ",\"shininess\" : " << shininess;



		f << "},\n"; 

		f << "\t\t\"boundingBox\" : \n\t\t\t[";
			f << minX << "," << minY << "," << minZ << "," << maxX << "," << maxY << "," << maxZ;
		f << "]\n"; 
		
		if (n == sc->mNumMeshes - 1)
			f << "\n\t}\n";
		else
			f << "\n\t},\n";
	}
	f << "\n],   \"boundingBox\":\n\t\t[";
	f << TminX << "," << TminY << "," << TminZ << "," << TmaxX << "," << TmaxY << "," << TmaxZ;
	f << "] \n}";
	f.close();
	printf("File %s saved\n", outputFile.c_str());
}



// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

	if (argc != 3) {
		printf("Usage %1 3dmodelfile jsonfile\n", argv[0]);
		printf("For supported file types please visit Assimp's site\n");
		exit(0);
	}
	scene = Import3DFromFile(argv[1]);
	if (scene)
		genJSON(scene,argv[2]);
	return(0);
}

