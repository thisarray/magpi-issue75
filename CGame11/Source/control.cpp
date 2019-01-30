#include "control.h"
#include "Input.h"
#include "Bob.h"
#include "OGL.h"


Control::Control()
{

	OGLES.Init();     // make sure we initialise the OGL instance we called OGLES

	this->TheGame = new Game(&OGLES);
	this->TheMenu = new Menu(&OGLES);
	this->TheState = MENU;
}


Control::~Control()
{

}

void Control::GameFlow()
{
	bool	ExitRequested	= false; // if we want the game to end we need to change this to true
	
	while (ExitRequested == false)
	{

		switch (TheState)
		{
		case	GAME:
			TheGame->SetupLevel();
			TheGame->Update(); // this goes off to play the game
			
// we should now reset things, the simplest thing to do is just delete the instances of game and menu
// as long as they do their own clearing up, all should be fine, but...check..do we clear up correctly? If not we will get a memory leak
// one minor annoyance is that the Bob class creates an instance of input, causing a thread to run, that must close down, before it is deleted, though it needs a key to be pressed to activate it
// in this main control class we could pass to the game control classes, like OGL.. for now we've kept it simple (but inefficient) and let Bob kill the thread and restart a new
// the bob destructor takes care of that, but really starting and restarting threads is unwise, and in a better constructed project we would have a master in this main control class 
// we could pass to the game control classes, like OGL.. for now we've kept it simple (but inefficient) and let Bob kill the thread and restart a new one when he is resurrected

			delete TheGame;
			delete TheMenu;
			
// having cleared those, lets create new ones			
			this->TheGame = new Game(&OGLES);
			this->TheMenu = new Menu(&OGLES);
			this->TheState = MENU;
			// so we need to decide if we want to go back to the menu or shut the game down, TheGame-<Update will return when the level is done
				TheState = MENU;
			
			break;
		
		case	MENU:
			
			TheMenu->DisplayOptions();
			TheGame->WhichMap = TheMenu->WaitForInput(TheGame, TheGame->Level);  // << here we see an example of passing an address to the method, it will return a whichmap, but can also alter the Level variable
			TheGame->HowManyStars = 2;
			if (TheGame->Level == -1)
			{
				TheState = SHUTDOWN;
			} 
			else	TheState = GAME; // now the menu is done its time for the game
			break;
			
			case	SHUTDOWN :
				// time to shut the game down, clear up all the memory and return safely to the OS. Normally do this on an Esc key being pressed
			
			ExitRequested = true;
			delete TheGame;
			delete TheMenu;
			
			break;
		
		}
	}
}