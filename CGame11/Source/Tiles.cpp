#include "Tiles.h"
// We created a small Tiles class to manage the tiles, and remvoved all that extra code from the Game Class, we should try to keep the Game class tidy
// so making a new class here lets us focus all the tile functions here.



Tiles::Tiles()
{
}

Tiles::~Tiles()
{
}


// take notice, that this init code is basically the same code we had in the game class to initialise our tiles
// there are a few minor changes to allow us to access things via the Game class that calls it, and we've moved the
// arrays which held constant data into the class definition to make it a little tidier. Now our game class only needs create an instance
// of tiles and the InitTiles(this) to load and set up
// We could also move this code up to the constructor, but generally speaking if the constructor is doing a lot of things we usually have an init funciton instead
// just to keep things a bit cleaner. This is doing 3 things.. Loading&creating texure, calculating the UV coords of the tiles, and finally creating a VBO
// thats quite a lot of work, so best done in an init

bool Tiles::InitTiles(Game* G)
{



		int Grwidth, Grheight, comp;
		char* OneTileTexture =  	(char*)"images/tileset.png";  // 1st load our texture containing all our tiles
	// now lets build our buffer
	// make our texture	but just load it into the 0 entry of the array
			unsigned char* data = stbi_load(OneTileTexture, &Grwidth, &Grheight, &comp, 4);      // ask it to load 4 componants since its rgba
			TileTexture = G->MyOGLES->CreateTexture2D(Grwidth, Grheight, (char*) data);         //just pass the width and height of the graphic, and where it is located and we can make a texture
			free(data);   // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak

	// ok we loaded our texture, now lets create a GPU buffer called a VBO and stoe all the vertices we need to draw the whole map in there.


	// now we will build up our tiles in to our array but this time our array is made up of TileData, we could use a fixed size array, since we know how many tiles there are in our map
	// but lets continue to practice our use of vectors.
		std::vector<TileVectorData> NewTiles;

	// because we are using a single texture where each tile is a fraction of the whole we need to work out the new UV coordinates for each tile
	// each vertix needs a UV coordinate, so thats means each tile has 6 pairs of UV coordinates or a simpleVec2, so 16 tiles, s * 6 vertices


		simpleVec2 UVcoords[16*6 ];

	// now the uv which will depend on the tile we want to draw, this is a bit more complex as the old UV coordinates depended on 1 set of data for each time, here the data is going to be different
	// our tilemap has 2x8 tiles in each texture, so the step to the next textire is goig to be 1/8 for X and 1/2 for Y


	// now go through a loop of 2 rows, 8 colums, and 6 vertices (though they are pairs so a counter of 12)
		int StoreIndex = 0;

	for (int Y = 0; Y < 2; Y++)
	{
		for (int X = 0; X < 8; X++)
		{
			for (int k = 0; k < 12; k += 2)  // increment in 2's
				{
					float NewU =  QuadTexVals[k] + (stepX*X);
					float NewV =  QuadTexVals[k + 1] + (stepY*Y);

					UVcoords[StoreIndex].xValue = NewU;  // store then increment the index
					UVcoords[StoreIndex].yValue = NewV;
					StoreIndex++;
				} // end of k loop
		} // end of X loop

	} // end of Y loop

// now we have our UV coordinates stored ready to be used  any time that we want them

// lets make our playfiled the same way as last lesson but this time use a vector to store all the vertex offsets, and UV's.
// Start by scanning through the map, finding out which tile is to be drawn.
// Then work out the position of its 6 vectors and is UV coordinates
// store them all into a nice big array or vector (vector are better for this as they are more dynamic)
// Once we have a buffer transfer it to the GPU memory
// then when its time to write, use the GPU memory buffer
// All this setting up, takes a little bit of time, but its only ever done once...at a point where we're not watching the screen
// which means the user will never see this slow setup, but will appreciate the faster update.



	for (int Row = 0; Row < 40; Row++) // this is an outer loop we will use the Row variable to go down the grid
		{
			for (int Column = 0; Column < 64; Column++) // this is an inner loop and will go along the grid
				{
					GLint WhatTile = G->WhichMap[(Row * 64) + Column];   // what tile is it?
						// each tile has 6 vertices**, so we have to provide the data for the quad and the screen position for the shader to place it

					for(int faces = 0 ; faces < 6 ; faces++) // we have 6 faces, but the back one is never seen, so there's no point adding it, is commented out of the list
					{

						for (int i = 0; i < 6; i++)	 // we have 6 vertices per face
							{

								TileVectorData NewTile;      // create a new instance of TileVectorData...Note, do not use new.

								// now work out the  position and get the uv for this vertex
											NewTile.PositionX = TheCubeVertices[i*3+(faces*18) ];    // times 3 because we are using trip vec, 18 entries each face
											NewTile.PositionY = TheCubeVertices[i*3+(faces * 18) + 1];
								NewTile.dummyZ  = TheCubeVertices[i * 3 + (faces * 18) + 2];
								if (WhatTile == 12) NewTile.dummyZ  = TheCubeVertices[i*3+(faces*18) +2 ] -0.8f; // move the bricks back a bit
								if(WhatTile == 5 || WhatTile == 6) NewTile.dummyZ  = TheCubeVertices[i * 3 + (faces * 18) + 2] - 0.75f;  // move the water back a bit

								// send the screen position for the centre of the quad, we are repeating ourseves by sending this 6 times but it allows faster access on the GPU

											NewTile.sPositionX = Column-32;           //each
											NewTile.sPositionY = -Row+20 ;



								// next we need the UV coordinate we precalulated for this tile, we're using the same faces
														NewTile.u = UVcoords[(WhatTile* 6) + i].xValue;   // we don't need to double i, since we are using simpleVec2 which already pairs them
														NewTile.v = UVcoords[(WhatTile * 6) + i].yValue;

								NewTiles.push_back(NewTile);      // place it in the vector
							} // end of for 1-6 vertice

					}


				} // end of for column
		} // end of for row


		// now at the end of all this we have a large vector called newTiles which contains ALL the vertex and UV positions for all the tiles, in one big chunk of memory
		// we can choose to use this over and over again, which will be fast, but faster still would be to send this to a GPU buffer called a VBO and use that when we
		// draw tiles, making the whole draw process simply a case of setting up a few uniforms and then doing a single draw call.

	// setting up a VBO is 3 steps
	// 1 ask the GPU to make one for us
		glGenBuffers(1, &TileVBO);
	// 2 bind it
		glBindBuffer(GL_ARRAY_BUFFER, TileVBO);
	// 3 transfer the data to it
		glBufferData(GL_ARRAY_BUFFER, NewTiles.size() * 7 * sizeof(float), &NewTiles.at(0), GL_STATIC_DRAW);

	// all done, it seems like a lot of work, but it will be good
	// we will see the benefit of this when we do 1 draw call to draw all our objects...But...there is an even FASTER way to draw these tiles.. using indexing, but thats something you can research

	// Something else to consider, when we created the arrays and vectors, we did them all inside this function, so all the variables were created on the stack since we didn't
	// create them using new.
	// so when we finish, they will all be cleared and we don't have to clear up

	glm::mat4 mTranslationMatrix(1.0f);  // we are not going to move any of them
	glm::mat4 mRotationMatrix(1.0f);  // or rotate them
	glm::mat4 mScaleMatrix(1.0f);  // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR, SCALEFACTOR, SCALEFACTOR);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);

	glUseProgram(G->MyOGLES->ProgramObject3D);
// these values never change so lets set them up
	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
	// we need access to some values in our shader, these values have been added to our tiles class
	positionLoc = glGetAttribLocation(G->MyOGLES->ProgramObject3D, "a_position");
	ScreenPositionLoc = glGetAttribLocation(G->MyOGLES->ProgramObject3D, "a_screenPosition");       // actually an offset I hope
	texCoordLoc = glGetAttribLocation(G->MyOGLES->ProgramObject3D, "a_texCoord");
	samplerLoc = glGetUniformLocation(G->MyOGLES->ProgramObject3D, "s_texture");
	MVPLoc = 	glGetUniformLocation(G->MyOGLES->ProgramObject3D, "MVP");
	OffsetLoc = 	glGetUniformLocation(G->MyOGLES->FastProgramObject, "u_Offsets");  // we won't use this right away

		return true;
}


// if we were to animate or change some tiles as part of game play it would be done here in this update
bool Tiles::Update(Game* G)
{
	return true;
}


// now our tiles are no longer flat, we need s slightly different render system, which takes into account the camera's view point
// and a projection matrix and also consider our maps own Model Matrix,  these three matrices combined are known as an MVP Matrix

// we also have a slightly different shader which can use these matrices so we don't feed it quite the same
// info as before, and to save some time addressing the GPU we now set things up in the init routine, since they never change, updating them
// each cycle is wasteful, especially when the need to talk to the GPU

bool Tiles::Render(Game* G)
{

	glUseProgram(G->MyOGLES->ProgramObject3D);  // it was created in the OGL init, so lets keep using it

// 1st things 1st, lets create an MVP Model is already set up with an Identity I matrix, which is the matrix equivilent of 0
// our map never really moves in space, but our camera might so this needs to be updated each cycle

	glm::mat4 MVPMatrix = *G->TheCamera.GetProjection() * *G->TheCamera.GetView() * Model;   // our camera conveniently gives us what we need

// did you notice that despite being called an MVP we multipled in the order PVM, this is because compilers can't tell the mathamatical precident order when things are all the same type
// so it does them in the order they are on the stack, when we do this calc,the P then the V  then the M are put on the stack, so the 1st thing it sees then is the M, then the V, then P
// remember that though we are abstracting this to P*V*M to make it look like a simple calculation, there are in fact a massive number of calcuations going on, so we don't want to do this unless we have to

	// let the shaders know what VBO and texture to use
	glBindBuffer(GL_ARRAY_BUFFER, TileVBO);
	glBindTexture(GL_TEXTURE_2D, TileTexture);    // we kept the texture in its own class this time

// load the constant uniform values to the shader, in this case the MVP
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVPMatrix[0][0]);


	GLuint stride = 7*sizeof(float);   // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
		3,		// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		(const void*)0  // where do they start as an offset in the VBO); // use 3 values, but add stride each time to get to the next
		);

	glVertexAttribPointer(ScreenPositionLoc,
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they a float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

	// Load the texture coordinate
	glVertexAttribPointer(texCoordLoc,
		2, // there are 2 values u and c
		GL_FLOAT,	 //they are floats
		GL_FALSE,	 // we don't need them normalised
		stride, 	  // whats the stride to the next ones
		(const void*)(sizeof(float) * 5)
		);

	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(texCoordLoc);
	glEnableVertexAttribArray(ScreenPositionLoc);

// if you get errors,uncomment the glGetError lines to try to find where
//	if (glGetError() != GL_NO_ERROR) printf("We got a setup error\n");  // its a good idea to test for errors.

// we "could" work out just the numbers of tiles we want to print and draw only a section, but
// in fact the calculation is simple but it increases the number of draw calls, which is never a good idea
// our map really isn't massive, so letting it draw it all, even if not shown is acceptable
	glDrawArrays(GL_TRIANGLES, 0, 40 * 64 * 6 * 6 ); // 40*64 tiles, 6 vertices per face, 6 faces // thats actually 77K polys :D maybe not so acceptable :D


//	if(glGetError() != GL_NO_ERROR) printf("We got a draw error\n");    // its a good idea to test for errors.
// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);
	glDisableVertexAttribArray(ScreenPositionLoc);

//	if (glGetError() != GL_NO_ERROR) printf("We got a clear up error\n");     // its a good idea to test for errors.
	return true;
}
