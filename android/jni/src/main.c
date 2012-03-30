#include <SDL_main.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_log.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
#endif

SDL_Window* window;
SDL_Renderer* renderer;

int main(int argc, char *argv[]) {

	SDL_Log("Starting up..");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Error %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	window = SDL_CreateWindow("a window", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN);
	if (window == NULL) {
		SDL_Log("Error %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(renderer == NULL){
		SDL_Log("RENDERER is null\n");
	}

	int i;
	for (i = 0; i < 50; i++) {
		SDL_Log("HERE!\n");
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
		sleep(2);
	}

	SDL_Quit();
}
