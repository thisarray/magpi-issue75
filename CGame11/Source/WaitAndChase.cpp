
#include "WaitAndChase.h"


// just like the Point to Point, we are loding the graphics in for each instance of the type we make.....is that a good idea? How can we improve that?

WaitAndChase::WaitAndChase()

{
	char* ImageNames[] = {
		(char*)"Assets/fungus_1.png",
		(char*)"Assets/fungus_2.png",
		(char*)"Assets/fungus_3.png",
		(char*)"Assets/fungus_4.png"
	};

// as we did with the P2P's we will use the asset manager to load our graphics and avoid repeated loading of the same images
	for (int i = 0; i < 4; i++)
	{
		// We've now replaced the manual load of images becuase we create this serveral times, meaning we might load the same graphics
		// so we'll let the asset manager handle the loading and check if we already have it
		Images[i] = SimpleObj::TheOGLInstance->TheAssetManager.LoadAsset(ImageNames[i]);

		// which means the 1st time we create a P2P object we will load these graphics, but the second time we wont....saving a lot of space and also avoiding a slow load process that
		// really isn't needed

	}


	BaseAnim = 2;   // 0 is static frame 1 is s stand, 2 and 3 are the walk frames
	Yspeed = Xspeed = 0;
	AnimIndex = 0;
	Dir = Right;
	TextureID = Images[0];

	ScaleX = 32.0;   // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;
}

// remember we now have a virtual base class destructor, so we have to provide a non default constructor
WaitAndChase::~WaitAndChase()
{
}

// A small helper function which returns the distance this object is from the Bob player which is always in position 0 of MyObjects
float WaitAndChase::GetDistance()
{
	float Xdist = (Game::MyObjects[0]->Xpos - Xpos);
	float Ydist = (Game::MyObjects[0]->Ypos - Ypos);
	float Dist = (Xdist*Xdist) + (Ydist*Ydist);
	return sqrt(Dist);

}


//Wait and chase is simply testing to see how far Bob is, if he's close he wakes up, if he gets closer he chases him
// Also notice the WaC object tests all the possible scuares it might bump into to see if there is an objstacle in the
// what that forces him to change direction. If Bob gets away he goes back to sleep.
// we're using some hard numbers such as 16 to indicate the size of a tile width, normally we would use a variable

bool WaitAndChase::Update(Game* G)
{
	float Speed = 1.2;
	Yspeed += 1.2f;

	// lets check if he's moving if not should we make him
		if(Moving == false)
	{
		BaseAnim =  AnimIndex = 0;
		Distance = GetDistance();
		if (Distance < 16 * 8) AnimIndex = 1;  // pop up and show interest
		if(Distance < 16 * 5)
		{
			Moving = true;
			Dir = (Game::MyObjects[0]->Xpos > Xpos) ? Right : Left;
		}
	}
	else
	{
		if (GetDistance() > 16 * 8) Moving = false;  // reset to wait if far away

		BaseAnim = 2;  // we are walking


		// we're moving so do the directional switches
	switch(Dir)
		{
		case Left:
			{
				Xpos -= Speed;
				// now we check for possible obsticles // we will scan from the top to the bottom of the sprite


				int YMap = (Ypos-16) / 16;
				int XMap = (Xpos - 16) / 16;

				for (int i = 0; i < 32 / 16; i++, YMap++) // 32 is the height of the character (it might however be variable)
				{
					int	WhatsAtTheEdge = G->WhichMap[(YMap * 64) + XMap];
					int Attrib = G->Attributes[WhatsAtTheEdge];
					if (Attrib & SOLID)
					{
						Dir = Right;
						break; // break the loop we are done
					}
				}
				break;
			}
		case Right:
			{
				{
					Xpos += Speed;
					// now we check for possible obsticles
					int YMap = (Ypos-16) / 16;
					int XMap = (Xpos + 1 + 16) / 16;

					for (int i = 0; i < 32 / 16; i++)
					{
						int	WhatsAtTheEdge = G->WhichMap[(YMap * 64) + XMap];
						;
						int Attrib = G->Attributes[WhatsAtTheEdge];

						if (Attrib & SOLID)
						{
							Dir = Left;
							break; // break the loop we are done
						}

						YMap++;  // move downp to the next
					}
					break;
				}
				break; // break the case
			}

		default:
			{
				printf("default occured, setting direction to Rightt \n");
				Dir = Right;
			}
		} // switch dir
	} // else
		// check for gravity and get the frame
	if(Yspeed > 9.81f / 4) Yspeed = 9.81f / 4;
	Ypos += Yspeed;

	int YMap = (Ypos + 17) / 16;
	int XMap = (Xpos + 8) / 16;

	// we could also choose to not let him fall!
	int	WhatsUnderOurFeet = G->WhichMap[(YMap * 64) + XMap];
	int Attrib = G->Attributes[WhatsUnderOurFeet];


// lets make him land a bit better
	if ((Attrib & SOLID) && Yspeed >= 0)
	{
		Ypos = YMap * 16 - 16;
		Yspeed = 0;
	}

	TextureID = Images[BaseAnim + AnimIndex];
	return true;
}


// just like Bob and Point to Point this is a standard simple draw system,
// we "could" move these all into the SimpleObj class and only call one, but it allows us
// the chanse to do slightly different things to the draw "if" we want to.

void WaitAndChase::Draw()
{

	/**************************************************************************************
	This is the exact same code we use to Draw Bob (with new matrix), but we may choose to
	draw our baddies in a different	way at some point so for now using the code this way
	opens up options to us later.
	***************************************************************************************/




	glBindTexture(GL_TEXTURE_2D, TextureID);       // we kept the texture in its own class this time
	glUseProgram(GameLoc->MyOGLES->ProgramObject3D);    // it was created in the OGL init, so lets keep using it

// 1st things 1st, lets create an MVP Model is already set up with an Identity I matrix, which is the matrix equivilent of 0
// our map never really moves in space, but our camera might so this needs to be updated each cycle

	glm::mat4 mRotationMatrix(1.0f);     // or rotate them
	glm::mat4 mScaleMatrix(1.0f);     // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR * 2, SCALEFACTOR * 2, SCALEFACTOR * 2);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	glm::mat4 mTranslationMatrix(1.0f);     // we are not going to move any of them
	mTranslationMatrix[3][0] = -32.5 + (Xpos / 16);
	mTranslationMatrix[3][1] = 20.5 + (-this->Ypos / 16);
	mTranslationMatrix[3][2] = 0.61f;

	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
	glm::mat4 MVPMatrix = *GameLoc->TheCamera.GetProjection() * *GameLoc->TheCamera.GetView() * Model;     // our camera conveniently gives us what we need

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
		2,		// there are 2 values UV
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

	if (glGetError() != GL_NO_ERROR) printf(" Wait and Chase draw errors\n");
	// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);


}
