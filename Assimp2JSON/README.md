Demos-and-Tools
===============

Assimp2JSON is a small utility to convert 3D models that can be read by Assimp (http://assimp.sourceforge.net/) into JSON format to be later used in WebGL apps.

To execute:
	assimp2json 3dmodelfile jsonfile

The structure of the JSON file is very simple. It contatins information regarding vertex positions, texcoords and normals, as well as materials per mesh, check out the json files provided.

For an example of a WebGL page reading these JSON formatted models check out: http://www.lighthouse3d.com/2013/07/webgl-importing-a-json-formatted-3d-model/
