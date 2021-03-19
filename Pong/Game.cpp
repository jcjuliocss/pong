// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;//sera usado para setar a altura de alguns objetos
const float paddleH = 50.0f;//tamanho da raquete

Game::Game()
:mWindow(nullptr)//para criar uma janela
,mRenderer(nullptr)//para fins de renderização na tela
,mTicksCount(0)//para guardar o tempo decorrido no jogo
,mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
,mPaddleLeftDir(0) // direcao da raquete da esquerda
,mPaddleRightDir(0) // direcao da raquete da direita
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Hello Pong!", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	mPaddleLeftPos.x = 10.0f;//posição inicial da raquete da esquerda no eixo x
	mPaddleLeftPos.y = 768.0f/2.0f;//posição inicial da raquete da esquerda no eixo y

	mPaddleRightPos.x = 1000.0f;//posição inicial da raquete da direita no eixo x
	mPaddleRightPos.y = 768.0f / 2.0f;//posição inicial da raquete da direita no eixo y

	mBallPos.x = 1024.0f/2.0f;//posição da bola eixo x
	mBallPos.y = 768.0f/2.0f;//posição da bola eixo y
	mBallVel.x = 150.0f;//-200.0f;//velocidade de movimentação da bola no eixo x
	mBallVel.y = 150.0f;//200.0f;//velocidade de movimentação da bola no eixo y

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput(){
	SDL_Event event;//evento, inputs do jogador são armazenados aqui
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard - podemos buscar por alguma tecla específica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE]){
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys - atualize a direção da raquete com base na entrada do jogador
	// W -> move a raquete para cima, S -> move a raquete para baixo
	mPaddleLeftDir = 0;
	if (state[SDL_SCANCODE_W]){
		mPaddleLeftDir -= 1;
	}

	if (state[SDL_SCANCODE_S]){
		mPaddleLeftDir += 1;
	}

	mPaddleRightDir = 0;
	if (state[SDL_SCANCODE_UP]) {
		mPaddleRightDir -= 1;
	}

	if (state[SDL_SCANCODE_DOWN]) {
		mPaddleRightDir += 1;
	} 

	if (state[SDL_SCANCODE_2]) {
		players.player2 = true;
		score.player1 = 0;
		score.player2 = 0;
	}
}

void Game::UpdateGame(){
	// Espere que 16ms tenham passado desde o último frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// Delta time é a diferença em ticks do último frame
	// (convertido pra segundos) - calcula o delta time para atualização do jogo
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// "Clamp" (lima/limita) valor máximo de delta time
	if (deltaTime > 0.05f){
		deltaTime = 0.05f;
	}

	// atualize a contagem de ticks par ao próximo frame
	mTicksCount = SDL_GetTicks();
	
	// atualiza a posição da raquete
	if (mPaddleLeftDir != 0){
		mPaddleLeftPos.y += mPaddleLeftDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddleLeftPos.y < (paddleH/2.0f + thickness)){
			mPaddleLeftPos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddleLeftPos.y > (768.0f - paddleH/2.0f - thickness)){
			mPaddleLeftPos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	if (players.player2) {
		if (mPaddleRightDir != 0) {
			mPaddleRightPos.y += mPaddleRightDir * 300.0f * deltaTime;
			// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
			if (mPaddleRightPos.y < (paddleH / 2.0f + thickness)) {
				mPaddleRightPos.y = paddleH / 2.0f + thickness;
			}
			else if (mPaddleRightPos.y > (768.0f - paddleH / 2.0f - thickness)) {
				mPaddleRightPos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}
	}

	// Atualize (negative) a posição da bola se ela colidir com a parede de baixo
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
		mBallVel.y *= -1;
	}
	else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f) { // parede de cima
		mBallVel.y *= -1;
	}
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f) { // parede da direita
		if (!players.player2) {
			mBallVel.x *= -1.0f;
		}else {
			score.player1++;
			// se o jogador 1 fez ponto, a bola deve se mover na direcao do jogador 1 ate ele acertar
			mBallVel.x = 150.0f;
			mBallVel.y = 150.0f;
			mBallPos.x = 1024.0f / 2.0f;//posição da bola eixo x
			mBallPos.y = 768.0f / 2.0f;//posição da bola eixo y
		}
	}else if (mBallPos.x <= 0.0f) {
		if (!players.player2) {
			mIsRunning = false;
		}
		else {
			score.player2++;
			// se o jogador 2 fez ponto, a bola deve se mover na direcao do jogador 1 ate ele acertar
			mBallVel.x = -150.0f;
			mBallVel.y = -150.0f;
			mBallPos.x = 1024.0f / 2.0f;//posição da bola eixo x
			mBallPos.y = 768.0f / 2.0f;//posição da bola eixo y
		}
	}
	
	// atualiza a posição da bola com base na sua velocidade
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// atualiza a posição da bola se ela colidiu com a raquete
	float diff = mPaddleLeftPos.y - mBallPos.y;
	//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
	//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
	//verifica se a bola está na area da raquete e na mesma posição no eixo x
	diff = (diff > 0.0f) ? diff : -diff;

	if (
		// A diferença no eixo y y-difference is small enough
		diff <= paddleH / 2.0f &&
		// Estamos na posicao x correta
		mBallPos.x <= paddleH / 2.0f)
	{
		mBallVel.x *= -1.0f;
		// depois de bater na raquete a velocidade da bolinha esta positiva, entao basta somar velocidade
		mBallVel.x += 25.0f;
		mBallVel.y += 25.0f;
	}

	if (players.player2) {
		float diff2 = mPaddleRightPos.y - mBallPos.y;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;

		if (diff2 <= paddleH / 2.0f && mBallPos.x >= 999.0f) {
			mBallVel.x *= -1.0f;
			mBallVel.x -= 25.0f;
			mBallVel.y -= 25.0f;
		}
	}
}

//Desenhando a tela do jogo
void Game::GenerateOutput()
{
	// Setamos a cor de fundo par preto
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		0,	// B
		0		// A
	);
	
	// limpa o back buffer
	SDL_RenderClear(mRenderer);

	// Desenha as paredes - mudamos a cor de mRenderer para o desenho dos retangulos que formaram as paredes
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
			0,			// Top left x
			0,			// Top left y
			1024,		// Width
			thickness	// Height
	};

	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo
	//desenhamos as outras paredes apenas mudando as coordenadas de wall
	// parede de baixo
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	// parede da direita
	if (!players.player2) {
		wall.x = 1024 - thickness;
		wall.y = 0;
		wall.w = thickness;
		wall.h = 1024;
		SDL_RenderFillRect(mRenderer, &wall);
	}
	//como as posições da raquete e da bola serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle{
		static_cast<int>(mPaddleLeftPos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddleLeftPos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	if (players.player2) {
		SDL_Rect paddle2{
			static_cast<int>(mPaddleRightPos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
			static_cast<int>(mPaddleRightPos.y - paddleH / 2),
			thickness,
			static_cast<int>(paddleH)
		};
		SDL_RenderFillRect(mRenderer, &paddle2);
	}

	//desenhando a bola - usando mBallPos que é uma struc de coordenadas definida como membro em Game.h
	
	//mudar a cor do renderizador para a bola
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	
	if (!players.player2) {
		DrawScore(score.player1, 1024 / 2 - 50);
	}else {
		DrawScore(score.player1, 256);
		DrawScore(score.player2, 718);
	}
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::DrawScore(int score, int pos_x) {
	int ttfResult = TTF_Init();

	TTF_Font* font = TTF_OpenFont("arial.ttf", 18); // define fonte do score
	SDL_Color color = { 255, 255, 255 }; // define a cor do score

	// cria a superficie e renderiza o texto
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, to_string(score).c_str(), color);

	// log de possiveis erros
	if (scoreSurface == NULL){
		SDL_Log("Failed to create surface: %s", SDL_GetError());
	}
	else{
		int width = 100;
		int heigth = 100;
		// cria a textura com o score a partir dos pixels da superficie
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(mRenderer, scoreSurface);
		if (scoreTexture == NULL){
			SDL_Log("Failed to create texture: %s", SDL_GetError());
		}
		else{
			//passa fonte, dimensoes(WxH) e o score para servir de texto e sua posicao na tela
			TTF_SizeText(font, to_string(score).c_str(), &width, &heigth);
		}
		// depois de definida a textura, pode descartar a superficie
		SDL_FreeSurface(scoreSurface);
		// cria o retangulo para exibir o score
		SDL_Rect scoreRect{
			static_cast<int>(pos_x), // exibe score no meio da tela horizontalmente
			50, // coloca o score no topo da tela
			width, // largura do tamanho do score
			heigth // altura do tamanho do score
		};
		// renderiza a textura no retangulo
		SDL_RenderCopy(mRenderer, scoreTexture, NULL, &scoreRect);
		// agora que já renderizou a textura, destruimos o texto ate a proxima atualizacao
		SDL_DestroyTexture(scoreTexture);
	}
}

//Para encerrar o jogo
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
	TTF_Quit();
}
