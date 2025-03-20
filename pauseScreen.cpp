#include "pauseScreen.hpp"
#include <SDL2/SDL_stdinc.h>


namespace pauseScreen{

void init(State& state){
	state.stage += 1;
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void eventHandler(SDL_Event& event,State& state){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			state.stage = 0;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if(state.stage % 2){
				state.stage -= 1;
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
			else{
				for(auto ball = state.balls.begin(); ball != state.balls.end(); ball++){
					ball->isFixed=false;
				}
			}

		}
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == SDLK_ESCAPE){
				state.stage -= 1; 
				SDL_SetRelativeMouseMode(SDL_FALSE);
			}
			if(event.key.keysym.sym == SDLK_q){
				state.stage = 0;
			}
			if(event.key.keysym.sym == SDLK_c){
				state.stage = 8;
			}
		}
	}
}


void draw(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer,0,0,255,255);
	SDL_Rect pauseScreen = {50,50,1800,800};
	SDL_RenderFillRect(renderer,&pauseScreen);
}


}