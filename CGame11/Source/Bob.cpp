#include "Bob.h"

#include <chrono>
#include <thread>
/* Lets tidy things up a bit
 * We have to accomodate a few things as a result of the growth of the game.
 * We can now draw Bob with a matrix, and indeed we now see that all the objects are drawn the same way. So we should stop repeating the draw code!
 *			so we will move most of the draw code back to the SimpleObj class and just focus on any small changes we might need in our Bob->draw class
 * We should stop him walking into walls. We'll do that by testing his feet and head
 * He can interact with more than 1 map, so the hard coded Map1 Map2 Map3 etc need to be replaced with a more flexible system
 * We should stop him pinging back to the look right frame as a default
 *
 */
//make him a bit faster from 1.0 to 1.2
#define SPEED 1.2f

Bob::Bob()
{
	// we can create with these images but lets store the full set

	char* ImageNames[] = {
		(char*)"Assets/brianWalkR_1.png",
		(char*)"Assets/brianWalkR_2.png",
		(char*)"Assets/brianWalkR_3.png",
		(char*)"Assets/brianWalkR_4.png",
		(char*)"Assets/brianWalkL_1.png",
		(char*)"Assets/brianWalkL_2.png",
		(char*)"Assets/brianWalkL_3.png",
		(char*)"Assets/brianWalkL_4.png",
		(char*)"Assets/brianclimb_1.png",
		(char*)"Assets/brianclimb_2.png",
		(char*)"Assets/brianclimb_3.png",
		(char*)"Assets/brianclimb_4.png",
	};

		// lets turn these all these graphic files into textures, but consider, is it good to have them all as seperate textures?
	for(int i = 0 ; i < 12 ; i++)
	{
		int Grwidth, Grheight, comp;
		unsigned char* data = stbi_load(ImageNames[i], &Grwidth, &Grheight, &comp, 4);      // ask it to load 4 componants since its rgba
		//now its loaded we have to create a texture, which will have a "handle" ID that can be stored, we have added a nice function to make this easy
		// be careful to make sure you load the graphics correctly?

		Images[i] = SimpleObj::TheOGLInstance->CreateTexture2D(Grwidth, Grheight, (char*) data);      //just pass the width and height of the graphic, and where it is located and we can make a texture
		free(data);  // when we load an image, it goes into memory, making it a texture copies it to GPU memory, but we need to remove the CPU memory before to load another or it causes a leak
	}

	BaseAnim = AnimIndex = 0;
	TextureID = Images[0];
	// bob needs an input handler, we defined one in the Bob.h file but its currently an empty pointer
	IH = new Input();
	IH->Init();

	Xpos = 120;
	Ypos = 160;
	ScaleX = 32.0;  // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;

	Climbing = false;
}

Bob::~Bob()
{
	// Bob created the key handler so he needs to be the one to remove it, 1st close it down
	IH->kQuit = true; // lets stop the keyboard thread
	IH->mQuit = true;
	// the thread won't actually stop until a final key is read, so our system asked us to press a key
		while(Input::KeyStopped == false)
		{
		printf("killing some time\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	delete IH;  // and delete this before we exit
}


// By returning bool as false we indicate to the game the character is dead so make sure bob always returns true
bool Bob::Update(Game* G)
{
	// remember our Ypos needs to be flipped to be able to look through an array since our screen starts at 0,0 in the bottom left, and the array
	// has 0,0 at the top left, since we need it a few times, we created a private variable in the bob class to do this
//	BaseAnim = 0;

//	TextureID = Images[0];



	glm::mat4 mTranslationMatrix(1.0f);   // we are not going to move any of them
	glm::mat4 mRotationMatrix(1.0f);   // or rotate them
	glm::mat4 mScaleMatrix(1.0f);   // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR*2, SCALEFACTOR*2, SCALEFACTOR*2);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);

	glUseProgram(G->MyOGLES->ProgramObject3D);

	// these values never change so lets set them up
	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;


	if (Climbing == false)
	{
		Yspeed += SPEED;
		if (Yspeed > 9.81f) Yspeed = 9.81f;
		Ypos += Yspeed;
	}
	else
	{
		Ypos += Yspeed;
		Yspeed = 0;
	}

	int YMap = (Ypos + 16) / 16;
	int XMap = (Xpos) / 16;

	// there's a slight change here, becuase we are using a varible to pass the 2D array, we can't use this [y][x] system any more,
			//int	WhatsUnderOurFeet = G->Map2[YMap][XMap];
			// its now replaced with a slightly more confusing version here.
			int WhatsUnderOurFeet =  G->WhichMap[(YMap * 64) + XMap];   // the reasons this works is that a 2D array is laid out in consecutive rows, so as long as we know the length of the columns we can index into it
			// there's still a hard number here, 64 which ideally we'd like to replace with a variable, but all our maps are 64x40 so....its kinda ok for this.
			//WhichMap can be set up then to point to any map which is 64 wide, and almost any size long.. we'll make this more flexible later when we have odd sized maps I'll repeat this once more below then refer you to this


	int Attrib = G->Attributes[WhatsUnderOurFeet];

	if ((Attrib & SOLID) && Yspeed >= 0 && Climbing == false)
	{
		Ypos = YMap * 16 - 16;
		Yspeed = 0;
	}

		if (IH->TestKey(KEY_RIGHT))
		{
// so we can be sure of gravity byut we also should check what we are planning to walk into
// this works the same way as gravity but to the left, we should test at leat 2 points though, our feet and our head

			Climbing = false;
			BaseAnim = 0;
			TextureID = Images[BaseAnim + AnimIndex];
			(AnimIndex += 1) &= 3;

// we can let the animation continue but we may not allow the movment
			YMap = (Ypos + 8) / 16;
			XMap = (Xpos + 8) / 16;
			int WhatsAtOurFeet =  G->WhichMap[(YMap * 64) + XMap];
			Attrib = G->Attributes[WhatsAtOurFeet];

			if (!(Attrib & SOLID) ) // both need to be not solid to move
					this->Xpos += SPEED*2;
		}
// might have been damaged reseet to gravity defaults
	YMap = (Ypos + 16) / 16;
	XMap = (Xpos) / 16;


		if (IH->TestKey(KEY_LEFT))
		{

			Climbing = false;
			BaseAnim = 4;
			TextureID = Images[BaseAnim + AnimIndex];
			(AnimIndex += 1) &= 3;

			// we can let the animation continue but we may not allow the movment
			YMap = (Ypos + 8) / 16;
			XMap = (Xpos - 8) / 16;
			int WhatsAtOurFeet =  G->WhichMap[(YMap * 64) + XMap];

			Attrib = G->Attributes[WhatsAtOurFeet];

			if (!(Attrib & SOLID) )
				this->Xpos -= SPEED*2;
		}
// reload attrib
	Attrib = G->Attributes[WhatsUnderOurFeet];

	if (IH->TestKey(KEY_UP))
	{
			// this is the climb
		if(Climbing)
		{
			Yspeed = -SPEED ;
			BaseAnim = 8;
			TextureID = Images[BaseAnim + AnimIndex];
			(AnimIndex += 1) &= 3;
			// we need to test if the climb is over?
			if((Attrib & LADDER) == 0)
					{
						Climbing = false;
					}

		}
		else
		{
			 // now we check if we are on a ladder
			if(TestClimb(G) == true)
			{
				Climbing = true;
			}
			if ((Attrib & SOLID)   && (Yspeed >= 0))
				Yspeed = -SPEED * 8;
		}
	}


	if (IH->TestKey(KEY_DOWN))
	{

		if (TestClimbDown(G))	Climbing = true;


		// check if we are climbing and if so, make sure we use the climbing animations
		// but also if we are currently climbing, make sure we are still on a ladder, and if, not stop climbing and revert to normal walk mode.
			if(Climbing == true)
		{
			// Note again, there's a slight change here, becuase we are using a varible to pass the 2D array, we can't use this [y][x] system any more,
			//int	WhatsUnderOurFeetplus = G->Map2[YMap][XMap];
			// its now replaced with a slightly more confusing version here.
			int WhatsUnderOurFeetplus =  G->WhichMap[(YMap * 64) + XMap];  // the reasons this works is that a 2D array is laid out in consecutive rows, so as long as we know the length of the columns we can index into it
			// there's still a hard number here, 64 which ideally we'd like to replace with a variable, but all our maps are 64x40 so....its kinda ok for this.
			//WhichMap can be set up then to point to any map which is 64 wide, and almost any size long.. we'll make this more flexible later when we have odd sized maps


			int Attrib2 = G->Attributes[WhatsUnderOurFeetplus];

			BaseAnim = 8;
			TextureID= Images[BaseAnim + AnimIndex];
			(AnimIndex -= 1) &= 3;
			this->Ypos += SPEED;
			if ((Attrib2 & LADDER) && (Attrib & LADDER))
			{
				Climbing = false;
				Yspeed = SPEED;
			}
		}
		else
		{
			// so this is I am not climbing, but I am still in the I pressed down condition
		 // maybe we can add a toe test graphic?
		}

	}

	// lets add a break condition now
	if(IH->TestKey(KEY_ESC)) return false;  // consider it over


	return true;
}



bool Bob::TestClimb(Game* G)
{
	int YMap = (Ypos + 16) / 16;
	int XMap = (Xpos - 2) / 16;

	int	WhatsUnderOurFeet =  G->WhichMap[(YMap * 64) + XMap]; // notice the change detailed above
	int Attrib = G->Attributes[WhatsUnderOurFeet];

	if ((Attrib & LADDER) == 0) return false; // test for ladder

	YMap = (Ypos + 16) / 16;
	XMap = (Xpos + 2) / 16;

	WhatsUnderOurFeet =    G->WhichMap[(YMap * 64) + XMap];  // notice the change detailed above
	Attrib = G->Attributes[WhatsUnderOurFeet];
	return (Attrib & LADDER) == LADDER;
}


bool Bob::TestClimbDown(Game* G)
{
	int YMap = (Ypos + 32 ) / 16;
	int XMap = (Xpos - 2) / 16;

// don't allow a test outside the map area
	if (YMap > 39) return false;

	int	WhatsUnderOurFeet =  G->WhichMap[(YMap * 64) + XMap];  // notice the change detailed above
	int Attrib = G->Attributes[WhatsUnderOurFeet];

	if ((Attrib & LADDER) == 0) return false; // test for ladder

	YMap = (Ypos + 32 ) / 16;
	XMap = (Xpos + 2) / 16;

	WhatsUnderOurFeet =  G->WhichMap[(YMap * 64) + XMap];  // notice the change detailed above
	Attrib = G->Attributes[WhatsUnderOurFeet];
	return (Attrib & LADDER) == LADDER;
}

typedef enum {
	STANDING = 0,
	WALKING,
	JUMPING,
	CLIMBING,
	FALLING,
	DROWNING
} StateVaues;


void Bob::BobsLogic()
{

	switch (CurrentState) // the Switch asks what is our current state represented as an enumerated value or int
	{
	case	STANDING:
		//Do code for a standing character
		break; // this ends the code
	case	WALKING:
		break; // notice we must end each section with break...if we don't it falls through
	case	JUMPING:
		//Do code for a jumping character
		break;  // this ends the code
	case	CLIMBING:
		//Do code for a climbing character
		break;  // this ends the code
	case	FALLING :
		//Do code for a falling character
		break;  // this ends the code

	case	DROWNING :
		//Do code for a drowingcharacter
		break;  // this ends the code

	default : // this is a special case in case we have not found code for a particular state value
		//do default code, which usually means we have forgotton something
		break;
	} // close the switch


}

// Bob and other sprites are the only ones now who are really doing thier own draw, for now its a copy
// of the older tile system, but as we optimise the code we are probably going to find this is too slow for our needs.
// But don't be in a rush to make changes, we have more important update and logic code to get working before we start to worry about
// the efficiency of this code.
void Bob::Draw()
{

		glBindTexture(GL_TEXTURE_2D, TextureID);      // we kept the texture in its own class this time
		glUseProgram(GameLoc->MyOGLES->ProgramObject3D);   // it was created in the OGL init, so lets keep using it

// 1st things 1st, lets create an MVP Model is already set up with an Identity I matrix, which is the matrix equivilent of 0
// our map never really moves in space, but our camera might so this needs to be updated each cycle

	glm::mat4 mRotationMatrix(1.0f);    // or rotate them
	glm::mat4 mScaleMatrix(1.0f);    // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR * 2, SCALEFACTOR * 2, SCALEFACTOR * 2);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	glm::mat4 mTranslationMatrix(1.0f);    // we are not going to move any of them
	mTranslationMatrix[3][0] = -32.5+(Xpos / 16);
	mTranslationMatrix[3][1] = 20.5+(-this->Ypos/16);
	mTranslationMatrix[3][2] = 0.58f; // we do need a z coord but lets just put it a little ahead of the tiles

	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
	glm::mat4 MVPMatrix = *GameLoc->TheCamera.GetProjection() * *GameLoc->TheCamera.GetView() * Model;    // our camera conveniently gives us what we need

// did you notice that despite being called an MVP we multipled in the order PVM, this is because compilers can't tell the mathamatical precident order when things are all the same type
// so it does them in the order they are on the stack, when we do this calc,the P then the V  then the M are put on the stack, so the 1st thing it sees then is the M, then the V, then P
// remember that though we are abstracting this to P*V*M to make it look like a simple calculation, there are in fact a massive number of calcuations going on, so we don't want to do this unless we have to

	// let the shaders know what VBO and texture to use
// load the constant uniform values to the shader, in this case the MVP
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVPMatrix[0][0]);

	// now turn the screen's start x y coords in to a shader usable value of offsets
	XYOffsets.xValue = 0;
	XYOffsets.yValue = 0;



// this code is all explained in the SimpleObj version

			glUniform1i(samplerLoc, 0);

			glBindBuffer(GL_ARRAY_BUFFER, SimpleObj::VBO);     //now we mind that, which we can leave as bound since we use the same VBO
			glBindTexture(GL_TEXTURE_2D, TextureID);     // we kept the texture in its own class this time

	GLuint stride = 5*sizeof(float);   // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
		3,		// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride,	  // how many floats to the next one
		(const void*)0  // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);

	glVertexAttribPointer(texCoordLoc,
		2,		// there are 2 values xyz
		GL_FLOAT, 	 // they a float
		GL_FALSE,	 // don't need to be normalised
		stride,		  // how many floats to the next one
		(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

	// but we will still ask it to use the same position and texture attributes locations
			glEnableVertexAttribArray(positionLoc);       // when we enable it, the shader can now use it and it starts at its base value
			glEnableVertexAttribArray(texCoordLoc);       // when we enable it, the shader can now use it
			// now its been set up, tell it to draw 6 vertices which make up a square
			glDrawArrays(GL_TRIANGLES, 0, 6);

	if (glGetError() != GL_NO_ERROR) printf(" Dob draw errors\n");
	// its wise to disable when done
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(texCoordLoc);

}


