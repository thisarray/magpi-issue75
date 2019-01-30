#include "PointToPoint.h"


PointToPoint::PointToPoint()

{
	char* ImageNames[] = {
		(char*)"Assets/fungus_1.png",
		(char*)"Assets/fungus_2.png",
		(char*)"Assets/fungus_3.png",
		(char*)"Assets/fungus_4.png"
	};

// lets turn these all these graphic files into textures, but consider, is it good to have them all as seperate textures?
for(int i = 0 ; i < 4 ; i++)
	{
		// We've now replaced the manual load of images becuase we create this serveral times, meaning we might load the same graphics
		// so we'll let the asset manager handle the loading and check if we already have it
		Images[i] = SimpleObj::TheOGLInstance->TheAssetManager.LoadAsset(ImageNames[i]);

		// which means the 1st time we create a P2P object we will load these graphics, but the second time we wont....saving a lot of space and also avoiding a slow load process that
		// really isn't needed

	}

	BaseAnim = 2;  // 0 is static frame 1 is s stand, 2 and 3 are the walk frames
	Yspeed = Xspeed = 0;
	AnimIndex = 0;
	Dir = Right;
	TextureID = Images[0];

	ScaleX = 32.0;  // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;
}



PointToPoint::~PointToPoint()
{
}
// this is a small helper function to let us set up a point to point
// it will allow is to set the startung or minx point and a distance  from that p
// where he should turn back

void PointToPoint::SetXPoints(int minx, int count, Direction d)
{
	Xmin = minx;
	Xmax = minx + count;
	Dir = d;
}

// this is the main update funciton for a point to point
// we've included the ability to do up and down, but not yet coded it
// why not find some nice graphics and try adding a flying object that moves up and down?

bool PointToPoint::Update(Game* G)
{
	float Speed = 1.2;

	Yspeed += 1.2f; // always add gravity to the yspeed, make corrections when you know if you land on something

	AnimIndex++;  // we will discover that just incrementing the index works, but it switches too fast, how can we make that more effective?

	if(AnimIndex > 1) AnimIndex = 0;

	switch(Dir)
	{
	case Left:
		{
			Xpos -= Speed;
			if (floor(Xpos) <= Xmin) Dir = Right;  // we don't check for any obstruction since he's placed were it is known there are no obstrucitons
													// but to make him more intelligent and allow changes in the map, a test for solid obstructions is a good ad.
			break;
		}
	case Right:
		{
			Xpos += Speed;
			if (floor(Xpos) >= Xmax) Dir = Left;
			break;
		}
	case Up:
		{
			break;
		}
	case Down:
		{
			break;
		}

	default:
		{
			printf("default occured, setting direction to Left \n");
			Dir = Left;
		}
	} // switch dir

// check for gravity and get the frame
	if(Yspeed > 9.81f / 4) Yspeed = 9.81f / 4;
	Ypos += Yspeed;

	int YMap = (Ypos + 16) / 16;
	int XMap = (Xpos) / 16;


	int WhatsUnderOurFeet = G->WhichMap[(YMap * 64) + XMap];
	int Attrib = G->Attributes[WhatsUnderOurFeet];

// if we find we landed on something, null the speed and set our feet on top of the blocks
	if ((Attrib & SOLID) && Yspeed >= 0)
	{
		Ypos = YMap * 16 - 16;
		Yspeed = 0;
	}

	this->TextureID = Images[BaseAnim + AnimIndex];

	return true;
}

void PointToPoint::Draw()
{

		/**************************************************************************************
	This is the exact same code we use to Draw Bob, but we may choose to draw our baddies in a different
	way at some point so for now using the code this way opens up options to us later.
		***************************************************************************************/

	glBindTexture(GL_TEXTURE_2D, TextureID);        // we kept the texture in its own class this time
	glUseProgram(GameLoc->MyOGLES->ProgramObject3D);     // it was created in the OGL init, so lets keep using it

// 1st things 1st, lets create an MVP Model is already set up with an Identity I matrix, which is the matrix equivilent of 0
// our map never really moves in space, but our camera might so this needs to be updated each cycle

	glm::mat4 mRotationMatrix(1.0f);      // or rotate them
	glm::mat4 mScaleMatrix(1.0f);      // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR * 2, SCALEFACTOR * 2, SCALEFACTOR * 2);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	glm::mat4 mTranslationMatrix(1.0f);      // we are not going to move any of them
	mTranslationMatrix[3][0] = -32.5 + (Xpos / 16);
	mTranslationMatrix[3][1] = 20.5 + (-this->Ypos / 16);
	mTranslationMatrix[3][2] =  0.62f;
	;

	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
	glm::mat4 MVPMatrix = *GameLoc->TheCamera.GetProjection() * *GameLoc->TheCamera.GetView() * Model;      // our camera conveniently gives us what we need

// did you notice that despite being called an MVP we multipled in the order PVM, this is because compilers can't tell the mathamatical precident order when things are all the same type
// so it does them in the order they are on the stack, when we do this calc,the P then the V  then the M are put on the stack, so the 1st thing it sees then is the M, then the V, then P
// remember that though we are abstracting this to P*V*M to make it look like a simple calculation, there are in fact a massive number of calcuations going on, so we don't want to do this unless we have to

	// let the shaders know what VBO and texture to use
// load the constant uniform values to the shader, in this case the MVP
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVPMatrix[0][0]);

	GLuint stride = 5*sizeof(float);   // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
		3,		// there are 3 values xyz
		GL_FLOAT, // they are float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		(const void*)0  // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);

	glVertexAttribPointer(texCoordLoc,
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they are float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

	// but we will still ask it to use the same position and texture attributes locations
			glEnableVertexAttribArray(positionLoc);       // when we enable it, the shader can now use it and it starts at its base value
			glEnableVertexAttribArray(texCoordLoc);       // when we enable it, the shader can now use it
			// now its been set up, tell it to draw 6 vertices which make up a square
			glDrawArrays(GL_TRIANGLES, 0, 6);

	if (glGetError() != GL_NO_ERROR) printf(" Point to Point draw errors\n");
	// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);


}
