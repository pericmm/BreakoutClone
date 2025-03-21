#include "pauseScreen.hpp"
#include "structs.hpp"
#include "lvlCreator.hpp"
#include "level.hpp"
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <string>

namespace pauseScreen{

void init(State& state){
	state.stage += 1;
	int x,y;
	SDL_GetMouseState(&x, &y);
	state.cursor = {x,y};
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void eventHandler(SDL_Event& event,State& state){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			state.stage = 0;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if((state.cursor.x>400&& state.cursor.x<1920 - 400) && (state.cursor.y>2*100+ 100 && state.cursor.y< 2*100+100+80)){
				state.stage -= 1;
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}else if((state.cursor.x>400&& state.cursor.x<1920 - 400) && (state.cursor.y>3*100+ 100 && state.cursor.y< 3*100+100+80)){
				level::init(state);
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}else if((state.cursor.x>400&& state.cursor.x<1920 - 400) && (state.cursor.y>4*100+ 100 && state.cursor.y< 4*100+100+80)){
				lvlCreator::init(state);
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}else if((state.cursor.x>400&& state.cursor.x<1920 - 400) && (state.cursor.y>5*100+ 100 && state.cursor.y< 5*100+100+80)){
				state.stage = 0;
			}
			

		}
		if(event.type == SDL_MOUSEMOTION){
			if(state.currentPoint.x + event.motion.xrel > 0 
			&& state.currentPoint.x + event.motion.xrel < 1920)
				state.cursor.x += event.motion.xrel;
			if(state.currentPoint.y + event.motion.yrel > 0 
			&& state.currentPoint.y + event.motion.yrel < 900)
				state.cursor.y += event.motion.yrel;
		}
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == SDLK_ESCAPE){
				state.stage -= 1; 
				SDL_SetRelativeMouseMode(SDL_TRUE);
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


void drawButton(SDL_Renderer* renderer, std::string text, State& state, int bNum){
	SDL_Surface* s_Text = TTF_RenderText_Solid(state.font, text.c_str(), {255,255,255,255});
	SDL_Texture* t_Text = SDL_CreateTextureFromSurface(renderer, s_Text);
	SDL_FreeSurface(s_Text);

	SDL_Rect buttonWrap = {400, bNum * 100 + 100, 1920 - 2 * 400, 80};
	if((state.cursor.x>400&& state.cursor.x<1920 - 400) && (state.cursor.y>bNum*100+ 100 && state.cursor.y< bNum*100+100+80)){
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
	}else {
		SDL_SetRenderDrawColor(renderer,0,0,255,255);
	}
	SDL_RenderFillRect(renderer,&buttonWrap);
	SDL_Rect buttonText = {buttonWrap.x + 300,buttonWrap.y + 20, static_cast<int>(text.size()*50), 50};
	SDL_RenderCopy(renderer, t_Text, NULL, &buttonText);

}




void draw(SDL_Renderer* renderer, State& state){
	SDL_Color fgColor = {255,255,255, 255};
	SDL_Color bgColor = {0,0,0};
	SDL_Surface* s_Pause = TTF_RenderText_Solid(state.font, "PAUSE", fgColor);
	SDL_Texture* t_Pause = SDL_CreateTextureFromSurface(renderer, s_Pause);
	SDL_FreeSurface(s_Pause);

	SDL_Rect pauseButton = {800,100,200,50};
	SDL_RenderCopy(renderer, t_Pause, NULL, &pauseButton);


	drawButton(renderer, "RESUME", state, 2);
	drawButton(renderer, "GAME", state, 3);
	drawButton(renderer, "LEVEL EDITOR", state, 4);
	drawButton(renderer, "QUIT", state, 5);

	
}


}