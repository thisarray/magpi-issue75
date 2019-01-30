#include "star.h"
#include "Bob.h"
#include "Game.h"

Star::Star()
{
	TextureID = TheOGLInstance->TheAssetManager.LoadandCreateTexture((char*)"Assets/star.png");
	ScaleX = 32.0;   // this tells us how many pixels to make our texture square, sprite are 32x32
	ScaleY = 32.0;

}


Star::~Star()
{

}

// stars don't care about gravity or enemty objects they just look cute and wait to be picked up
bool Star::Update(Game* G)
{
	// A stars only purpose in life is to test if Bob has come close enogh to pick it up
	// we can use the simple box check we have in our base class to test

	if(this->DidIGetHit(G->MyBob))
	{
		G->HowManyStars--;
		return false; // this  will result in its removal

	}
	return true; // don't need to do much
}


void Star::Draw()
{

// Bob and other sprites are the only ones now who are really doing thier own draw, for now its a copy
// of the older tile system, but as we optimise the code we are probably going to find this is too slow for our needs.
// But don't be in a rush to make changes, we have more important update and logic code to get working before we start to worry about
// the efficiency of this code.
		glBindTexture(GL_TEXTURE_2D, TextureID);       // we kept the texture in its own class this time
		glUseProgram(GameLoc->MyOGLES->ProgramObject3D);    // it was created in the OGL init, so lets keep using it

// 1st things 1st, lets create an MVP Model is already set up with an Identity I matrix, which is the matrix equivilent of 0
// our map never really moves in space, but our camera might so this needs to be updated each cycle

	glm::mat4 mRotationMatrix(1.0f);     // or rotate them
	glm::mat4 mScaleMatrix(1.0f);     // or scale them

	glm::vec3 Scales = glm::vec3(SCALEFACTOR, SCALEFACTOR , SCALEFACTOR );
		mScaleMatrix = glm::scale(mScaleMatrix, Scales);

// here's a small exercise for you... we have a static value below called radian, its the radian angle of our rotation	(computers like radians rather than degrees)
// making it static means that every instance of a star will use the same value and as such as stars are picked up or added the
// speed of rotation will change, this may or may not be desirable, thats up to you to decide
// Try to change it so the stars rotate at the same speed, there are a couple of ways to do that, some better than others. (answers at the bottom of the file)

	static float radian = 0; // create a static single value for the angle (in radians.. 0>2PI)

	mRotationMatrix = glm::rotate(mRotationMatrix, radian, glm::vec3(0, 0, 1.0f));// the final vector dictates what axis rotates, a non 0 value in x y or z causes rotation
	radian += 0.1/20.0f; // increment the value by a reasonable amount (it does not matter if it goes over 2Pi it will modulus the value)

		glm::mat4 mTranslationMatrix(1.0f);     // set to identity
		mTranslationMatrix[3][0] = -32.5 + (Xpos / 16);
		mTranslationMatrix[3][1] = 20.5 + (-this->Ypos / 16);
		mTranslationMatrix[3][2] = 0.776f;  // this is a z pos just to bring bob a little bit forwardso he does not clash with tiles
// now make the master model matrix
		Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
// and the MVP we plan to send to the shader
		glm::mat4 MVPMatrix = *GameLoc->TheCamera.GetProjection() * *GameLoc->TheCamera.GetView() * Model;     // our camera conveniently gives us what we need

		// did you notice that despite being called an MVP we multipled in the order PVM, this is because compilers can't tell the mathamatical precident order when things are all the same type
		// so it does them in the order they are on the stack, when we do this calc,the P then the V  then the M are put on the stack, so the 1st thing it sees then is the M, then the V, then P
		// remember that though we are abstracting this to P*V*M to make it look like a simple calculation, there are in fact a massive number of calcuations going on, so we don't want to do this unless we have to

			// let the shaders know what VBO and texture to use
			// load the constant uniform values to the shader, in this case the MVP
				glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVPMatrix[0][0]);

		// now turn the screen's start x y coords in to a shader usable value of offsets
//	XYOffsets.xValue = 0;
	//	XYOffsets.yValue = 0;



		// this code is all explained in the SimpleObj version

			glUniform1i(samplerLoc, 0);

		glBindBuffer(GL_ARRAY_BUFFER, SimpleObj::VBO);      //now we mind that, which we can leave as bound since we use the same VBO
		glBindTexture(GL_TEXTURE_2D, TextureID);      // we kept the texture in its own class this time

	GLuint stride = 5*sizeof(float);    // 3 position floats, 2 screen location floats, and 2 uv

	// now tell the attributes where to find the vertices, positions and uv data
	glVertexAttribPointer(positionLoc,
			3,			// there are 3 values xyz
			GL_FLOAT,	 // they a float
			GL_FALSE,	 // don't need to be normalised
			stride,		  // how many floats to the next one
			(const void*)0  // where do they start as an index); // use 3 values, but add stride each time to get to the next
		);

		glVertexAttribPointer(texCoordLoc,
			2,		// there are 2 values xyz
			GL_FLOAT,	 // they a float
			GL_FALSE,	 // don't need to be normalised
			stride,		  // how many floats to the next one
			(const void*)(sizeof(float) * 3)    // where do they start as an index
		);

		// but we will still ask it to use the same position and texture attributes locations
				glEnableVertexAttribArray(positionLoc);        // when we enable it, the shader can now use it and it starts at its base value
				glEnableVertexAttribArray(texCoordLoc);        // when we enable it, the shader can now use it
				// now its been set up, tell it to draw 6 vertices which make up a square
				glDrawArrays(GL_TRIANGLES, 0, 6);

		if (glGetError() != GL_NO_ERROR) printf(" star draw errors\n");
		// its wise to disable when done
		glDisableVertexAttribArray(positionLoc);
		glDisableVertexAttribArray(texCoordLoc);





}

/*Static excercise
 *
 * If you want each individual star to have its own copy of radian, then you should include it as a member(variable) in the class definition
 * that also allows you to set different start values for each star
 * If really feel you need the static float, but consistant speed, you could also keep track of the number of stars, by keeping a static counter of how many stars there are,
 * and divide the increase in angle by the current number of stars
 * If you really want them all to use the same radian value, it might be better to keep the variable in the game class that calls the stars
 * Even though it seems a simple thing, there are man ways to handle this, but try to focus on the simplest. (add a member is for me the easiest)
 */
