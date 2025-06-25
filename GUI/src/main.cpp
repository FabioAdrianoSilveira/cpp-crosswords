/**
 * CRUZADINHAS++
 * INSTITUTO FEDERAL DE SÃO PAULO
 * 
 * ALGORITMOS E LOGICA DE PROGRAMACAO 2
 * 
 * AUTHOR: Harper Moreira Mascarenhas
 * DATE: 2025-06-18
*/

// INCLUIR BIBLIOTECAS NECESSARIAS
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// INCLUIR ARQUIVOS PRODUZIDOS PELO SERVIDOR
#include "../../server/exports/global.h"

// definir parametros da janela e fonte
#define WINDOW_TITLE "Cruzadinhas++"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define TEXT_SIZE 23

// struct para parametros do jogo
struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
	SDL_Texture *cell_selected, *cell;
	TTF_Font *text_font;
	SDL_Color text_color;
	SDL_Rect text_rect;
	SDL_Texture *text_image;
	SDL_Texture *char_image[8][8];
};

// struct para posicao da celula
struct SelectedCell {
	int i, j;
	SDL_Texture *text;
	std::string hint;
};

// struct com as informações carregadas do servidor
gameStruct loadedGame = loadStruct("../../server/files/board.txt", "../../server/files/tips.txt");

// declara funções do programa
void game_cleanup(struct Game *game, int exit_status, struct SelectedCell *selected_cell);
bool load_media(struct Game *game, struct SelectedCell *selected_cell);
bool sdl_initialize(struct Game *game);
bool render_text(struct Game *game, std::string text);
bool render_number(struct Game *game, SDL_Rect *rect, char c, int i, int j);
bool draw_canvas(struct Game *game, int canvas[8][8], struct SelectedCell *selected_cell, char cells[8][8]);
bool isWin(char cells[8][8]);

int main() {

	// matriz para receber valores inseridos pelo jogador
	char cells[8][8] =
	{
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0'}
	};

	// declarar o texto de início
	SelectedCell selected_cell = {
		.i = 0,
		.j = 0,
		.hint = "Boas vindas às Cruzadinhas++!\nClique em uma célula, leia a dica e digite a palavra em seu teclado!"
	};
	// declarar o formato e células do tabuleiro
	int canvas[8][8] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1}
	};

	// declarar variáveis para receber posição do mouse
	int mousepos_x, mousepos_y;

	// declarar variavel de leitura do texto e inicializar para verdadeiro
	bool drawing_text = true;

	// declarar contador do texto, importante para fazer o texto aparecer aos poucos
	int text_counter = 1;

	// declarar variável de vitória
	bool win = false;

	// definir struct do jogo
    struct Game game = {
        .window = NULL,
        .renderer = NULL,
		.text_font = NULL,
		.text_color = {255, 255, 255, 255},
		.text_image = NULL
    };

	// executar func de inicializacao e fazer testes
    if (sdl_initialize(&game)) {
        game_cleanup(&game, EXIT_FAILURE, &selected_cell);
    }

	// executar func de carregamento e fazer testes
	if (load_media(&game, &selected_cell)) {
        game_cleanup(&game, EXIT_FAILURE, &selected_cell);
    }

	// GAME LOOP
    while (true) {
		// estado de vitoria ou nao
		bool hasWin = false;

		// CONTROLE DE INPUTS DO JOGADOR
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
			// evento de fechar o programa
            case SDL_QUIT:
                game_cleanup(&game, EXIT_SUCCESS, &selected_cell);
                break;
			// evento de clique do mouse
			// responsável por detectar o clique e pegar a posição do mouse para definir em qual célula ele está
            case SDL_MOUSEBUTTONDOWN:
				drawing_text = true;
				
				if(isWin(cells))
				{
					SDL_GetMouseState( &mousepos_x, &mousepos_y);

					if (mousepos_x >= 100)
					{
						while( mousepos_x >= 10 )
						{
							mousepos_x = mousepos_x / 10;
						}
					}
					else
					{
						mousepos_x = 0;
					}
					if (mousepos_y >= 100)
					{
						while( mousepos_y >= 10 )
						{
							mousepos_y = mousepos_y / 10;
						}
					}
					else
					{
						mousepos_y = 0;
					}
					selected_cell.i = mousepos_x;
					selected_cell.j = mousepos_y;

					if (loadedGame.tips[selected_cell.j][selected_cell.i] != "0" && win == false)
					{
						selected_cell.hint = loadedGame.tips[selected_cell.j][selected_cell.i];
						text_counter = 1;
					}
				}
				else
				{
					selected_cell.i = 0;
					selected_cell.j = 0;
					selected_cell.hint = "Parabéns!\nVocê venceu!";
					if (!hasWin)
					{
						text_counter = 1;
					}
					hasWin = true;
				}
			default:
                break;
            }
			// Detectar input do teclado, ver cada possibilidade de tecla e definir ela na respectiva célula selecionada
			// Deve haver uma maneira melhor de fazer isso
			if(canvas[selected_cell.j][selected_cell.i] == 1)
			{
				const Uint8 *state = SDL_GetKeyboardState(NULL);
				if (state[SDL_SCANCODE_A])
				{
					cells[selected_cell.i][selected_cell.j] = 'a';
				}
				if (state[SDL_SCANCODE_B])
				{
					cells[selected_cell.i][selected_cell.j] = 'b';
				}
				if (state[SDL_SCANCODE_C])
				{
					cells[selected_cell.i][selected_cell.j] = 'c';
				}
				if (state[SDL_SCANCODE_D])
				{
					cells[selected_cell.i][selected_cell.j] = 'd';
				}
				if (state[SDL_SCANCODE_E])
				{
					cells[selected_cell.i][selected_cell.j] = 'e';
				}
				if (state[SDL_SCANCODE_F])
				{
					cells[selected_cell.i][selected_cell.j] = 'f';
				}
				if (state[SDL_SCANCODE_G])
				{
					cells[selected_cell.i][selected_cell.j] = 'g';
				}
				if (state[SDL_SCANCODE_H])
				{
					cells[selected_cell.i][selected_cell.j] = 'h';
				}
				if (state[SDL_SCANCODE_I])
				{
					cells[selected_cell.i][selected_cell.j] = 'i';
				}
				if (state[SDL_SCANCODE_J])
				{
					cells[selected_cell.i][selected_cell.j] = 'j';
				}
				if (state[SDL_SCANCODE_K])
				{
					cells[selected_cell.i][selected_cell.j] = 'k';
				}
				if (state[SDL_SCANCODE_L])
				{
					cells[selected_cell.i][selected_cell.j] = 'l';
				}
				if (state[SDL_SCANCODE_M])
				{
					cells[selected_cell.i][selected_cell.j] = 'm';
				}
				if (state[SDL_SCANCODE_N])
				{
					cells[selected_cell.i][selected_cell.j] = 'n';
				}
				if (state[SDL_SCANCODE_O])
				{
					cells[selected_cell.i][selected_cell.j] = 'o';
				}
				if (state[SDL_SCANCODE_P])
				{
					cells[selected_cell.i][selected_cell.j] = 'p';
					}
				if (state[SDL_SCANCODE_Q])
				{
					cells[selected_cell.i][selected_cell.j] = 'q';
				}
				if (state[SDL_SCANCODE_R])
				{
					cells[selected_cell.i][selected_cell.j] = 'r';
				}
				if (state[SDL_SCANCODE_S])
				{
					cells[selected_cell.i][selected_cell.j] = 's';
				}
				if (state[SDL_SCANCODE_T])
				{
					cells[selected_cell.i][selected_cell.j] = 't';
				}
				if (state[SDL_SCANCODE_U])
				{
					cells[selected_cell.i][selected_cell.j] = 'u';
				}
				if (state[SDL_SCANCODE_V])
				{
					cells[selected_cell.i][selected_cell.j] = 'v';
				}
				if (state[SDL_SCANCODE_W])
				{
					cells[selected_cell.i][selected_cell.j] = 'w';
				}
				if (state[SDL_SCANCODE_X])
				{
					cells[selected_cell.i][selected_cell.j] = 'x';
				}
				if (state[SDL_SCANCODE_Y])
				{
					cells[selected_cell.i][selected_cell.j] = 'y';
				}
				if (state[SDL_SCANCODE_Z])
				{
					cells[selected_cell.i][selected_cell.j] = 'z';
				}
			}

			//std::cout << "TECLA PRESSIONADA: " << cells[selected_cell.i][selected_cell.j] << std::endl;

        }
		// LIMPAR O RENDERIZADOR (limpar frame anterior)
        SDL_RenderClear(game.renderer);

		// DESENHAR O TABULEIRO
		draw_canvas(&game, canvas, &selected_cell, cells);

		// DESENHAR O TEXTO
		// Pega o comprimento da string e compara com a variável text_counter declarada previamente
		// text_counter é incrementado e subtraído inversamente da string original
		// assim, o texto é revelado aos poucos
		int stringlen = strlen(data(selected_cell.hint));
		if(drawing_text == true)
		{
			text_counter++;
			if( text_counter > stringlen)
			{
				drawing_text = false;
			}
		}
		render_text(&game, selected_cell.hint.substr(0, text_counter));
		SDL_RenderCopy(game.renderer, game.text_image, NULL, &game.text_rect);

		// apresenta o novo frame
        SDL_RenderPresent(game.renderer);

		// Espera 30 milésimos de segundo para repetir o loop
        SDL_Delay(30);
    }

	// quando o loop acabar, limpar e fechar o jogo
    game_cleanup(&game, EXIT_SUCCESS, &selected_cell);

    return 0;
}

/*
	Funcao para limpar o jogo e fechar sem deixar resíduos na memória
	Todas as texturas utilizadas são destruídas

	O Renderer e a Janela são destruidos
	Assim como todas as bibliotecas abertas
	TTF para textos, IMG para imagens e SDL para janelas e renderers
*/
void game_cleanup(struct Game *game, int exit_status, struct SelectedCell *selected_cell) {
	SDL_DestroyTexture(game->cell);
	SDL_DestroyTexture(selected_cell->text);

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 0; j++)
		{
			SDL_DestroyTexture(game->char_image[i][j]);
		}
	}

	SDL_DestroyTexture(game->text_image);
	TTF_CloseFont(game->text_font);

    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

	TTF_Quit();
	IMG_Quit();
    SDL_Quit();
    exit(exit_status);
}

/*
	Funcao para inicializar o SDL

	Ela inicia todos os modulos necessarios com verificacoes e output de erro
*/
bool sdl_initialize(struct Game *game) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return true;
    }

	if (TTF_Init()) {
        std::cout << "Error initializing SDL_ttf: " << IMG_GetError() << std::endl;
        return true;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, 0);
    if (!game->window) {
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return true;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if (!game->renderer) {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
        return true;
    }

    return false;
}

/*
	Funcao de carregamento

	Carrega cada textura necessaria para o jogo e a fonte
	Com verificacoes e outputs de erro
*/
bool load_media(struct Game *game, struct SelectedCell *selected_cell)
{

	game->cell = IMG_LoadTexture(game->renderer, "../src/assets/cell.bmp");
	selected_cell->text = IMG_LoadTexture(game->renderer, "../src/assets/cell_selected.bmp");

	if (!game->cell || !selected_cell->text)
	{
        std::cout << "Error creating Texture: " << IMG_GetError() << std::endl;
        return true;
    }

	game->text_font = TTF_OpenFont("../src/assets/fonts/Moodcake.ttf", TEXT_SIZE);
	if (!game->text_font)
	{
        std::cout << "Error creating Font: " << TTF_GetError() << std::endl;
        return true;
    }

	return false;
}

/*
	Funcao que renderiza o texto da dica

	Ela recebe a string da main(), transforma em uma textura e retorna
*/
bool render_text(struct Game *game, std::string text)
{
	SDL_Surface *surface =
        TTF_RenderUTF8_Blended_Wrapped(game->text_font, std::data(text), game->text_color, 800);
    if (!surface) {
        std::cout << "Error creating Surface: " << SDL_GetError() << std::endl;
        return true;
    }
    game->text_rect.w = surface->w;
    game->text_rect.h = surface->h;
    game->text_image = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    if (!game->text_image) {
        std::cout << "Error creating Texture: " << SDL_GetError() << std::endl;
        return true;
    }

	return false;
}

/*
	Funcao para renderizar letra

	Ela recebe um char e renderiza, com verificações e saída de erro
	Mesmos principios de render_text()
*/
bool render_number(struct Game *game, SDL_Rect *rect, char c, int i, int j)
{
	SDL_Rect trect;
	trect.y = rect->x + 33;
	trect.x = rect->y + 30;
	trect.h = TEXT_SIZE + 10;
	trect.w = TEXT_SIZE + 10;
	const SDL_Rect crect = trect;
	std::string cs(1, c);

	SDL_Surface*surface = TTF_RenderText_Solid(game->text_font, std::data(cs), game->text_color);
	if (!surface)
	{
		std::cout << "Error creating Surface: " << SDL_GetError() << std::endl;
		return true;
	}
    game->text_rect.w = surface->w;
    game->text_rect.h = surface->h;
    game->char_image[i][j] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    if (!game->text_image)
	{
    	std::cout << "Error creating Texture: " << SDL_GetError() << std::endl;
		return true;
	}
	SDL_RenderCopy(game->renderer, game->char_image[i][j], NULL, &crect);

	return false;
}

/*
	Funcao para desenhar o tabuleiro

	Ela passa pelos valores de canvas[8][8] verificando cada posição e colocando a textura de acordo
	Também é responsável por renderizar a textura da célula selecionada e o char da célula cas ela tenha algum
*/
bool draw_canvas(struct Game *game, int canvas[8][8], struct SelectedCell *selected_cell, char cells[8][8])
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 100;
	rect.h = 100;

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{

			if (canvas[i][j] == 1)
			{
				SDL_RenderCopy(game->renderer, game->cell, NULL, &rect);
			}

			if (cells[i][j] != '0')
			{
				render_number(game, &rect, cells[i][j], i, j);
			}

			rect.x = rect.x + 100;

		}
		rect.x = 0;
		rect.y = rect.y + 100;
	}
	if (canvas[selected_cell->j][selected_cell->i] == 1)
	{
		rect.x = selected_cell->i * 100;
		rect.y = selected_cell->j * 100;
		SDL_RenderCopy(game->renderer, selected_cell->text, NULL, &rect);
	}

	return false;
}

// func para comparar entradas do jogador com gabarito e retornar se teve vitoria ou nao
bool isWin (char cells[8][8])
{
	bool win_test = true;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (loadedGame.board[j][i] == cells[i][j] && win_test == true)
			{
				win_test = true;
			}
			else
			{
				win_test = false;
			}
		}
	}
	if (win_test)
	{
		return false;
	}
	return true;
}