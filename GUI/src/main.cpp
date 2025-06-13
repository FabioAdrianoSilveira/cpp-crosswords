#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WINDOW_TITLE "Cruzadinhas++"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define TEXT_SIZE 23

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

struct SelectedCell {
	int i, j;
	SDL_Texture *text;
	std::string hint;
};

void game_cleanup(struct Game *game, int exit_status);
bool load_media(struct Game *game, struct SelectedCell *selected_cell);
bool sdl_initialize(struct Game *game);
bool render_text(struct Game *game, std::string text);
void draw_canvas(struct Game *game, int canvas[8][8], struct SelectedCell *selected_cell, char cells[8][8]);

int main() {

	char cells[8][8];
	SelectedCell selected_cell = {
		.hint = "Boas vindas às Cruzadinhas++!\nClique em uma célula, leia a dica e digite a palavra em seu teclado!"
	};
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

	int mousepos_x, mousepos_y;

	bool drawing_text = true;

	int text_counter = 1;

    struct Game game = {
        .window = NULL,
        .renderer = NULL,
		.text_font = NULL,
		.text_color = {255, 255, 255, 255},
		.text_image = NULL
    };

    if (sdl_initialize(&game)) {
        game_cleanup(&game, EXIT_FAILURE);
    }

	if (load_media(&game, &selected_cell)) {
        game_cleanup(&game, EXIT_FAILURE);
    }

    while (true) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                game_cleanup(&game, EXIT_SUCCESS);
                break;
            case SDL_MOUSEBUTTONDOWN:
				drawing_text = true;
				
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

				// selected_cell.hint = cells[selected_cell.i][selected_cell.j].hint;
            default:
                break;
            }
        }
        SDL_RenderClear(game.renderer);

		// DESENHAR O CANVAS
		draw_canvas(&game, canvas, &selected_cell, cells);

		// DESENHAR O TEXTO
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

        SDL_RenderPresent(game.renderer);

        SDL_Delay(30);
    }

    game_cleanup(&game, EXIT_SUCCESS);

    return 0;
}

void game_cleanup(struct Game *game, int exit_status) {
	SDL_DestroyTexture(game->cell);
	SDL_DestroyTexture(game->cell_selected);

	SDL_DestroyTexture(game->text_image);
	TTF_CloseFont(game->text_font);

    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

	TTF_Quit();
	IMG_Quit();
    SDL_Quit();
    exit(exit_status);
}

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

void draw_canvas(struct Game *game, int canvas[8][8], struct SelectedCell *selected_cell, char cells[8][8])
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
			rect.x = rect.x + 100;
		}
		rect.x = 0;
		rect.y = rect.y + 100;
	}

	if(canvas[selected_cell->j][selected_cell->i] != 0)
	{
		rect.x = selected_cell->i * 100;
		rect.y = selected_cell->j * 100;
		SDL_RenderCopy(game->renderer, selected_cell->text, NULL, &rect);

		rect.x += 27;
		rect.y += 25;
		rect.h = TEXT_SIZE * 2;
		rect.w = TEXT_SIZE * 2;
		const SDL_Rect crect = rect;
		std::string c = "B";

		SDL_Surface *surface = TTF_RenderUTF8_Blended(game->text_font, std::data(c), game->text_color);
    	if (!surface) {
        	std::cout << "Error creating Surface: " << SDL_GetError() << std::endl;
    	}
    	game->text_rect.w = surface->w;
    	game->text_rect.h = surface->h;
    	game->char_image[selected_cell->j][selected_cell->i] = SDL_CreateTextureFromSurface(game->renderer, surface);
    	SDL_FreeSurface(surface);
    	if (!game->text_image) {
        	std::cout << "Error creating Texture: " << SDL_GetError() << std::endl;
    	}
		SDL_RenderCopy(game->renderer, game->char_image[selected_cell->j][selected_cell->i], NULL, &crect);
	}
}