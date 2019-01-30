#pragma once

#include "Game.h"
#include "Menu.h"

typedef enum GameStates
{
	GAME,
	MENU,
	SHUTDOWN
}GameStates;


// we've moved the main control from the Game Class which now exclusivley set up to play the levels
// control class allows the menu and game and shutdown to be seperate
class Control 
{
public:
	Control();
	~Control();
	
	void GameFlow();
	OGL	OGLES;
private:	

	GameStates TheState;
	Game*	TheGame;
	Menu*	TheMenu;

}
;
