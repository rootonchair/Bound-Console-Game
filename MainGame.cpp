﻿#include <iostream>
#include <conio.h>
#include <chrono>
#include <fstream>
#include <SDL_mixer.h>
#include <SDL.h>
#include "ScreenBuffer/ScreenBuffer.h"
#include "ScreenBuffer/Color.h"
#include "AudioPlayer/AudioPlayer.h"
#include "SpriteRenderer/SpriteRenderer.h"
#include "ControlBall.h"

// === CONSTANTS DECLARE ===
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 80;

const int FONT_WIDTH = 8;
const int FONT_HEIGHT = 8;

const int  GAME_WIDTH = 48;
const int GAME_HEIGHT = 78;

const int BALL_RADIUS = 2;

const int SPACE_WIDTH = 7;
const int WALL_HEIGHT = 4;

const int NUMBER_OF_WALLS = 4;

const int BALL_LIMIT = 50;

const int KEY_P = 0x50;
const int KEY_O = 0x4F;
const int KEY_C = 0x43;
const int KEY_H = 0x48;
const int KEY_E = 0x45;


// === ENUM DECLARE ===

// === STRUCT DECLARE ===

struct Wall {
	float spaceX;
	float spaceY;
	bool passed = 0;
};

// === GLOBAL VARIABLES DECLARE===
bool gameOver;
int score= 0;
int bestScore=0;
int MaxScore=9999999;
int SectionHeigth;
Ball ball;
Wall Obstacle[NUMBER_OF_WALLS];
int Obstacleupdate;
int CaseUpdate;
//Mix_Chunk *intro = Mix_LoadWAV("Bound-Console-Game/Music/Intro");


// === FUNCTION DECLARE ===
void Intro();
int Menu();
void Init();
void Depose();
void ResetGame();
void onGameUpdate(float elapsedTime);
void UpdateAndShowScore();
int ReadBestScore();
void SaveBestScore();
void GameOver();

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
		AudioPlayer::PlayEffect("Bound-Console-Game/GameData/Music/Die.wav");
		GameOver();
	}
	Depose();
	// TODO: Close Window
	return 0;
}

// -------- FUNCTIONS FOR STARTING STATE --------

/*
	Initialize global variables, loading game assets, records or something that will need to init once in program
*/
Sprite 
Logo_Outline,
Menu_Play,
Menu_Options,
Menu_Credits,
Menu_Help,
Menu_Exit;

Sprite
Skin1_Ball, Skin1_LeftObs, Skin1_RightObs, //Flappy Bird Skin
Skin2_Ball, Skin2_LeftObs, Skin2_RightObs, //Super Mario Skin
*SBall, *SLeftObs, *SRightObs; //Selected Skin

std::string SkinName;

int DefaultSkin;

void LoadMenuData();
void LoadSkin();

void Init() {
	ScreenBuffer::SetupBufferScreen(SCREEN_WIDTH, SCREEN_HEIGHT, FONT_WIDTH, FONT_HEIGHT);
	// TODO: Load game asset
	// Load Intro and Menu
	AudioPlayer::initPlayer();
	LoadSprite(Logo_Outline, "Bound-Console-Game/GameData/Logo/Logo_Outline.dat");
	LoadMenuData();
	//
	LoadSkin();
}

/*
	Depose global variables, game assets or something need to clean up when the program is done
*/
void Depose() {
	ScreenBuffer::deposeBuffer();
	// TODO: Free AudioPLayer
	// TODO: Free game asset
	// Free Menu assets
	FreeSprite(Logo_Outline);
	FreeSprite(Menu_Play);
	FreeSprite(Menu_Options);
	FreeSprite(Menu_Credits);
	FreeSprite(Menu_Help);
	FreeSprite(Menu_Exit);
}

// === INTRO ===

void Intro() {
	Sprite Black;
	LoadSprite(Black, "Bound-Console-Game/GameData/Logo/Black.dat");
	Sprite Logo_Inner;
	LoadSprite(Logo_Inner, "Bound-Console-Game/GameData/Logo/Logo_Inner.dat");
	Sleep(500);
	DrawSprite(Logo_Inner, 11, 35);
	ScreenBuffer::drawToConsole();
	AudioPlayer::PlayEffect("Bound-Console-Game/GameData/Music/Intro.wav");
	Sleep(500);
	for (int i = 0; i < 6; i++)
	{
		Sleep(50);
		DrawSprite(Logo_Outline, 10, 34);
		for (int j = 0; j < 5; j++)
			if (j != i)
				DrawSprite(Black, 12 * j + 10, 34);
		DrawSprite(Logo_Inner, 11, 35);
		ScreenBuffer::drawToConsole();
	}
	for (int i = 0; i < 5; i++)
	{
		Sleep(50);
		DrawSprite(Logo_Outline, 10, 34);
		for (int j = i + 1; j < 5; j++)
			if (j != i)
				DrawSprite(Black, 12 * j + 10, 34);
		DrawSprite(Logo_Inner, 11, 35);
		ScreenBuffer::drawToConsole();
	}
	Sleep(1000);
	FreeSprite(Logo_Inner);
	FreeSprite(Black);
}

// === MENU ===



void LoadMenuData()
{
	LoadSprite(Menu_Play, "Bound-Console-Game/GameData/Menu/Menu_Play.dat");
	LoadSprite(Menu_Options, "Bound-Console-Game/GameData/Menu/Menu_Options.dat");
	LoadSprite(Menu_Credits, "Bound-Console-Game/GameData/Menu/Menu_Credits.dat");
	LoadSprite(Menu_Help, "Bound-Console-Game/GameData/Menu/Menu_Help.dat");
	LoadSprite(Menu_Exit, "Bound-Console-Game/GameData/Menu/Menu_Exit.dat");
}

void DrawMenu()
{
	ScreenBuffer::fillBuffer(32, 0);
	ScreenBuffer::drawToConsole();
	DrawSprite(Logo_Outline, 10, 5);
	DrawSprite(Menu_Play, 20, 30);
	DrawSprite(Menu_Options, 20, 39);
	DrawSprite(Menu_Credits, 20, 48);
	DrawSprite(Menu_Help, 20, 57);
	DrawSprite(Menu_Exit, 20, 66);
	ScreenBuffer::drawString(35, 25, "BEST: ", 10);

	//Get Best Score from file txt to show in Menu
	ReadBestScore();

	std::string Score = "";
	int Temp = bestScore;
	if(bestScore==0)
		Score='0';
	else
	{
		while (Temp > 0)
		{
			Score = (char)(Temp % 10 + 48) + Score;
			Temp /= 10;
		}
	}
	ScreenBuffer::drawString(41, 25, Score, 10);
	ScreenBuffer::drawToConsole();
}

void LoadSkin()
{
	//Flappy Bird Skin
	LoadSprite(Skin1_Ball, "Bound-Console-Game/GameData/Skins/FlappyBird/Ball.dat");
	LoadSprite(Skin1_LeftObs, "Bound-Console-Game/GameData/Skins/FlappyBird/Left_Obs.dat");
	LoadSprite(Skin1_RightObs, "Bound-Console-Game/GameData/Skins/FlappyBird/Right_Obs.dat");
	//Super Mario Skin
	LoadSprite(Skin2_Ball, "Bound-Console-Game/GameData/Skins/SuperMario/Ball.dat");
	LoadSprite(Skin2_LeftObs, "Bound-Console-Game/GameData/Skins/SuperMario/Left_Obs.dat");
	LoadSprite(Skin2_RightObs, "Bound-Console-Game/GameData/Skins/SuperMario/Right_Obs.dat");
	//Load Saved Data
	std::ifstream SavedSkin("SavedSkin");
	SavedSkin >> DefaultSkin;
	switch (DefaultSkin)
	{
	case 1:
		SBall = &Skin1_Ball;
		SLeftObs = &Skin1_LeftObs;
		SRightObs = &Skin1_RightObs;
		SkinName = "FLAPPY BIRD";
		break;
	case 2:
		SBall = &Skin2_Ball;
		SLeftObs = &Skin2_LeftObs;
		SRightObs = &Skin2_RightObs;
		SkinName = "SUPER MARIO";
		break;
	}
	SavedSkin.close();
}

void Options()
{
	ScreenBuffer::fillBuffer(32, 0);
	ScreenBuffer::drawLine(2, 40, 77, 40, 249, Color::FG_WHITE);
	ScreenBuffer::drawString(9, 40, " SKIN ", Color::FG_GREEN);
	ScreenBuffer::drawRect(2, 44, 24, 69, 249, Color::FG_WHITE);
	ScreenBuffer::drawRect(28, 44, 77, 69, 249, Color::FG_WHITE);
	ScreenBuffer::drawString(4, 46, "[1] FLAPPY BIRD", Color::FG_YELLOW);
	ScreenBuffer::drawString(4, 48, "[2] SUPER MARIO", Color::FG_YELLOW);
	ScreenBuffer::drawString(2, 71, "PRESS NUMBER KEY TO SELECT", Color::FG_BLUE);
	ScreenBuffer::drawString(52, 75, "[ENTER]: RETURN TO MENU", FG_DARK_CYAN);
	ScreenBuffer::drawToConsole();
	//Default
	DrawSprite(*SBall, 50, 49);
	DrawCrop(*SLeftObs, 29, 58, 36, 0, 49, 6);
	DrawCrop(*SRightObs, 53, 58, 0, 0, 23, 6);
	ScreenBuffer::drawString(30, 46, SkinName, Color::FG_YELLOW);
	ScreenBuffer::drawToConsole();
	//
	int Key;
	while (true)
	{
		while (_kbhit()) _getch();
		Key = _getch();
		switch (Key)
		{
		case 0x31:
			SBall = &Skin1_Ball;
			SLeftObs = &Skin1_LeftObs;
			SRightObs = &Skin1_RightObs;
			SkinName = "FLAPPY BIRD";
			DefaultSkin = 1;
			break;
		case 0x32:
			SBall = &Skin2_Ball;
			SLeftObs = &Skin2_LeftObs;
			SRightObs = &Skin2_RightObs;
			SkinName = "SUPER MARIO";
			DefaultSkin = 2;
			break;
		case 13:
			std::ofstream SavedSkin("SavedSkin");
			SavedSkin << DefaultSkin;
			SavedSkin.close();
			return;
		}
		ScreenBuffer::fillRect(50, 49, 54, 53, 219, 0);
		DrawSprite(*SBall, 50, 49);
		DrawCrop(*SLeftObs, 29, 58, 36, 0, 49, 6);
		DrawCrop(*SRightObs, 53, 58, 0, 0, 23, 6);
		ScreenBuffer::drawLine(29, 45, 76, 45, 219, 0);
		ScreenBuffer::drawString(30, 46, SkinName, Color::FG_YELLOW);
		ScreenBuffer::drawToConsole();
	}
}

void Credits() // @GiaVinh: don't try to use color value. I'm not a good designer
{	
	ScreenBuffer::fillBuffer(32, 0);
	Sprite Credits;
	LoadSprite(Credits, "Bound-Console-Game/GameData/Credits/Credits.dat");
	DrawSprite(Credits, 25, 5);
	ScreenBuffer::drawString(10, 12, "____________________________________________________________");
	ScreenBuffer::drawString(16, 14, "This game is a school project created by a group", FG_GREEN);
	ScreenBuffer::drawString(28, 15, "of first - year students", FG_GREEN);
	ScreenBuffer::drawString(16, 17, "Bound is 100% a Command Line game which graphics", FG_GREEN);
	ScreenBuffer::drawString(19, 18, "are all characters and background coloring", FG_GREEN);
	ScreenBuffer::drawString(20, 20, "Console rendering was inspired by javidx9's", FG_GREEN);
	ScreenBuffer::drawString(27, 21, "olcConsoleGameEngine project", FG_GREEN);
	ScreenBuffer::drawString(31, 30, "LEADER", FG_DARK_CYAN);
	ScreenBuffer::drawString(39, 30, "PHAM HONG VINH");
	ScreenBuffer::drawString(29, 35, "DESIGNER", FG_DARK_CYAN);
	ScreenBuffer::drawString(39, 35, "NGUYEN TRAN TRUNG");
	ScreenBuffer::drawString(27, 40, "PROGRAMMER", FG_DARK_CYAN);
	ScreenBuffer::drawString(39, 40, "VO TRONG GIA VINH");
	ScreenBuffer::drawString(39, 42, "LE THANH VIET");
	ScreenBuffer::drawString(39, 44, "BUI THANH UY");
	ScreenBuffer::drawString(21, 50, "MUSIC/SFX SOURCE", FG_DARK_CYAN);
	ScreenBuffer::drawString(39, 50, "FREESOUND.COM");
	ScreenBuffer::drawString(25, 55, "LIBRARY USED", FG_DARK_CYAN);
	ScreenBuffer::drawString(39, 55, "SDL2/MIXER");
	ScreenBuffer::drawString(52, 75, "[ENTER]: RETURN TO MENU", FG_DARK_CYAN);
	ScreenBuffer::drawToConsole();
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) break;
	}
	FreeSprite(Credits);
}

void Help()
{
	ScreenBuffer::fillBuffer(32, 0);
	Sprite Help;
	LoadSprite(Help, "Bound-Console-Game/GameData/Help/Help.dat");
	DrawSprite(Help, 32, 5);
	FreeSprite(Help);
	ScreenBuffer::drawString(10, 12, "____________________________________________________________");
	ScreenBuffer::drawString(10, 20, "ABOUT BOUND:", FG_GREEN);
	ScreenBuffer::drawString(24, 25, "Bound is an endless game, in which the goal is");
	ScreenBuffer::drawString(24, 26, "to keep your character alive while navigating");
	ScreenBuffer::drawString(24, 27, "through a series of obstacles. You wll get");
	ScreenBuffer::drawString(24, 28, "point when passed an you obstacle. Game over");
	ScreenBuffer::drawString(24, 29, "when your character hit the obstacles or the");
	ScreenBuffer::drawString(24, 30, "game border.");
	ScreenBuffer::drawString(10, 35, "HOW TO PLAY:", FG_GREEN);
	ScreenBuffer::drawString(24, 40, "[ ]/[ ]: MOVE TO LEFT/RIGHT");
	ScreenBuffer::draw(25, 40, 27); //draw left arrow
	ScreenBuffer::draw(29, 40, 26); //draw right arrow
	ScreenBuffer::drawString(24, 44, "[SPACE]: TO JUMP");
	ScreenBuffer::drawString(10, 49, "TIPS:", FG_GREEN);
	ScreenBuffer::drawString(24, 54, "JUMP TO REDUCE THE CHARACTER'S FALLING SPEED");
	ScreenBuffer::drawString(24, 58, "GO THROUGH THE GAP WITHOUT JUMPING TO GET MORE");
	ScreenBuffer::drawString(24, 59, "POINTS");
	ScreenBuffer::drawString(24, 63, "YOU CAN CHANGE YOUR CHARACTER AND OBSTACLES");
	ScreenBuffer::drawString(24, 64, "APPEARANCE AT THE OPTIONS MENU");
	ScreenBuffer::drawString(52, 75, "[ENTER]: RETURN TO MENU", FG_DARK_CYAN);
	ScreenBuffer::drawToConsole();
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) break;
	}
}

int Menu() {
	AudioPlayer::PlayBackgroundMusic("Bound-Console-Game/GameData/Music/Menu.wav");
	DrawMenu();
	int Key;
	while (true)
	{
		while (_kbhit()) _getch(); //clear the input buffer
		Key = _getch();
		switch (Key)
		{
		case KEY_E + 32:
		case KEY_E:
			return 0;

		case KEY_P + 32:
		case KEY_P:
			AudioPlayer::PauseMusic();
			return 1;

		case KEY_O + 32:
		case KEY_O:
			Options();
			DrawMenu();
			break;

		case KEY_C + 32:
		case KEY_C:
			Credits();
			DrawMenu();
			break;

		case KEY_H + 32:
		case KEY_H:
			Help();
			DrawMenu();
			break;
		}
	}
}



// -------- FUNCTIONS FOR PLAYING STATE --------

/*
	Getting ready to start the game again. Reset/ Init ball XY or score,...
*/
void ResetGame() {
	// Initialize global variables
	gameOver = false;
	ball.x = 1 / 2.0f * GAME_WIDTH;
	ball.y = 1 / 3.0f * GAME_HEIGHT;
	score = 0;
	ball.v = 0;
	ball.passed = 0;
	Obstacleupdate = NUMBER_OF_WALLS;
	CaseUpdate = 1;
	//Section Heigth is just to know where to put the Obstacle in the first place. So if you change the Game Height, it won't appeared in weird position.
	SectionHeigth = ((float)GAME_HEIGHT / NUMBER_OF_WALLS + 1) + 5;
	srand(time(NULL));
	for (int i = 0; i < NUMBER_OF_WALLS - 1; i++)
	{
		Obstacle[i].passed = 0;
		Obstacle[i].spaceX = rand() % (GAME_WIDTH - SPACE_WIDTH);
		Obstacle[i].spaceY = (i + NUMBER_OF_WALLS - 1)*SectionHeigth;
		while (Obstacle[i].spaceX < 3)
		{
			Obstacle[i].spaceX = rand() % (GAME_WIDTH - SPACE_WIDTH);
		}
	}
	Obstacle[NUMBER_OF_WALLS - 1].spaceY = -15;
	Obstacle[NUMBER_OF_WALLS - 1].passed = 1;
	}

// === HANDLE PLAY INPUT ===
void GameHandleInput() {
	leftPressed=0;
	rightPressed=0;

	next_space=GetAsyncKeyState(VK_SPACE)&0x8000;
	if(next_space&&(!pre_space))
		spacePressed=1;	
	if(next_space&&pre_space)
		spacePressed=0;	
	if(!next_space)
		spacePressed=0;
	pre_space=next_space;

	if(GetAsyncKeyState(VK_LEFT)&0x8000)
		leftPressed=1;

	if(GetAsyncKeyState(VK_RIGHT)&0x8000)
		rightPressed=1;
}

// === PLAY LOGIC ===
void ObstacleLogic(float fElapsedTime);
void Collision();
void DrawLogic();
void DrawScore(int temp,int x,int y);

void GameLogic(float elapsedTime) {
	controlBall(elapsedTime,ball);
	ObstacleLogic(elapsedTime);
	DrawLogic();
	Collision();
}

void UpdateObstacle(float elapsedTime)
{
	if (Obstacleupdate == NUMBER_OF_WALLS || Obstacle[Obstacleupdate].spaceY < ball.y) //this is to make the Obstacle choosed won't be replace if we have go over it.
	{
		for (int i = 0; i < NUMBER_OF_WALLS; i++) //this is to take the name of the Obstacle that is lower than our Ball
		{
			if (Obstacle[i].spaceY > ball.y)
			{
				Obstacleupdate = i;
				break;
			}
		}
	}
	if (Obstacle[Obstacleupdate].spaceX + SPACE_WIDTH  >= GAME_WIDTH) CaseUpdate = 2; 
	if (Obstacle[Obstacleupdate].spaceX <= 0) CaseUpdate = 1;
	
		switch (CaseUpdate)
		{
		case 1: Obstacle[Obstacleupdate].spaceX += elapsedTime*14.0f; break;
		case 2: Obstacle[Obstacleupdate].spaceX -= elapsedTime*14.0f; break;
		}
	
}

void ObstacleLogic(float fElapsedTime)
{
	for (int i = 0; i < NUMBER_OF_WALLS - 1; i++)
	{
		Obstacle[i].spaceY -= 8.0f*fElapsedTime; //this is to keep the Obstacle (Wall) going up
		
		
		if (Obstacle[i].spaceY <= 0) // this to make the transition of wall which touch the top border to the bottom border smother.
		{
			Obstacle[i].passed = 0; //this is to know that this wall is not passed.(after recycled)
			Obstacle[NUMBER_OF_WALLS - 1].spaceX = Obstacle[i].spaceX;
			Obstacle[NUMBER_OF_WALLS - 1].spaceY = Obstacle[i].spaceY;
			Obstacle[i].spaceX = 1 + rand() % (GAME_WIDTH - SPACE_WIDTH - 1);
			Obstacle[i].spaceY = GAME_HEIGHT - 1 + Obstacle[NUMBER_OF_WALLS - 1].spaceY;
		}
		Obstacle[NUMBER_OF_WALLS - 1].spaceY -= 6.0f*fElapsedTime;
		
	}
	if (score >= 1) UpdateObstacle(fElapsedTime); //Set any score you want, i put 1 to test easily.
}

void DrawLogic()
{
	if (ball.y > BALL_LIMIT*1.0f)
	{
		for (int i = 0; i < NUMBER_OF_WALLS; i++)


			Obstacle[i].spaceY -= ball.y - BALL_LIMIT*1.0f;
		ball.y = BALL_LIMIT*1.0f;
	}
}

void Collision()
{
	int xball, yball, xspace, yspace;
	for (int i = 0; i < NUMBER_OF_WALLS; i++)
	{
		xball = ball.x + 0.5f;
		yball = ball.y + 0.5f;
		xspace = Obstacle[i].spaceX + 0.5f;
		yspace = Obstacle[i].spaceY + 0.5f;
		if (yspace - yball <= BALL_RADIUS && yspace - yball > -WALL_HEIGHT - BALL_RADIUS)
		{
			if (!(xball - xspace > BALL_RADIUS - 1 && xball - xspace < SPACE_WIDTH - BALL_RADIUS))
				gameOver = true;
		}
	}

	if (ball.y < 1) gameOver = 1;

	for (int i = 0; i < NUMBER_OF_WALLS; i++)
	{
		if (ball.y >= Obstacle[i].spaceY + WALL_HEIGHT && Obstacle[i].passed == 0)
		{
			AudioPlayer::PlayEffect("Bound-Console-Game/GameData/Music/Point.wav");
			score+=pow(2.0, ball.passed);
			Obstacle[i].passed = 1;
			ball.passed++;
		}
	}
}

// === PLAY DRAW ===
void drawHUD() {

	ScreenBuffer::drawRect(0, 0, GAME_WIDTH + 1, GAME_HEIGHT + 1, 219, 15);
	//
	
	ScreenBuffer::drawLine(52, 1, 77, 1, 4, Color::FG_GREEN);
	ScreenBuffer::drawString(61, 1, " SCORE ", Color::FG_GREEN);
	//ScreenBuffer::drawString(52, 12, "BONUS +2", 14);
	ScreenBuffer::drawString(52, 15, "BEST: 123", 10);
	//
	ScreenBuffer::drawLine(52, 19, 77, 19, 4, Color::FG_CYAN);
	ScreenBuffer::drawString(60, 19, " CONTROL ", Color::FG_CYAN);
	ScreenBuffer::drawString(52, 23, "[SPACE] TO JUMP", 11);
	ScreenBuffer::drawString(52, 26, "[ ]/[ ] TO MOVE LEFT/RIGHT", 11);
	ScreenBuffer::draw(53, 26, 27, 11); ScreenBuffer::draw(57, 26, 26, 11);
	//
	ScreenBuffer::drawLine(52, 30, 77, 30, 4, Color::FG_YELLOW);
	ScreenBuffer::drawString(62, 30, " TIPS ", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 34, "GO THROUGH THE GAP WITHOUT", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 36, "JUMPING TO GET MORE POINT", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 39, "JUMP TO REDUCE THE", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 41, "CHARACTER'S FALLING SPEED", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 44, "YOU CAN CHANGE YOUR", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 46, "CHARACTER'S SKIN AT", Color::FG_YELLOW);
	ScreenBuffer::drawString(52, 48, "MENU/OPTIONS", Color::FG_DARK_YELLOW);
	//
	ScreenBuffer::drawString(59, 78, "BOUND GAME VER. 1.0", Color::FG_RED);
	//TODO: Draw score

	UpdateAndShowScore();
}

void drawStage(int originX, int originY, int maxX, int maxY) {

	ScreenBuffer::fillRect(originX + ball.x-BALL_RADIUS + 0.5f, originY + ball.y-BALL_RADIUS + 0.5f, originX + ball.x + BALL_RADIUS + 0.5f, originY + ball.y + BALL_RADIUS + 0.5f,' ', Color::BG_RED);
	for (int i = 0; i < NUMBER_OF_WALLS; i++) {
		int drawSpaceX = Obstacle[i].spaceX + 0.5f;
		int drawSpaceY = Obstacle[i].spaceY + 0.5f;
		ScreenBuffer::fillRect(originX, originY + drawSpaceY, originX + drawSpaceX - 1, originY + drawSpaceY + WALL_HEIGHT - 1, ' ', Color::BG_DARK_GREY);
		ScreenBuffer::fillRect(originX + drawSpaceX + SPACE_WIDTH, originY + drawSpaceY, maxX, originY + drawSpaceY + WALL_HEIGHT - 1, ' ', Color::BG_DARK_GREY);
	}
}

void GameDraw() {
	// TODO: Add a padding variables. Too many mysterious numbers

	//Clear the whole screen
	ScreenBuffer::fillBuffer(' ');

	drawStage(1, 1, GAME_WIDTH, GAME_HEIGHT);

	drawHUD();
}

/*
	The update loop of our game
*/

void onGameUpdate(float elapsedTime) {
	GameHandleInput();
	GameLogic(elapsedTime);
	GameDraw();
}

void DrawScore(int temp,int x,int y)
{
	Sprite numb[10];
	LoadSprite(numb[0], "Bound-Console-Game/GameData/Numbers/0.dat");
	LoadSprite(numb[1], "Bound-Console-Game/GameData/Numbers/1.dat");
	LoadSprite(numb[2], "Bound-Console-Game/GameData/Numbers/2.dat");
	LoadSprite(numb[3], "Bound-Console-Game/GameData/Numbers/3.dat");
	LoadSprite(numb[4], "Bound-Console-Game/GameData/Numbers/4.dat");
	LoadSprite(numb[5], "Bound-Console-Game/GameData/Numbers/5.dat");
	LoadSprite(numb[6], "Bound-Console-Game/GameData/Numbers/6.dat");
	LoadSprite(numb[7], "Bound-Console-Game/GameData/Numbers/7.dat");
	LoadSprite(numb[8], "Bound-Console-Game/GameData/Numbers/8.dat");
	LoadSprite(numb[9], "Bound-Console-Game/GameData/Numbers/9.dat");

	switch (temp)
	{
	case 0:
		DrawSprite(numb[0], x, y);
		FreeSprite(numb[0]);
		break;
	case 1:
		DrawSprite(numb[1], x, y);
		FreeSprite(numb[1]);
		break;
	case 2:
		DrawSprite(numb[2], x, y);
		FreeSprite(numb[2]);
		break;
	case 3:
		DrawSprite(numb[3], x, y);
		FreeSprite(numb[3]);
		break;
	case 4:
		DrawSprite(numb[4], x, y);
		FreeSprite(numb[4]);
		break;
	case 5:
		DrawSprite(numb[5], x, y);
		FreeSprite(numb[5]);
		break;
	case 6:
		DrawSprite(numb[6], x, y);
		FreeSprite(numb[6]);
		break;
	case 7:
		DrawSprite(numb[7], x, y);
		FreeSprite(numb[7]);
		break;
	case 8:
		DrawSprite(numb[8], x, y);
		FreeSprite(numb[8]);
		break;
	case 9:
		DrawSprite(numb[9], x, y);
		FreeSprite(numb[9]);
		break;
	}
}
void UpdateAndShowScore()
{
	int temp_score;
	int t;
	int x = 78;
	temp_score = score;
	if (temp_score == 0)
		DrawScore(temp_score, x - 3, 5);
	else
	{
		while (temp_score > 0)
		{
			t = temp_score % 10;
			DrawScore(t, x - 3, 5);
			x -= 4;
			temp_score /= 10;
		}
	}
}

int ReadBestScore()
{
	std::ifstream infile("bestScore.txt");
	if (!infile.is_open())
	{
		std::ofstream out("bestScore.txt");
		out << 0 << std::endl;
		out.close();
		std::ifstream infile("bestScore.txt");
		infile >> bestScore;
	}
	if (infile.is_open())
		infile >> bestScore;
	infile.close();
	return bestScore;
}

void SaveBestScore()
{
	if (score > bestScore)
	{
		bestScore = score;
		std::ofstream outfile("bestScore.txt");
		if (outfile.is_open())
			outfile << bestScore;
		outfile.close();
	}
}
void GameOver()
{
	SaveBestScore();
	while (gameOver)
	{
		int Temp;
		std::string StrScore = "";
		Temp = score;
		if (score == 0)
			StrScore = '0';
		else
		{
			while (Temp > 0)
			{
				StrScore = (char)(Temp % 10 + 48) + StrScore;
				Temp /= 10;
			}
		}
		
		std::string StrBest = "";
		Temp = bestScore;
		if (bestScore == 0)
			StrBest = '0';
		else
		{
			while (Temp > 0)
			{
				StrBest = (char)(Temp % 10 + 48) + StrBest;
				Temp /= 10;
			}
		}
		
		Sprite Game_over;
		LoadSprite(Game_over, "Bound-Console-Game/GameData/GameOver/GameOver.dat");
		DrawSprite(Game_over, 3, 32);
		FreeSprite(Game_over);
		ScreenBuffer::drawString(19, 40, "SCORE", 224);
		ScreenBuffer::drawString(26, 40, StrScore, 224);
		ScreenBuffer::drawString(20, 42, "BEST", 224);
		ScreenBuffer::drawString(26, 42, StrBest, 224);
		ScreenBuffer::drawLine(4, 44, 45, 44, 223, 14); //@TranTrung: you should use Color or else I won't know what color it is. Ask me if you need to know which color do these value represent for
		ScreenBuffer::drawString(4, 45, "         PRESS ENTER TO CONTINUE          ", 14);
		ScreenBuffer::drawToConsole();
		while (_kbhit()) _getch(); //clear the input buffer
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) gameOver = 0;
	}
}

// -------- end --------