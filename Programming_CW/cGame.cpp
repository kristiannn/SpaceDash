/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();


	// Set filename
	theFile.setFileName("Data/usermap.dat"); //sets the file to userdata.dat
	// Check file is available
	if (!theFile.openFile(ios::in)) //open file for input output
	{
		cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl; // writes out it couldn't open the file in console
		fileAvailable = false; // sets bool fileavailable to false
		prevScore = 0; // sets prevScore to 0 as it couldn't load the file
	}
	else
	{
		cout << "File '" << theFile.getFileName() << "' opened for input!" << endl; // writes out it opened the file in console
		fileAvailable = true; // sets bool fileavailable to true
		string fileData = theFile.readDataFromFile(); // reads the data from the file into fileData string
		stringstream scoreData(fileData); // converts the data from fileData string into integer and puts it in scoreData
		scoreData >> prevScore; // sets prevScore to what was loaded from the file
		theFile.closeFile(); // closes file after use
	}


	score = 0; // sets score to 0
	sprintf_s(buffer, "Score: %d", score); // puts in "Score: 0" in buffer char
	sprintf_s(bestScore, "Best Score: %d", prevScore); // same as above but with previous score

	saved = false; //sets bool saved to false
	replay = false; // sets bool replay to false

	theAreaClicked = { 0, 0 }; //resets clicked area position

	// Store the textures
	textureName = { "theBackground", "background", "ship" , "asteroid"}; //sets the texture names
	texturesToUse = {  "Images/Bkg/bkg.png", "Images/backgrounds/MainLevel.bmp", "Images/objects/ship.png", "Images/objects/asteroid.png"}; // sets the path to each texture
	for (int tCount = 0; tCount < textureName.size(); tCount++) //creates an int tCount and sets it to 0, and increases it every time it goes through the loop until it goes through the amount of textures set in textureName
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]); // adds the textures with the texturemanager
	}

	// Store the button textures
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" }; // sets the names of all the buttons
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" }; //sets the path to all the buttons
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } }; // sets the position of all the buttons
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //creates an int bCount and sets it to 0 and it increases it every time it goes through the loop until it reaches the size of btnNameList array
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]); //adds the textures for the buttons with thetexturemanager
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //goes through all the buttons
	{
		cButton * newBtn = new cButton(); // creates a new instance of a class cButton [uses that class to hold information]
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount])); // sets the texture for the button 
		newBtn->setSpritePos(btnPos[bCount]); //sets the position of the button
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight()); //sets the size of the button to the size of the texture file
		theButtonMgr->add(btnNameList[bCount], newBtn); // adds the button
	}
	theGameState = MENU; // sets theGameState to MENU so when u start up the game it starts up in the menu
	theBtnType = EXIT; // sets theBtnType to EXIT

	// Create textures for Game Dialogue (text)
	fontList = { "frosty"}; // sets the names of the fonts
	fontsToUse = { "Fonts/FFF_Tusj.ttf"}; // sets the path to them
	for (int fonts = 0; fonts < fontList.size(); fonts++) // goes through them in a loop
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36); // adds them
	}

	// Create text Textures
	gameTextNames = { "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt", "Score", "Best Score"}; // sets the names of the text textures
	gameTextList = { "Space Dash", "Avoid the Asteroids!", "Use W and S to move up and down.", "Thanks for playing!", "See you again soon!", buffer, bestScore}; //sets the text that each text texture holds
	for (int text = 0; text < gameTextNames.size(); text++) // goes through all the text textures
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("frosty")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 })); //adds them
	}

	// Load game sounds
	soundList = { "music", "click", "explosion" }; //sets the sound names
	soundTypes = { MUSIC, SFX , SFX}; // sets the sound types
	soundsToUse = { "Audio/Music/music.mp3", "Audio/SFX/Click.wav", "Audio/SFX/Explosion.wav"}; // sets the sound paths
	for (int sounds = 0; sounds < soundList.size(); sounds++) // goes through all the sounds
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]); //adds them
	}

	theSoundMgr->getSnd("music")->play(-1); //plays the music in an infinite loop

	//This uses cSprite class 
	spriteBkgd.setSpritePos({ 0, 0 }); //sets the position of the background to 0,0
	spriteBkgd.setTexture(theTextureMgr->getTexture("background")); //sets the texture of the background
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("background")->getTWidth(), theTextureMgr->getTexture("background")->getTHeight()); //sets the dimensions of the background to the background file's dimensions

	//This uses cRocket class
	theRocket.setSpritePos({ 50, 350 }); //sets the position of the rocket to 50, 350 - this will be the starting position of the rocket
	theRocket.setTexture(theTextureMgr->getTexture("ship")); // sets the texture to the ship
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("ship")->getTWidth(), theTextureMgr->getTexture("ship")->getTHeight()); // sets the dimensions to the ones of ship.png
	theRocket.setRocketVelocity({ 0, 0 }); // sets the velocity to 0


	// Create vector array of textures
	for (int astro = 0; astro < 7; astro++) // creates 7 asteroids
	{
		theAsteroids.push_back(new cAsteroid); //creates a new instance of an asteroid
		theAsteroids[astro]->setSpritePos({ 1500 + ((std::rand() % 200 + 300) * astro), std::rand() % 700 }); // sets its position randomly, so they're not positioned the same way every time
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture("asteroid")); // sets the texture to asteroid
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture("asteroid")->getTWidth(), theTextureMgr->getTexture("asteroid")->getTHeight()); // sets the dimensions to the dimensions of the asteroid file
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f }); // sets velocity to 3,3
		theAsteroids[astro]->setActive(true); //activates it
	}
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true; //sets loop to true

	while (loop) //plays the game while loop is true
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer); //clears theRenderer
	switch (theGameState) //defines theGameState switch
	{
	case MENU: //MENU case of it
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); //renders the background

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //gets TitleTxt
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; // sets the position of the text
		scale = { 1, 1 }; // sets the scale of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders the text by using the parameters above

		tempTextTexture = theTextureMgr->getTexture("CreateTxt"); //gets (uses) CreateTxt
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets the position of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders text using parameters above
		
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt"); //gets (uses) DragDropTxt
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets the position of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); // renders the text using the parameters above


		// Renders all the buttons 
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale()); //renders the play button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 }); //sets the position of the exit button
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); // renders the exit button
	}
	break;
	case PLAYING:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); //renders the background
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //gets titleTxt texture
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };  // sets the position of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);//renders the text by using the parameters above


		//Render the Score
		theTextureMgr->addTexture("Score", theFontMgr->getFont("frosty")->createTextTexture(theRenderer, buffer, SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 })); //adds the texture 
		tempTextTexture = theTextureMgr->getTexture("Score"); //gets the texture added above
		pos = { 850, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; // sets the position of it
		scale = { 1, 1 }; // sets the scale of it
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); // renders it

		// render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());

		// Render each asteroid in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}


		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 740, 650 }); //sets the position of the exit button
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //renders the exit button
	}
	break;
	case END:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); //renders the background
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //gets the texture
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders it

		tempTextTexture = theTextureMgr->getTexture("ThanksTxt"); //gets the texture
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders it

		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt"); //gets the texture
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders it

		//Render the Score
		theTextureMgr->addTexture("Score", theFontMgr->getFont("frosty")->createTextTexture(theRenderer, buffer, SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 })); //adds score texture
		tempTextTexture = theTextureMgr->getTexture("Score"); // gets texture added above
		pos = { 450, 250, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; // sets its position
		scale = { 1, 1 }; // sets its scale
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); // uses parameters above and renders it

		//Render Best Score
		theTextureMgr->addTexture("Best Score", theFontMgr->getFont("frosty")->createTextTexture(theRenderer, bestScore, SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 })); // adds the best score texture
		tempTextTexture = theTextureMgr->getTexture("Best Score"); //uses it
		pos = { 450, 300, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //sets its position
		scale = { 1, 1 }; // sets its scale
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //renders it using the parameters above

		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 }); //sets the position of the menu button
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale()); //renders it
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 }); //sets the position of the exit button
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //renders it
	}
	break;
	case QUIT:
	{
		loop = false; //sets loop to false hence quits the game
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer); //refreshes theRenderer
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer); //refreshes theRenderer
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Check Button clicked and change state
	switch (theGameState)
	{
		case MENU:
		{
			if (replay) //if replay is true
			{
				for (int astro = 0; astro < 7; astro++) //goes through the asteroids
				{
					theAsteroids[astro]->setSpritePos({ 1500 + ((std::rand() % 200 + 300) * astro), std::rand() % 700 }); // respawns asteroids on restart, so they're in starting position
				}
				score = 0; //sets score to 0 so you don't continue with the score you've finished your last game with
				theRocket.setSpritePos({ 50, 350 }); // puts the rocket in starting position
				theRocket.setRocketVelocity({ 0, 0 }); // makes sure it doesn't move if you've died while rocket was still moving
				replay = false; //sets replay to false
			}

			saved = false; //sets saved to false
			theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked); //on exit button click changes state to QUIT
			theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked); //on play button click changes state to PLAYING
		}
		break;
		case PLAYING:
		{
			theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked); // changes theGameState to END on exit button click

			if ((SDL_GetTicks() - currentTime2) / 1000.0f > 1.0f) // when 1 second has passed 
			{
				score++; //increase score by 1
				sprintf_s(buffer, "Score: %d", score); //updates the score holder for rendering
				currentTime2 = SDL_GetTicks(); // sets currentTime2 to the current time so it can compare it later
				theTextureMgr->deleteTexture("Score"); //deletes the score texture so it renders out the new one
			}

			//Updates the position of the asteroids by going through the whole array (have to use iterator because it's an array)
			vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
			while (asteroidIterator != theAsteroids.end())
			{
				if ((*asteroidIterator)->isActive() == false)
				{
					asteroidIterator = theAsteroids.erase(asteroidIterator);
				}
				else
				{
					(*asteroidIterator)->update(deltaTime);
					++asteroidIterator;
				}
			}

			/*
			==============================================================
			| Check for collisions
			==============================================================
			*/
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator) //goes through all the asteroids
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &theRocket.getBoundingRect())) //compares them 1 by 1 with the rocket's rect (1 by 1 because it's a for above loop)
				{
					theSoundMgr->getSnd("explosion")->play(0); //plays the explosion sound once
					std::cout << "Collision" << std::endl; // writes out Collision in console
					theGameState = END; //sets theGameState to END
				}
			}

			// Update the Rocket's position
			theRocket.update(deltaTime);


		}
		break;
		case END:
		{
			if (score > prevScore && !saved) //if you've beaten your high score and it hasn't saved it yet
			{
				// Check file is available
				if (!theFile.openFile(ios::out)) //open file for input output
				{
					cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl; //writes in console if it couldn't open file
				}
				else
				{
					cout << "File '" << theFile.getFileName() << "' opened for input!" << endl; //prints out that the file has been opened
					theFile.writeDataToFile(to_string(score)); //converts the score to a string and writes it to the file
					theFile.closeFile(); //closes the file
					prevScore = score; //sets the previous score to the current score 
					sprintf_s(bestScore, "Best Score: %d", prevScore); //updates the bestscore holder for texture rendering
					saved = true; // sets saved to true so it doesn't go through this again
				}
			}

			replay = true; //sets replay to true so it resets the positions of everything so you can play again
			theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked); //sets thegamestate to quit when u press the button
			theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked); //sets thegamestate to menu when u press the button
		}
		break;
		case QUIT:
		{
		}
		break;
		default:
			break;
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false; //on QUIT event sets theLoop to false
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
					if (theGameState == PLAYING)
					{

					}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					if (theGameState == PLAYING)
					{
						theAreaClicked = { event.motion.x, event.motion.y };

					}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			{
			}
			break;


			//KEY RELEASE
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_DOWN:
				{
								  theRocket.setSpriteTranslation({ 0, 0 }); //stops theRocket's movement on key release
				}
					break;

				case SDLK_UP:
				{
								   theRocket.setSpriteTranslation({ 0, 0 }); //stops theRocket's movement on key release
				}
					break;
				case SDLK_s:
				{
								   theRocket.setSpriteTranslation({ 0, 0 }); //stops theRocket's movement on key release
				}
					break;

				case SDLK_w:
				{
								   theRocket.setSpriteTranslation({ 0, 0 }); //stops theRocket's movement on key release
				}
					break;
				}
				break;

			//KEY PRESS
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;

				case SDLK_s:
				{
								   theRocket.setSpriteTranslation({ 0, -12 }); //moves theRocket on button press (or hold)
				}
					break;

				case SDLK_w:
				{
								   theRocket.setSpriteTranslation({ 0, 12 }); //moves theRocket on button press (or hold)
				}
					break;


				case SDLK_DOWN:
				{
								  theRocket.setSpriteTranslation({ 0, -12 }); //moves theRocket on button press (or hold)
				}
				break;

				case SDLK_UP:
				{
								theRocket.setSpriteTranslation({ 0, 12 }); //moves theRocket on button press (or hold)
				}
				break;
				case SDLK_RIGHT:
				{
				}
				break;

				case SDLK_LEFT:
				{
				}
				break;
				case SDLK_SPACE:
				{
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

