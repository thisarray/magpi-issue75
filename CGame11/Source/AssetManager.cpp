

#include  "AssetManager.h"
#include "stb_image.h"

AssetManager::AssetManager()
{
	
}
AssetManager::~AssetManager()
{ // now think about this, we may have a large collection of graphics, or other assets under control of this manager
 // if we eve delete the manager, we need to tidy up 
// try doing it yourself?
}
	

// the primary purpose of this is to load assets, for the moment we only have 1 kind of asset we want to load so it is just going to do that
// later we will try to differentaite assets based on their filename and call different loaders as we need/write them
// its also a good plan to call the specific loader for a specific asset type
GLuint AssetManager::LoadAsset(char* fname)
{
	// only doing textures so just load them
	return LoadandCreateTexture(fname); 
	
}

GLuint AssetManager::LoadandCreateTexture(char* fname)//returns the id of the texture
{
//our 1st task is to scan through the map and see if this fname is already loaded	
	if (Totaltextures.find(fname) == Totaltextures.end())
	{
		 // so we didn't find it that means we gotta make a new texture
		GLuint texture_id;
		int w, h, comp;

					
		char *cstr = &fname[0u];  // embarrasing side effect of the decision to use char.....
		unsigned char* data = stbi_load(fname, &w, &h, &comp, 4);   

		if (!data) {
			printf("Unable to load texture:%s \n", cstr);
			exit(1); // here we are going to exit the program, but we could just allow it to continue...which will produce unpredictable results but will work
		}
// lets report the data we got from the load process, its not really needed and can only be seen in a console, but if we have faulty graphics, we might see the error in this output				
		printf("Texture info for %s :-\n", cstr);
		printf("# width   : %d\n", w);
		printf("# Height  : %d\n", h);
		printf("# comp    : %d\n", comp);


		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					
// we might load a couple of different types of data, this comp value infroms us if we have alpha values and can use a different form of texture create					
		if (comp == 4)	// check if we had an alpha texture or a plane (strictly speaking any value not 3 is wrong)	
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
		else
		{
			if (comp != 3) printf("Made a default non alpha texture, comp was :%d\n", comp);
				
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	
		}
					
		if (glGetError() != GL_NO_ERROR) 
		{
			printf("Oh bugger:- Model texture creation error, but attempting to carry on.\n");						
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		free(data);  // release the cpu memory once its been put in the GPU. THIS is important, if we don't we end up with this memory being unused and lost.
// now we make an associate with this file name, and the id we got from making the texture, and store it in our map.		
		Totaltextures.insert(std::make_pair(fname, texture_id));

		return texture_id;
	}
// we discovered we have this fname in the map, so lets return the texture we associate with ti	
	printf("texture already exists, not reloaded\n"); // give a little notice to the coder, you can remove this when you release the code
	return Totaltextures[fname];
}
	
//textures need to be deleted here	
bool AssetManager::deleteTexture(GLuint IDofTexture) // removes from Texture space by ID, and from the map, return false if not possible
{
	
}
bool AssetManager::deleteTexture(char* FileNameOfTexture) // removes from Texture space by name, and from the map 
{
	
}
	
