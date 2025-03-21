#include "lvlCreator.hpp"
#include "level.hpp"
#include "structs.hpp"
#include "pauseScreen.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <iostream>
#include <list>
#include <unistd.h>
#include <vector>
#include <cmath>

#define ITER2POINT(x) (&(*(x)))



void clearScreen(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_RenderClear(renderer);
}

SDL_FPoint normalize(SDL_FPoint p){
	float scale = std::sqrt(p.x*p.x + p.y*p.y);
	p.x = p.x  / scale;
	p.y = p.y  / scale;
	return p;
}

void restartGame(State& state){
	Sphere temp;
	SDL_Point ballPosition = {state.paddle.x + state.paddle.w / 2 + temp.r * 2, state.paddle.y -  temp.r};
	temp.c = {static_cast<float>(ballPosition.x),static_cast<float>(ballPosition.y)};
	SDL_FPoint ballSpeed = normalize({1, -1});
	temp.speedVector = ballSpeed;
	state.balls.push_back(temp);
}

















int main(){
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	State state;




	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(1920,1080,0,&window, &renderer);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	
	TTF_Init();
	state.font = TTF_OpenFont("Arial.ttf", 50);

	SDL_Surface* surface = SDL_LoadBMP("large.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	std::vector<SDL_Texture*> textures;
	textures.push_back(texture);
	SDL_FreeSurface(surface);

	level::init(state);

	while (state.stage != 0){
		if(state.balls.empty()){
			restartGame(state);
		}

		usleep(1000);
		clearScreen(renderer);

		SDL_Event event;
		switch (state.stage) {
			case 1:
			case 3:
			case 5:
			case 9:
				pauseScreen::eventHandler(event, state);
				pauseScreen::draw(renderer, state);
				break;
			case 2:
				level::eventHandler(event, state);
				level::draw(state, renderer, textures);
				break;
			case 4:
				
				break;
			case 8:
				lvlCreator::eventHandler(event, state);
				lvlCreator::draw(state,renderer);
				break;
			default:
				std::cout<<"invalid stage"<<std::endl;
				state.stage =  0;
		}



		SDL_RenderPresent(renderer);
	}
	TTF_CloseFont(state.font);
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}