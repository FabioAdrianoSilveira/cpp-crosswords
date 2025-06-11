// Adicionar headers
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

// Adicionar arquivos de struct
#include "Game.cpp"

// definir parametros de inicializacao
#define SDL_FLAGS SDL_INIT_VIDEO

// definir parametros da janela
#define WINDOW_TITLE "Cruzadinhas++"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


// Declarar funcoes
bool game_init_sdl(Game *g);
void game_free(Game *g);
void game_run(Game *g);

// inicializar o jogo com verificações
bool game_init_sdl(Game *g)
{
	if (!SDL_Init(SDL_FLAGS))
	{
		std::cout << "Error initializing SDL3: " << SDL_GetError() << std::endl;
		return false;
	}

	// Inicializar janela
	g->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (g->window == NULL)
	{
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		return false;
	}

	// Criar renderer
        g->renderer = SDL_CreateRenderer(g->window, NULL);
        if (g->renderer == NULL)
        {
                std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
        }

	return true;
}

// Fechar o jogo e destruir componentes
void game_free(Game *g)
{
	if (g->renderer)
        {
                SDL_DestroyRenderer(g->renderer);
                g->renderer = NULL;
        }

	if(g->window)
	{
		SDL_DestroyWindow(g->window);
		g->window = NULL;
	}

	SDL_Quit();
}

void game_run(Game *g)
{
	SDL_SetRenderDrawColor(g->renderer, 128, 0, 128, 255);

	SDL_RenderClear(g->renderer);
	SDL_RenderPresent(g->renderer);

	SDL_Delay(5000);
}

int main(void)
{
	bool exit_status = EXIT_FAILURE;

	Game game = {0};

	if (game_init_sdl(&game))
	{
		game_run(&game);

		exit_status = EXIT_SUCCESS;
	}

	game_free(&game);

	return exit_status;
}
