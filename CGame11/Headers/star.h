#pragma once

// Star is a simple pickup, it does nothing except rotate and look cute, and test if its been picked up by Bob
// it ignores collision with any other object
#include "SimpleObj.h"
#include "Game.h"
class Star : public SimpleObj
{
public:
	Star();
	~Star();
	
	
	// one noticiable difference is I am passing a pointer to Game, because Game has useful data that my update might need	
	// I could, (and should) only pass the data I want, but its just easier like this for now.	
	bool Update(Game* G);
	
	int	BaseAnim;
	int	AnimIndex;
	void Draw();


private :	
	GLuint Image; // there's only one image for this

	

};