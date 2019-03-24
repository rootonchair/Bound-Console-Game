#include <iostream>
#include <chrono>
#include <SDL_mixer.h>
#include <SDL.h>
#include "ScreenBuffer/ScreenBuffer.h"
#include "ScreenBuffer/Color.h"
#include "AudioPlayer/AudioPlayer.h"

// === CONSTANTS DECLARE ===
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 80;

const int FONT_WIDTH = 8;
const int FONT_HEIGHT = 8;

const int  GAME_WIDTH = 48;
const int GAME_HEIGHT = 78;

const int BALL_RADIUS = 0;
const int BALL_HEIGHT = 3;

const int SPACE_WIDTH = 5;
const int WALL_HEIGHT = 4;

const int NUMBER_OF_WALLS = 3;// to @ThanhUy: Suppose to be 4 ?

const int BALL_LIMIT = 50;

// === ENUM DECLARE ===

// === STRUCT DECLARE ===
struct Ball {
	float x;
	float y;
	// Add something if you need
};

struct Wall {
	float spaceX;
	float spaceY;
	bool passed = 0;
};

// === GLOBAL VARIABLES DECLARE===
bool gameOver;
int score;
int bestScore;
int SectionHeigth;
Ball ball;
Wall Obstacle[NUMBER_OF_WALLS];
float SectionHeight;


// === FUNCTION DECLARE ===
void Intro();
int Menu();
void Init();
void Depose();
void ResetGame();
void onGameUpdate(float elapsedTime);



int main(int argc, char* argv[]) {
	Init();
	Intro();
	while ( Menu() ) {
		ResetGame();
		auto t1 = std::chrono::system_clock::now();
		auto t2 = t1;
		while (!gameOver) {
			t2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = t2 - t1;
			t1 = t2;
			float fElapsedTime = elapsedTime.count();

			onGameUpdate(fElapsedTime);
			ScreenBuffer::drawToConsole();
		}
	}

	Depose();
	return 0;
}

// -------- FUNCTIONS FOR STARTING STATE --------

/*
	Initialize global variables, loading game assets, records or something that will need to init once in program
*/
void Init() {
	ScreenBuffer::SetupBufferScreen(SCREEN_WIDTH, SCREEN_HEIGHT, FONT_WIDTH, FONT_HEIGHT);
	// TODO: Init AudioPLayer
	// TODO: Load game asset
}

/*
	Depose global variables, game assets or something need to clean up when the program is done
*/
void Depose() {
	ScreenBuffer::deposeBuffer();
	// TODO: Free AudioPLayer
	// TODO: Free game asset
}

// === INTRO ===
void Intro() {

}

// === MENU ===
int Menu() {

}



// -------- FUNCTIONS FOR PLAYING STATE --------

/*
	Getting ready to start the game again. Reset/ Init ball XY or score,...
*/
void ResetGame() {
	// Initialize global variables
	gameOver = false;
	ball.x = 1 / 2 * GAME_WIDTH;
	ball.y = 1 / 3 * GAME_HEIGHT;
	// @ThanhUy TODO: Init Walls
	SectionHeight = ((float)GAME_HEIGHT / NUMBER_OF_WALLS + 1) + 5; // to @ThanhUy: I need you to add comment explaining for this 
	for (int i = 0; i < 3; i++)
	{
		Obstacle[i].spaceX = rand() % (GAME_WIDTH - 10);
		Obstacle[i].spaceY = (i + 1)*SectionHeight;
	}
	Obstacle[4].spaceY = 31; // 31 is just a number to make sure that Wall 4 won't show up in the screen until time.
}

// === HANDLE PLAY INPUT ===
void GameHandleInput() {

}

// === PLAY LOGIC ===
void GameLogic(float elapsedTime) {
	// TODO: Update ball
	ObstacleLogic(elapsedTime);
	// TODO: Check for collision, increase score or game over
	// TODO: Update Game Camera
}

// to @ThanhUy: Please remove all code for drawing in your function. It should not be there
void ObstacleLogic(float fElapsedTime)
{
	//TODO: Clear the screen first. But i don't know if it affect other people code. --> to @Thanh Uy: Yes, it does. Beside, it's DRAWING's reponsibility

	for (int i = 0; i < 3; i++)
	{
		if (Obstacle[i].spaceY <= 1)
		{
			Obstacle[4].spaceX = Obstacle[i].spaceX;
			Obstacle[4].spaceY = Obstacle[i].spaceY;
			Obstacle[i].spaceX = rand() % (GAME_WIDTH - 10);
			Obstacle[i].spaceY = GAME_HEIGHT - 1;
		}
		//Buffer::fillRect(1, Obstacle[i].spaceY, GAME_WIDTH - SPACE_WIDTH - Obstacle[i].spaceX, Obstacle[i].spaceY + WALL_HEIGHT, L' ', BG_CYAN);
		//Buffer::fillRect(GAME_WIDTH - Obstacle[i].spaceX, Obstacle[i].spaceY, GAME_WIDTH - 1, Obstacle[i].spaceY + WALL_HEIGHT, L' ', BG_CYAN);
		Obstacle[i].spaceY -= 8.0f*fElapsedTime;

		if (Obstacle[4].spaceY + WALL_HEIGHT >= 0 && Obstacle[4].spaceY != 31) //31 is declare in the Reset Game 
		{
			//Buffer::fillRect(1, 1, GAME_WIDTH - SPACE_WIDTH - Obstacle[4].spaceX, Obstacle[4].spaceY + WALL_HEIGHT, L' ', BG_BLUE);
			//Buffer::fillRect(GAME_WIDTH - Obstacle[4].spaceX, 1, GAME_WIDTH - 1, Obstacle[4].spaceY + WALL_HEIGHT, L' ', BG_BLUE);
			Obstacle[4].spaceY -= 6.0f*fElapsedTime;
		}


	}

}

void DrawLogic()
{
	if (ball.y > BALL_LIMIT*1.0)
	{ 
		for (int i = 0; i <= NUMBER_OF_WALLS; i++)
		{
			Obstacle[i].spaceY -= ball.y - BALL_LIMIT*1.0;
			ball.y = BALL_LIMIT*1.0;
		}
	}
}

void Collision()
{
	for (int i = 0; i <= NUMBER_OF_WALLS; i++)
	{
		if (Obstacle[i].spaceY - ball.y <= BALL_HEIGHT/2 && Obstacle[i].spaceY - ball.y >= -WALL_HEIGHT)
		{
			if (!(ball.x - Obstacle[i].spaceX >= BALL_HEIGHT/2 && ball.x - Obstacle[i].spaceX <= SPACE_WIDTH - BALL_HEIGHT/2 - 1))
				gameOver = 1;
		}
	}
	if (ball.y < 1) gameOver = 1;
	for (int i = 0; i <= NUMBER_OF_WALLS; i++)
	{
		if (ball.y == Obstacle[i].spaceY + 4 && Obstacle[i].passed == 0)
		{
			score++;
			Obstacle[i].passed = 1;
		}
	}
}

// === PLAY DRAW ===
void GameDraw() {

}

/*
	The update loop of our game
*/
void onGameUpdate(float elapsedTime) {
	GameHandleInput();
	GameLogic(elapsedTime);
	GameDraw();
}


// -------- end --------