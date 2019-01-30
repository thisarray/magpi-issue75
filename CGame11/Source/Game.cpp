/*
 Time to scroll our screen around, we need to make a few adjustments to the way we draw, but our basic concepts of interacting with the map
 do no change. We've added a scaling factor in Game.h which will allow us to change the size of tiles and sprites and allow us to work out
 where the sceen starts to draw when scrolling
 At the moment we are hard coding the size of the screen as full hdmi, 1920x1080, this isn't ideal but it lets you see the values in action.
 next time we will alter that to use the size supplied by the  hardware for the screen size, which could include tiny 320x200 matrix screens.
 Using varialbe values lets us adapt our code to different systems and different screen sizes.
 *
 **/


#include "Game.h"
#include "SimpleObj.h"
#include "Bob.h"
#include "PointToPoint.h"
#include "WaitAndChase.h"
#include "Menu.h"
#include "star.h"
#include "Tiles.h"

/*****************************************************************/


// These are called static variables they exist in the SimpleObj class, and mean that all Instances of a SimpleObj will share the values that are put in here
// the are defaulted to 0 or -1 but should be set to their default values as soon as an instance of a SimpleObj is uses.
// take care that if you change one of these values, for 1 object, you change it for all objects of the same type/base

	GLint SimpleObj::ProgramObject =0;
GLint SimpleObj::positionLoc =0;
GLint SimpleObj::texCoordLoc = 0;

// if we want to position it somewhere we need to ask for the handle for the position uniform and other unfirms
GLint SimpleObj::positionUniformLoc = 0;
GLint SimpleObj::samplerLoc = 0;
GLint SimpleObj::ScreenCoordLoc = 0;
GLint SimpleObj::ScaleLoc = 0;
GLuint SimpleObj::VBO = -1;
GLuint SimpleObj::OffsetsLoc = 0;
Game* SimpleObj::GameLoc = nullptr;
GLuint SimpleObj::MVPLoc = 0;
OGL* SimpleObj::TheOGLInstance = nullptr;


// we are going to reflect the size of our textures, so we need a square that has size 1x1
// since the distance from the centre to the corners is 0.5x and 0.5y, two tringles will use offsets of 0.5f
static const GLfloat QuadVerticesWithTex[] =
{
	// square vertices with texture coordinates added, so we have 3 floats of vertex,
   // then 2 floats of texture coords (called uv coords), this is called interlacing data
	0.5f,0.5f,0,
	1.0f,0.0f,	// texture coordinate
	-0.5f,0.5f,0,
	0.0f,0.0f,	// texture coordinate
	0.5f,-0.5f,  0,
	1.0f,1.0f, 	// texture coordinate

	//2nd tri
	-0.5f,0.5f,0,
	0.0f,0.0f,	// texture coordinate
	-0.5f,-0.5f,0,
	0.0f,1.0f, 	// texture coordinate
	0.5f,-0.5f,0,
	1.0f,1.0f // texture coordinate
};


/**********************************************************************************************
 We should consider moving these values above out of Game class too ^^^^ where's a good place?
***********************************************************************************************/

//now into the meat of our Game class.


std::vector<SimpleObj*> Game::MyObjects; // because we defined this as static variables in our header we have to make sure it exists
Game::Game()
{
}


// biggest change is a slimming down of the game constructor, its is no longer responsible for the text and OGL init
Game::Game(OGL* theOGLES)
{
	MyOGLES = theOGLES;
	MyText = new Text();
	MyText->WhereisOGL = theOGLES; // text does need to know where the ogl is

// make our Bob	here, this ensures there is at least one instance of a simpleObj which makes our init of static variables safer (they can be destoyed by a later instance generation)
// also because Bob is so importont to us we name him and give him a pointer in the class defintion now

	MyBob = new Bob();
	;   // Bob's render system needs to know where the OGELS class is, since it has info on the basic shader
	MyObjects.push_back(MyBob);   // put our Bob characer into a processing list of characters

// put our vertices into our VBO
	glGenBuffers(1, &SimpleObj::VBO);  // first we generate the buffer and store the handle in VBO
	glBindBuffer(GL_ARRAY_BUFFER, SimpleObj::VBO);   //now we mind that, which we can leave as bound since we use the same VBO
	glBufferData(GL_ARRAY_BUFFER, // now we send the vertex data to the GPU one time
		6*5 * sizeof(float),
		QuadVerticesWithTex,
		GL_STATIC_DRAW);


	// All our sprites use the same shader and quad box to draw, so lets set them up, so they can get the values needed when they are asked to draw
	// there is a change from past systems though we are now using a matrix driven system and a 3D program object
	glUseProgram(MyOGLES->ProgramObject3D);


	//	 Get the attribute locations, we'll store these in values we can call handles.

// the simpleObj's now have a list of statics we can set up
	SimpleObj::TheOGLInstance = theOGLES;   // let the SimpleOBJ's know where the OGL instance, it does get used in a character constructors so needs an early set up
	SimpleObj::positionLoc = glGetAttribLocation(MyOGLES->programObject, "a_position");
	SimpleObj::texCoordLoc = glGetAttribLocation(MyOGLES->programObject, "a_texCoord");

// set up the uniform locations too
	SimpleObj::positionUniformLoc = glGetUniformLocation(MyOGLES -> programObject, "u_position");
	SimpleObj::samplerLoc = glGetUniformLocation(MyOGLES->programObject, "s_texture");
	SimpleObj::ScreenCoordLoc = glGetUniformLocation(MyOGLES->programObject, "u_Screensize");
	SimpleObj::ScaleLoc = glGetUniformLocation(MyOGLES->programObject, "u_Scale");
	SimpleObj::OffsetsLoc = glGetUniformLocation(MyOGLES->programObject, "u_Offsets");
	SimpleObj::MVPLoc = 	glGetUniformLocation(MyOGLES->ProgramObject3D, "MVP");
	SimpleObj::GameLoc = this; //they do need to access some info from this game class
// So the Game ckass constructor, basically sets up the OGL, and provides the sprites with the values its shader needs.

	Menu TheMenu;



};

Game::~Game() {}; // We should close down some things here but can do it later.

// we've got to set
void Game::SetupLevel()
{
	// we've now split the setup away from the update (which makes more sense)
	// we create bob in our constructor, and in the process set up some important SimpleObj static variables,
// now lets put a couple of differnt baddies on screen, first up,  these are point to point types


//we will create a few enemies, these are the same locations for all maps and don't always work, maybe you can add to this?


	PointToPoint* P2P1 = new PointToPoint();
	P2P1->TheOGLInstance = MyOGLES;
	P2P1->Xpos = 280;
	P2P1->Ypos = 280;
	P2P1->SetXPoints(280, 50, Right);  // << what do you think will happen if we make it move further? Try a count of 150 pixels, and try to predict, what goes "wrong", can you think of a way to fix things?
	MyObjects.push_back(P2P1);

	PointToPoint* P2P2 = new PointToPoint();
	P2P2->TheOGLInstance = MyOGLES;
	P2P2->Xpos = 280;
	P2P2->Ypos = 80;
	P2P2->SetXPoints(280, 350, Right);
	MyObjects.push_back(P2P2);

	// and these are wait and chase
	WaitAndChase* WaC1 = new WaitAndChase();
	WaC1->TheOGLInstance = MyOGLES;
	WaC1->Xpos = 380;
	WaC1->Ypos = 80;
	MyObjects.push_back(WaC1);

	WaitAndChase* WaC2 = new WaitAndChase();
	WaC2->TheOGLInstance = MyOGLES;
	WaC2->Xpos = 480;
	WaC2->Ypos = 280;
	MyObjects.push_back(WaC2);

// lets set up the stars for each level

	for (int i = 0; i < 20; i++)
	{

		Star* star = new Star();
		star->TheOGLInstance = MyOGLES;
		switch (Level)
		{
		case 0:
			star->Xpos = this->stars1[i][0];
			star->Ypos = this->stars1[i][1];
			break;

		case 1:
			star->Xpos = this->stars2[i][0];
			star->Ypos = this->stars2[i][1];
			break;

		case 2:
			star->Xpos = this->stars3[i][0];
			star->Ypos = this->stars3[i][1];
			break;

		case 3:
			star->Xpos = this->stars4[i][0];
			star->Ypos = this->stars4[i][1];
			break;
		default:
			printf("Code has come to this switch default, meaning we have not written a case for it");
			break;

		}
		MyObjects.push_back(star);
	}
// we also need to initialise lists of our enemes


	// all that messy init code for FastUpdate is now contained in the Tiles Class, so lets make an instance of Tiles
	// and initialise them ready to be rendered
	MyTiles = new Tiles();
	MyTiles->InitTiles(this);  //<<<<see how much cleaner this is? At least in this file!

// ok all done, we should have our map from the meny class called in in contol, and our game has a level value so it can access the star and enemy lists to set things up

}

/**********************************************************************************

 Here's the main code..since the game starts here, we can do a bit of set up and init
 Then go into a loop to control the game

 ***********************************************************************************/

void Game::Update()
{

	struct timeval t1, t2;
	struct timezone tz;
	float deltatime;
	float totaltime = 0.0f;
	unsigned int frames = 0;
	gettimeofday(&t1, &tz);


/**************************************************************************/
	// Here's where the main loop occurs
    // we're now doing a while test, to make the loop run as long as the stars remain to be collected or the timer is no yet 0

		int TimerCountDown = 100; // we will keep a small count down when we have collected all stars

// we can test more than one thing in a condition using && and || which represent AND and OR
	while(HowManyStars !=0 || TimerCountDown !=0) // instead of a simpler counter, we now wait for all stars to be picked up, as well as a small timer to be 0
	{
		if (HowManyStars == 0) TimerCountDown--; // this make sure we only count down when all stars are picked up

	// where is the camera going to look?
	// This won't produce a 1 to 1 match for the motion but that helps to give a sense of depth to the image as we will see edges later
	// It's actually a littly clumbsy, as it ties the scroll directly to the motion of Bob which can be a bit uncomfortable on the eye.
		this->TheCamera.Target.x = (MyObjects[0]->Xpos - 30 * 16) / 16;
		this->TheCamera.Target.y = -(MyObjects[0]->Ypos - 20 * 16) / 16;
		this->TheCamera.Target.z  = 0;



		this->TheCamera.Target.x = (MyObjects[0]->Xpos - 30 * 16) / 16;
		this->TheCamera.Target.y = -(MyObjects[0]->Ypos - 20 * 16) / 16;
		this->TheCamera.Target.z  = 0;

		// we also want to allow some small movement on the camera

		if(MyBob->IH->TestKey(KEY_Q) && TheCamera.CameraOffset < 12.0f)
		{
			TheCamera.CameraOffset += 0.1f;
		}

		if (MyBob->IH->TestKey(KEY_A) && TheCamera.CameraOffset > -12.0f)
		{
			TheCamera.CameraOffset -= 0.1f;
		}


		this->TheCamera.Update();

		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 0.0000001f);
		t1 = t2;

		glClear(GL_COLOR_BUFFER_BIT); // clear the screen, though since you are covering the screen with tiles, you might not need to!


// lets move Bob
			if(MyObjects[0]->Update(this) == false) break;  // this is bob, he's alaways 1st he also lets us test for break
//now' he's moved we need to work out where the screen needs to be we will keep ScX and ScY, the screens coordinates in the map, in the game.h
// so other objects can use it


	 ScX = (MyObjects[0]->Xpos - ((1920 / 2)/SCALEFACTOR) );
	 ScY = (MyObjects[0]->Ypos - ((1080 / 2)/SCALEFACTOR) );
// lets make sure we don't scroll below 0
		if (ScX < 0) 	ScX = 0;
		if (ScY < 0)	ScY = 0;
//also lets make sure we don't scroll past the map (which in this hard case is 64x40, normally we would not use such hard numbers so
// we could have variable size maps, this will be discussed soon
		float RightEdge = (64 * 16) - (1920  / SCALEFACTOR);
		float BottomEdge = (40 * 16) - (1080  / SCALEFACTOR);

// now check for the edges and correct if needed
		if (ScX > RightEdge) ScX = RightEdge;
		if (ScY > BottomEdge) ScY = BottomEdge;




// So now our screen's position is calculated
		MyTiles->ScreensXcoord = ScX*SCALEFACTOR;
		MyTiles->ScreensYcoord = ScY*SCALEFACTOR;

// Important to note we calculated the screens position based on Bob's current position, but we added the possibility of
// correcting the scroll if it was going to go below 0 or past the right edge, so we will recalculate Bob's screen positon (not his xpos, that is map based)
// to make sure he stays at the correct screen position even if the scroll has stopped.
// lets calculate the screen position we want to display him at
		MyObjects[0]->ScreenX = (MyObjects[0]->Xpos - ScX);
		MyObjects[0]->ScreenY = (MyObjects[0]->Ypos - ScY);
// draw the tiles 1st
		MyTiles->Render(this);

		MyObjects[0]->Draw(); // make sure we draw bob

// we can go through a vector the same way we can an array to update the objects we put in the vector
		for (unsigned int i = 1; i < MyObjects.size(); i++)
		{
			bool IsItAlive =	MyObjects[i]->Update(this);
			MyObjects[i]->ScreenX = MyObjects[i]->Xpos - ScX; // this time before we draw them work out their screen positon
			MyObjects[i]->ScreenY = MyObjects[i]->Ypos - ScY; // which is based on where our screen starts

			MyObjects[i]->Draw();
			if (IsItAlive == false) //if we killed something we should remove it from the list
			{
				delete MyObjects[i]; // it was new'd into existance so we must delete it, but this used to correctly generate a warning as we were unsure what polymorph we were deleting
										// Now, even though the warning is annoying it does not stop our project running, and we let it slide last time, but we really should deal with it.
										// the correct solution we've added here is to make the base class (SimpleObj) destructor virtual, and ensure all derived classed include a
										// destructor. (we added one ot WaitAndChase).
										// Now when you build this, you won't get the warning.. This is important to let you realise sometimes the cause of the error/warning is not
										// at the location it is reported!

				MyObjects.erase(MyObjects.begin() + i); // then remove it from the vector
			}

		}
	// sprintf is a special kind of formatting output that prints into a buffer.
	// this can be useful for outputting variable data, and specific variables for debugging while in full screen mode
	// lets use it in 2 different ways
		char TempBuffer[130];
		if (HowManyStars)
			std::sprintf(TempBuffer, "We have %i stars to collect and can display %i for debug", HowManyStars, this->MyBob->BaseAnim);
		else
			 if (TimerCountDown == 0)
			{
			std::sprintf(TempBuffer, "Press any Key to Return to Menu");
			}
				else	std::sprintf(TempBuffer, "Resetting Level in %i", TimerCountDown);

		MyText->RenderText(TempBuffer, 10, 32, 1.8f);


// put our image on screen
		eglSwapBuffers(MyOGLES->state.display, MyOGLES->state.surface);

	//	let our user know how long that all took
		totaltime += deltatime;
		frames++;
		if (totaltime >  1.0f)
		{ // printf is a nice function that allows us to format data, %4d means format to 4 digits integer values
		 // %1.4f means 1 integer and 4 decimal points for a float value
		 // %3.4f means......what do you think?

			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);
			totaltime -= 1.0f;
			frames = 0;
		}

	}

	// clean the memory out before we escape, we new'ed a lot of things, lets delete them, now in fairness, the OS will clear all this up for you when you exit the program
	// but its a very good habit to get into, and can sometimes help you to locate memory leaks you didn't know you had. This loop though is very simple and just removes
	// all those simple player and enemy objects we created, MyTiles was not new'ed so its local and will be cleared on exit.

		for(unsigned int i = 0 ; i < MyObjects.size() ; i++)
			delete MyObjects[i];  // delete the objects
		MyObjects.erase(MyObjects.begin(), MyObjects.end());  // clear the vector


	return; // we don't really need the return but it helps us to see the end of the method was reached and we can breakpoint it

};

