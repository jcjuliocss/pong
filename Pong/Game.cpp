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
,mPaddleTopDir(0) // direcao da raquete de cima
,mPaddleBottomDir(0) // direcao da raquete de baixo
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

	mPaddleTopPos.x = 1024.0f / 2.0f;//posição inicial da raquete de cima no eixo x
	mPaddleTopPos.y = 10.0f;//posição inicial da raquete de cime no eixo y

	mPaddleBottomPos.x = 1024.0f / 2.0f;//posição inicial da raquete de baixo no eixo x
	mPaddleBottomPos.y = 768.0f;//posição inicial da raquete de baixo eixo y

	mBallPos.x = 1024.0f/2.0f;//posição da bola eixo x
	mBallPos.y = 768.0f/2.0f;//posição da bola eixo y
	mBallVel.x = 0;//-200.0f;//velocidade de movimentação da bola no eixo x
	mBallVel.y = 0;//200.0f;//velocidade de movimentação da bola no eixo y

	//segunda bolinha
	mBallPos2.x = 1024.0f / 2.0f; //posição da bola 2 eixo x
	mBallPos2.y = 768.0f / 2.0f; //posição da bola 2 eixo y
	mBallVel2.x = 0;//200.0f; //velocidade de movimentação da bola 2 no eixo x
	mBallVel2.y = 0;//-200.0f; //velocidade de movimentação da bola 2 no eixo y
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

	mPaddleTopDir = 0;
	if (state[SDL_SCANCODE_A]) {
		mPaddleTopDir -= 1;
	}

	if (state[SDL_SCANCODE_D]) {
		mPaddleTopDir += 1;
	}

	mPaddleRightDir = 0;
	if (state[SDL_SCANCODE_UP]) {
		mPaddleRightDir -= 1;
	}

	if (state[SDL_SCANCODE_DOWN]) {
		mPaddleRightDir += 1;
	}

	mPaddleBottomDir = 0;
	if (state[SDL_SCANCODE_LEFT]) {
		mPaddleBottomDir -= 1;
	}

	if (state[SDL_SCANCODE_RIGHT]) {
		mPaddleBottomDir += 1;
	}
}

void Game::UpdateGame()
{
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
		if (mPaddleLeftPos.y < (paddleH/2.0f + 25)){
			mPaddleLeftPos.y = paddleH/2.0f + 25;
		}
		else if (mPaddleLeftPos.y > (768.0f - paddleH/2.0f - 25)){
			mPaddleLeftPos.y = 768.0f - paddleH/2.0f - 25;
		}
	}

	if (mPaddleRightDir != 0) {
		mPaddleRightPos.y += mPaddleRightDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddleRightPos.y < (paddleH / 2.0f + 26)) {
			mPaddleRightPos.y = paddleH / 2.0f + 26;
		}
		else if (mPaddleRightPos.y > (768.0f - paddleH / 2.0f - 26)) {
			mPaddleRightPos.y = 768.0f - paddleH / 2.0f - 26;
		}
	}

	if (mPaddleTopDir != 0) {
		mPaddleTopPos.x += mPaddleTopDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddleTopPos.x < (paddleH / 2.0f)) {
			mPaddleTopPos.x = paddleH / 2.0f;
		}
		else if (mPaddleTopPos.x > (1024.0f - paddleH / 2.0f - paddleH)) {
			mPaddleTopPos.x = 1024.0f - paddleH / 2.0f - paddleH;
		}
	}

	if (mPaddleBottomDir != 0) {
		mPaddleBottomPos.x += mPaddleBottomDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddleBottomPos.x < (paddleH / 2.0f)) {
			mPaddleBottomPos.x = paddleH / 2.0f;
		}
		else if (mPaddleBottomPos.x > (1024.0f - paddleH / 2.0f - paddleH)) {
			mPaddleBottomPos.x = 1024.0f - paddleH / 2.0f - paddleH;
		}
	}
	
	// atualiza a posição da bola com base na sua velocidade
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	mBallPos2.x += mBallVel2.x * deltaTime;
	mBallPos2.y += mBallVel2.y * deltaTime;
	
	// atualiza a posição da bola se ela colidiu com a raquete
	float diff = mPaddleLeftPos.y - mBallPos.y;
	float diff2 = mPaddleLeftPos.y - mBallPos2.y;
	//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
	//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
	//verifica se a bola está na area da raquete e na mesma posição no eixo x
	diff = (diff > 0.0f) ? diff : -diff;
	diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
	if (
		// A diferença no eixo y y-difference is small enough
		diff <= paddleH / 2.0f &&
		// Estamos na posicao x correta
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// A bolinha está se movendo para a esquerda
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
		// depois de bater na raquete a velocidade da bolinha esta positiva, entao basta somar velocidade
		mBallVel.x += 50.0f;
		mBallVel.y += 50.0f;

		score.player1++;
	}else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f){
		mBallVel.x *= -1.0f;
	}

	if (
		// A diferença no eixo y y-difference is small enough
		diff2 <= paddleH / 2.0f &&
		// Estamos na posicao x correta
		mBallPos2.x <= 25.0f && mBallPos2.x >= 20.0f &&
		// A bolinha está se movendo para a esquerda
		mBallVel2.x < 0.0f)
	{
		mBallVel2.x *= -1.0f;
		// depois de bater na raquete a velocidade da bolinha esta positiva, entao basta somar velocidade
		mBallVel2.x += 50.0f;
		mBallVel2.y += 50.0f;

		score.player1++;
	}
	else if (mBallPos2.x <= 0.0f)
	{
		mIsRunning = false;
		score.player2++;
	}
	/*else if (mBallPos2.x >= (1024.0f - thickness) && mBallVel2.x > 0.0f) {
		mBallVel2.x *= -1.0f;
	}
	
	// Atualize (negative) a posição da bola se ela colidir com a parede de baixo
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f){
		mBallVel.y *= -1;
	}else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f){
		mBallVel.y *= -1;
	}

	if (mBallPos2.y <= thickness && mBallVel2.y < 0.0f){
		mBallVel2.y *= -1;
	}else if (mBallPos2.y >= (768 - thickness) && mBallVel2.y > 0.0f) {
		mBallVel2.y *= -1;
	}*/
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
	//SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	//SDL_Rect wall{
	//	0,			// Top left x
	//	0,			// Top left y
	//	1024,		// Width
	//	thickness	// Height
	//};
	//SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo
	
	//desenhamos as outras paredes apenas mudando as coordenadas de wall
	// parede de baixo
	//wall.y = 768 - thickness;
	//SDL_RenderFillRect(mRenderer, &wall);
	
	// parede da direita
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);
	
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

	SDL_Rect paddle2{
		static_cast<int>(mPaddleTopPos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddleTopPos.y),
		paddleH, // largura da requete
		static_cast<int>(thickness) // altura da raquete
	};
	SDL_RenderFillRect(mRenderer, &paddle2);

	SDL_Rect paddle3{
		static_cast<int>(mPaddleRightPos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddleRightPos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle3);

	SDL_Rect paddle4{
		static_cast<int>(mPaddleBottomPos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddleBottomPos.y - paddleH / 2),
		paddleH,
		static_cast<int>(thickness)
	};
	SDL_RenderFillRect(mRenderer, &paddle4);
	

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
	
	SDL_Rect ball2{	
		static_cast<int>(mBallPos2.x - thickness/2),
		static_cast<int>(mBallPos2.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball2);

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
	
	DrawScore();
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::DrawScore() {
	int ttfResult = TTF_Init();

	TTF_Font* font = TTF_OpenFont("arial.ttf", 18); // define fonte do score
	SDL_Color color = { 255, 255, 255 }; // define a cor do score

	// cria a superficie e renderiza o texto
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, to_string(score.player1).c_str(), color);

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
			TTF_SizeText(font, to_string(score.player1).c_str(), &width, &heigth);
		}
		// depois de definida a textura, pode descartar a superficie
		SDL_FreeSurface(scoreSurface);
		// cria o retangulo para exibir o score
		SDL_Rect scoreRect{
			static_cast<int>(1024 / 2 - width / 2), // exibe score no meio da tela horizontalmente
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
