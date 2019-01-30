#pragma once
//this is a very simple Menu class, just to give our project an entry and exit point
#include "OGL.h" 
#include "Text.h" //
#include "Input.h" // though not used here, clearly we will use them in the cpp file, so keep them in here


class Game;
class Menu
{
public:
	Menu();
	Menu(OGL* TheOGL);
	~Menu();
	Text* MyText;
	
	void DisplayOptions();
	int* WaitForInput(Game*, int &Level); // notice we don't actually need to provide a variable name in the method defenition, it just needs 
							    // to know what kind of thing we are passing, but its good practice to use an explanitory variable
	
private:	
	
};
