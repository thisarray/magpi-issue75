#include "Menu.h"
#include <string>
#include "Game.h"
#include "Input.h"
#include "Bob.h" // bob has some things we want (keys)

Menu::Menu()
{
}

Menu::~Menu()
{}
Menu::Menu(OGL* TheOGL)
{
		MyText = new Text();
		MyText->WhereisOGL = TheOGL;
}

void Menu::DisplayOptions()
{

	char TempBuffer[130];
	std::sprintf(TempBuffer, "Not a very inspiring Menu!");

	
//positioning text is actually quite a challenge, since we have to factor in the size of the glyph and the scale
// we're estimating it to be mostly 32 pixel and scale 1.8 and 2.4
// for now we're hard coding screen locations, its more precise in pixel terms but harder to do clearly and quickly
// it would make sense to re-write the Rendertext routine to work in column/row system
	MyText->RenderText(TempBuffer, 32*10*1.8f, 1080-(32*2*1.8f), 1.8f);
	
	std::sprintf(TempBuffer, "But it will give us an idea");
	MyText->RenderText(TempBuffer, 32 * 10 * 1.8f, 1080-(32*5*1.8f), 1.8f);
	
	std::sprintf(TempBuffer, "of how a basic menu works.");
	MyText->RenderText(TempBuffer, 32 * 10 * 1.8f, 1080 - (32 * 8 * 1.8f), 1.8f);
	std::sprintf(TempBuffer, "and how to space the text.");
	MyText->RenderText(TempBuffer, 32 * 10 * 1.8f, 1080 - (32 * 11 * 1.8f), 1.8f);
	
	
	std::sprintf(TempBuffer, "Press 1 to 4 to select a simple map");
	MyText->RenderText(TempBuffer, 32 * 3 * 2.4f, 1080 - (32 * 12 * 2.4f), 2.4f);
// and just for fun, lets have some tiny text	
	std::sprintf(TempBuffer, "Remember these are very simple demo maps");
	MyText->RenderText(TempBuffer, (1920/2)-100, 1080 - (32 *68 * 0.45f), 0.45f);


	// put our image on screen
	eglSwapBuffers(MyText->WhereisOGL->state.display, MyText->WhereisOGL->state.surface);
// now we don't (in this case) plan to change the display, until we get a key, so there's no need to do a loop to update the screen each cycle, render once and leave it there
	
}

// lets get the key

int* Menu::WaitForInput(Game* TheGame, int &Level) // we need access to they keys, as always we could (and should) just pass the key handlers and input
{
	// One thing we don't have easy acccess to is a key handler, its normally contained in Bob but its clumsy to constantly ask for that so lets make s shortcut
	
	Input* MenuInput = TheGame->MyBob->IH; /// we will make a simple local version here
	
	bool HaveWeGotAResult = false;
	// unlike a game loop, this very simple menu is not updating the screen display, so there's no need to do a clear/update/draw cycle
	// though if we added more interactive content we would need to
//int* WhichMap = nullptr;
	
	while (HaveWeGotAResult == false)
	{
		if (MenuInput->TestKey(KEY_SPACE))
			HaveWeGotAResult = true; // ALLOW SPACE TO DEFAULT TO MAP 1
// LETS TEST ALL KEYS		
			if(MenuInput->TestKey(KEY_1))
		{
			Level = 0;
			return *TheGame->Map1;
		}
		// we "could" use else here bit its going to fall to this line anyway and do another test so its redundant		
					if(MenuInput->TestKey(KEY_2))
		{
			Level = 1;
			return *TheGame->Map2;
		}
			
		if (MenuInput->TestKey(KEY_3))
		{
			Level = 2;
			return *TheGame->Map3;
		}	
		
		if (MenuInput->TestKey(KEY_4))
		{
			Level = 3;
			return *TheGame->Map4;
		}	
// test for the escape		
		if (MenuInput->TestKey(KEY_ESC))
		{
			Level = -1; // give an impossible value
			return *TheGame->Map1;
			
		}	

	}

	

	return *TheGame->Map1;
}
