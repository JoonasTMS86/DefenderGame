#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#define enemyWidth 91
#define enemyHeight 87
#define enemySpeed 5
#define humanWidth 91
#define humanHeight 87
#define humanfallingSpeed 5
#define playerWidth 91
#define playerHeight 87
#define playershipSpeed 20
#define playershipmaxSpeed 40
#define playerbulletSpeed 25
#define playerbulletWidth 181
#define playerbulletHeight 5
#define digitWidth 76
#define digitHeight 76
#define scorescreenXpos 200
#define scorescreenYpos 10
#define dirE 0
#define dirW 1
#define pointsforUfo 500
#define penaltyforKilling 500
#define stateTITLE 0
#define stateGAME 1
#define stateLOSTLIFE 2
SDL_Texture* usableTexture = NULL;
SDL_Surface* image1 = NULL;
SDL_Surface* scrollablegameArea = NULL;
SDL_Surface* ship = NULL;
SDL_Surface* bullet = NULL;
SDL_Surface* miscGfx = NULL;
SDL_Surface* scoreGfx = NULL;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Mix_Chunk* snd1 = NULL;
Mix_Chunk* snd2 = NULL;
Mix_Chunk* snd3 = NULL;

// Global variables.
int gameState, score;

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
		gWindow = SDL_CreateWindow("Defender!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				success = false;
			}
		}
	}
	return success;
}

void close()
{
	Mix_FreeChunk(snd1);
	Mix_FreeChunk(snd2);
	Mix_FreeChunk(snd3);
	snd1 = NULL;
	snd2 = NULL;
	snd3 = NULL;
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	Mix_Quit();
	SDL_Quit();
}

bool loadMedia()
{
	bool success = true;
	image1 = SDL_LoadBMP("data/defender1.bmp");
	if (image1 == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	scrollablegameArea = SDL_LoadBMP("data/defender2.bmp");
	if (scrollablegameArea == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	ship = SDL_LoadBMP("data/defender4.bmp");
	if (ship == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	bullet = SDL_LoadBMP("data/defender5.bmp");
	if (bullet == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	miscGfx = SDL_LoadBMP("data/defender3.bmp");
	if (miscGfx == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	scoreGfx = SDL_LoadBMP("data/defender6.bmp");
	if (scoreGfx == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	snd1 = Mix_LoadWAV("data/defender1.wav");
	if (snd1 == NULL)
	{
		printf("SDL Mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	snd2 = Mix_LoadWAV("data/defender2.wav");
	if (snd2 == NULL)
	{
		printf("SDL Mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	snd3 = Mix_LoadWAV("data/defender3.wav");
	if (snd3 == NULL)
	{
		printf("SDL Mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	return success;
}

// Here we read the keyboard controls of the game.
// The user can go up, down, left and right with the ship.
// The Right Ctrl key is used for firing the laser cannon.
void readKeyboard(int *gameareaxDelta, SDL_Rect *shipDest, SDL_Rect *bulletDest, bool *bulletfiring, int *bulletxDelta, int *playerDir)
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_UP])
	{
		if (gameState == stateGAME)
		{
			*&shipDest->y = *&shipDest->y - playershipSpeed;
			if (*&shipDest->y < 1)
			{
				*&shipDest->y = 0;
			}
		}
	}
	if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		if (gameState == stateGAME)
		{
			*&shipDest->y = *&shipDest->y + playershipSpeed;
			if (*&shipDest->y >= (768 - playerHeight))
			{
				*&shipDest->y = 768 - playerHeight;
			}
		}
	}
	if (currentKeyStates[SDL_SCANCODE_LEFT])
	{
		if (gameState == stateGAME)
		{
			if (*gameareaxDelta < 0)
			{
				*playerDir = dirW;
				*gameareaxDelta = *gameareaxDelta - 1;
				if (*gameareaxDelta < -playershipmaxSpeed) *gameareaxDelta = -playershipmaxSpeed;
			}
			else
			{
				if (*gameareaxDelta == 0)
				{
					*gameareaxDelta = *gameareaxDelta - 1;
				}
				else
				{
					*gameareaxDelta = *gameareaxDelta * -1;
				}
			}
		}
	}
	if (currentKeyStates[SDL_SCANCODE_RIGHT])
	{
		if (gameState == stateGAME)
		{
			if (*gameareaxDelta >= 0)
			{
				*playerDir = dirE;
				*gameareaxDelta = *gameareaxDelta + 1;
				if (*gameareaxDelta > playershipmaxSpeed) *gameareaxDelta = playershipmaxSpeed;
			}
			else
			{
				*gameareaxDelta = *gameareaxDelta + 1;
				*gameareaxDelta = *gameareaxDelta * -1;
			}
		}
	}
	if (currentKeyStates[SDL_SCANCODE_RCTRL])
	{
		if (gameState == stateTITLE)
		{
			gameState = stateGAME;
			score = 0;
		}
		else if (gameState == stateGAME)
		{
			if (!*bulletfiring)
			{
				Mix_PlayChannel(-1, snd1, 0);
				*bulletfiring = true;
				*&bulletDest->x = *&shipDest->x + 10;
				*&bulletDest->y = *&shipDest->y + 45;
				if (*playerDir == dirE)
				{
					*bulletxDelta = playerbulletSpeed;
				}
				else
				{
					*bulletxDelta = -playerbulletSpeed;
				}
			}
		}
	}
}

// Each enemy UFO will simply abduct the human that the enemy is associated with.
// So, we move the enemy closer to the human by adjusting the enemy's X,Y coordinates
// until they match the X,Y coordinates of the targeted human.
void enemyAI(bool *enemyAlive, bool *humanAbducted, bool *humanAlive, int *xpos, int *ypos, int *humanX, int *humanY)
{
	if (*humanAbducted == false)
	{
		int xdist1, xdist2, ydist1, ydist2;
		if (*xpos < *humanX)
		{
			*xpos = *xpos + enemySpeed;
		}
		if (*xpos > *humanX)
		{
			*xpos = *xpos - enemySpeed;
		}
		if (*ypos < *humanY)
		{
			*ypos = *ypos + enemySpeed;
		}
		if (*ypos > *humanY)
		{
			*ypos = *ypos - enemySpeed;
		}
		xdist1 = *xpos;
		xdist2 = *humanX;
		ydist1 = *ypos;
		ydist2 = *humanY;
		xdist1 = xdist1 - xdist2;
		ydist1 = ydist1 - ydist2;
		if (xdist1 < 0) xdist1 = xdist1 * -1;
		if (ydist1 < 0) ydist1 = ydist1 * -1;
		if ((xdist1 + ydist1) < (enemySpeed * 2))
		{
			*ypos = *ypos - (enemyHeight - 25);
			*humanAbducted = true;
		}
	}
	else
	{
		*ypos = *ypos - 1;
		*humanY = *humanY - 1;
		if (*ypos < 1)
		{
			*enemyAlive = false;
			*humanAlive = false;
		}
	}
}

bool handleEvent(SDL_Event& e)
{
	bool result = false;
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			result = true;
			break;
		}
	}
	return result;
}

int main(int argc, char* args[])
{
	int enemyXY[] = {0,0,91,0,182,0,273,0,364,0,455,0,546,0,637,0,728,0,819,0};
	int humanXY[] = {337,439,1439,609,2473,563,3523,471,4713,433,5819,551,6761,423,7705,577,8507,347,9545,529};
	int originalY[10];
	bool enemyAlive[] = {true,true, true, true, true, true, true, true, true, true};
	bool humanAbducted[] = {false,false, false, false, false, false, false, false, false, false};
	bool humanAlive[] = {true,true, true, true, true, true, true, true, true, true};
	SDL_Rect gamescreenDimensions, shipSrc, shipDest, bulletDest, enemySrc, enemyDest, humanSrc, humanDest,
		scoreSrc, scoreDest;
	int deathanimPhase, deathanimDelay, deathTime;
	int playeranimPhase = 0;
	int playeranimDelay = 0;
	int gameareaxDelta = 0;
	int bulletxDelta;
	int playerDir = dirE;
	bool bulletfiring = false;
	bool flipflop = false;
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		bool quit = false;
		SDL_Event e;
		for (int setYs = 0; setYs < 10; setYs++)
		{
			originalY[setYs] = humanXY[(setYs * 2) + 1];
		}
		humanSrc.x = enemyWidth;
		humanSrc.y = 0;
		humanSrc.w = humanWidth;
		humanSrc.h = humanHeight;
		enemySrc.x = 0;
		enemySrc.y = 0;
		enemySrc.w = enemyWidth;
		enemySrc.h = enemyHeight;
		shipDest.x = 467;
		shipDest.y = 200;
		shipSrc.y = 0;
		shipSrc.w = playerWidth;
		shipSrc.h = playerHeight;
		scoreSrc.y = 0;
		scoreSrc.w = digitWidth;
		scoreSrc.h = digitHeight;
		scoreDest.y = scorescreenYpos;
		SDL_SetColorKey(ship, 1, 0x000000);
		SDL_SetColorKey(bullet, 1, 0x000000);
		SDL_SetColorKey(miscGfx, 1, 0x000000);
		SDL_SetColorKey(scoreGfx, 1, 0x000000);
		gamescreenDimensions.x = 5120;
		gamescreenDimensions.y = 0;
		gamescreenDimensions.w = 1024;
		gamescreenDimensions.h = 768;
		SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_ShowCursor(SDL_DISABLE);
		gameState = stateTITLE;
		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				if(handleEvent(e)) quit = true;
			}
			readKeyboard(&gameareaxDelta, &shipDest, &bulletDest, &bulletfiring, &bulletxDelta, &playerDir);




			if (gameState == stateGAME || gameState == stateLOSTLIFE)
			{
				gamescreenDimensions.x = gamescreenDimensions.x + gameareaxDelta;
				if (gamescreenDimensions.x > 9216)
				{
					gameareaxDelta = 0;
					gamescreenDimensions.x = 9216;
				}
				if (gamescreenDimensions.x < 0)
				{
					gameareaxDelta = 0;
					gamescreenDimensions.x = 0;
				}
				if (gameareaxDelta != 0)
				{
					if (flipflop)
					{
						flipflop = false;
						if (gameareaxDelta < 0)
						{
							gameareaxDelta++;
						}
						else
						{
							gameareaxDelta--;
						}
					}
					else
					{
						flipflop = true;
					}
				}
				SDL_BlitSurface(scrollablegameArea, &gamescreenDimensions, image1, NULL);
				for (int enemyxyOffset = 0; enemyxyOffset < 20; enemyxyOffset += 2)
				{
					if (enemyAlive[enemyxyOffset / 2])
					{
						enemyAI(&enemyAlive[enemyxyOffset / 2], &humanAbducted[enemyxyOffset / 2], &humanAlive[enemyxyOffset / 2], &enemyXY[enemyxyOffset], &enemyXY[enemyxyOffset + 1],
							&humanXY[enemyxyOffset], &humanXY[enemyxyOffset + 1]);
					}
					// Place all the human gfx objects on the screen.
					if (humanAlive[enemyxyOffset / 2])
					{
						if (!humanAbducted[enemyxyOffset / 2] &&
							humanXY[enemyxyOffset + 1] < originalY[enemyxyOffset / 2])
						{
							humanXY[enemyxyOffset + 1] = humanXY[enemyxyOffset + 1] + humanfallingSpeed;
							if (humanXY[enemyxyOffset + 1] > originalY[enemyxyOffset / 2])
								humanXY[enemyxyOffset + 1] = originalY[enemyxyOffset / 2];
						}
						if (humanXY[enemyxyOffset] < (gamescreenDimensions.x + 1024))
						{
							humanDest.x = humanXY[enemyxyOffset] - gamescreenDimensions.x;
						}
						else if (humanXY[enemyxyOffset] > (gamescreenDimensions.x + 1024))
						{
							humanDest.x = gamescreenDimensions.x - humanXY[enemyxyOffset];
						}
						else
						{
							humanDest.x = humanXY[enemyxyOffset];
						}
						humanDest.y = humanXY[enemyxyOffset + 1];
						SDL_BlitSurface(miscGfx, &humanSrc, image1, &humanDest);
					}
					// Here we place all the enemy gfx objects on the screen.
					if (enemyAlive[enemyxyOffset / 2])
					{
						if (enemyXY[enemyxyOffset] < (gamescreenDimensions.x + 1024))
						{
							enemyDest.x = enemyXY[enemyxyOffset] - gamescreenDimensions.x;
						}
						else if (enemyXY[enemyxyOffset] > (gamescreenDimensions.x + 1024))
						{
							enemyDest.x = gamescreenDimensions.x - enemyXY[enemyxyOffset];
						}
						else
						{
							enemyDest.x = enemyXY[enemyxyOffset];
						}
						enemyDest.y = enemyXY[enemyxyOffset + 1];
						SDL_BlitSurface(miscGfx, &enemySrc, image1, &enemyDest);
					}
				}
				if (playerDir == dirE)
				{
					shipSrc.x = (0 + playeranimPhase) * playerWidth;
				}
				else
				{
					shipSrc.x = (2 + playeranimPhase) * playerWidth;
				}
				playeranimDelay++;
				if (playeranimDelay >= 3)
				{
					playeranimDelay = 0;
					playeranimPhase++;
					if (playeranimPhase >= 2) playeranimPhase = 0;
				}
				if (gameState == stateLOSTLIFE)
				{
					shipSrc.x = (4 + deathanimPhase) * playerWidth;
					deathanimDelay++;
					if (deathanimDelay >= 5)
					{
						deathanimDelay = 0;
						deathanimPhase++;
						if (deathanimPhase >= 5) deathanimPhase = 0;
					}
					deathTime--;
					if (deathTime <= 0) gameState = stateGAME;
				}
				SDL_BlitSurface(ship, &shipSrc, image1, &shipDest);

				int globX, globY;
				for (int posoffset = 0; posoffset < 20; posoffset++)
				{
					globX = bulletDest.x + gamescreenDimensions.x;
					globY = bulletDest.y;
					// Check if the player's fire hit an enemy.
					if (enemyAlive[posoffset / 2] == true)
					{
						if (globX >= ((enemyXY[posoffset]) - playerbulletWidth) &&
							globX < ((enemyXY[posoffset]) + enemyWidth) &&
							globY >= ((enemyXY[posoffset + 1]) - playerbulletHeight) &&
							globY < ((enemyXY[posoffset + 1]) + enemyHeight))
						{
							Mix_PlayChannel(-1, snd2, 0);
							humanAbducted[posoffset / 2] = false;
							enemyAlive[posoffset / 2] = false;
							score += pointsforUfo;
						}
					}
					// Check if the player's fire hit a human.
					if (humanAlive[posoffset / 2] == true)
					{
						if (globX >= ((humanXY[posoffset]) - playerbulletWidth) &&
							globX < ((humanXY[posoffset]) + humanWidth) &&
							globY >= ((humanXY[posoffset + 1]) - playerbulletHeight) &&
							globY < ((humanXY[posoffset + 1]) + humanHeight))
						{
							Mix_PlayChannel(-1, snd3, 0);
							humanAlive[posoffset / 2] = false;
							score -= penaltyforKilling;
							if (score < 0) score = 0;
						}
					}

					// Check if the player collided with an enemy.
					if (gameState != stateLOSTLIFE)
					{
						globX = shipDest.x + gamescreenDimensions.x;
						globY = shipDest.y;
						if (enemyAlive[posoffset / 2] == true)
						{
							if (globX >= ((enemyXY[posoffset]) - playerWidth) &&
								globX < ((enemyXY[posoffset]) + enemyWidth) &&
								globY >= ((enemyXY[posoffset + 1]) - playerHeight) &&
								globY < ((enemyXY[posoffset + 1]) + enemyHeight))
							{
								deathTime = 50;
								deathanimPhase = 0;
								deathanimDelay = 0;
								Mix_PlayChannel(-1, snd3, 0);
								gameState = stateLOSTLIFE;
							}
						}
					}


				}


				if (bulletfiring)
				{
					SDL_BlitSurface(bullet, NULL, image1, &bulletDest);
					bulletDest.x = bulletDest.x + bulletxDelta;
					if (bulletDest.x > 1023 || bulletDest.x < 0) bulletfiring = false;
				}

				// Display the player's score on the screen.
				int divider = 100000000;
				int readscore = score / divider;
				for (int doneDigits = 0; doneDigits < 9; doneDigits++)
				{
					// I decided to display the score the old school way:
					// Fixed 9 digits length with padding zeroes.
					if (doneDigits > 0)
					{
						readscore = score - ((score / divider) * divider);
						divider /= 10;
						readscore = readscore / divider;
					}
					scoreSrc.x = readscore * digitWidth;
					scoreDest.x = scorescreenXpos + (digitWidth * doneDigits);
					SDL_BlitSurface(scoreGfx, &scoreSrc, image1, &scoreDest);
				}


			}





			usableTexture = SDL_CreateTextureFromSurface(gRenderer, image1);
			SDL_RenderCopy(gRenderer, usableTexture, NULL, NULL);
			SDL_DestroyTexture(usableTexture);
			SDL_RenderPresent(gRenderer);
		}
	}
	close();
	return 0;
}