#pragma once

/*
Somewhere to load and keep track of our textures files, since it is not desirable to have
multiple loads of the same file, a map will be used to check if the file is already in place
All texture loads will come here, and test if the texture is already in place in texture memory
Later we can expand this concept to include shaders since they can also be loade and the resulting
GLuint Handles used to identify them
*/

#pragma once

#include <GLES2/gl2.h>
#include <map>
#include <string>
#include <vector>



class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	
	// create with 
		GLuint LoadandCreateTexture(char* fname);  //returns the id of the texture, and will load a new file or return the id associated with an old file
		GLuint LoadAsset(char* fname);
	
		//textures need to be deleted here	
			bool deleteTexture(GLuint IDofTexture);  // removes from Texture space by ID, and from the map, return false if not possible
			bool deleteTexture(char* FileNameOfTexture);  // removes from Texture space by name, and from the map 
	
	
// This is a data system called a std::map, its part of the STL and is similar to an array but does not use index values, it stores pairs of data, in this case, a string and a GLuint
// you can ask it to supply you with the GLuint associated with a string. we will use filenames as strings and it will store and return the texture ID we make with a particular texture image
// as long as the file names are unique (they should be) you can simpley ask it to check if the filename has been set up, and if so, what GLuint do you have for that filename
// other types of association are possble so long as the search paramater is unique.
			std::map<std::string, GLuint> Totaltextures; 
			
};	
	
	
	
	



