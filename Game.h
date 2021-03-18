// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include <string>

using namespace std;

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};

struct Score {
	int player1;
	int player2;
};

// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void DrawScore();

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	
	// Pong specific
	// direcao da raquete da esquerda
	int mPaddleLeftDir;
	// posicao da raquete da esquerda
	Vector2 mPaddleLeftPos;

	// direcao da raquete da direita
	int mPaddleRightDir;
	// posicao da raquete da direita
	Vector2 mPaddleRightPos;

	// direcao da raquete de cima
	int mPaddleTopDir;
	// posicao da raquete de cima
	Vector2 mPaddleTopPos;

	// direcao da raquete de baixo
	int mPaddleBottomDir;
	// posicao da raquete de baixo
	Vector2 mPaddleBottomPos;

	// Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;

	Vector2 mBallPos2;
	Vector2 mBallVel2;

	Score score;
};
