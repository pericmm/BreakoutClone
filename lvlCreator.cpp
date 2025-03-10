#include "lvlCreator.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <fstream>
#include <list>
#include <vector>
#include <string>





void writeToFile(std::string filename, State state){
	std::ofstream WriteFile(filename);
	for(auto block: state.blocks){
		for(auto point: block){
			WriteFile << point.x << ',' << point.y << ',';
		}
		WriteFile  << '\n';
	}
	WriteFile.close();
}

void lvlCreator::eventHandler(SDL_Event &event, State &state){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			state.stage = 0;
		}
		if(event.type == SDL_MOUSEMOTION){
			if(state.currentPoint.x + event.motion.xrel > 0 
			&& state.currentPoint.x + event.motion.xrel < 1920)
				state.currentPoint.x += event.motion.xrel;
			if(state.currentPoint.y + event.motion.yrel > 0 
			&& state.currentPoint.y + event.motion.yrel < 900)
				state.currentPoint.y += event.motion.yrel;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if(event.button.button == 1)
				state.currentBlock.push_back(state.currentPoint);
			if(event.button.button == 3){
				state.blocks.push_back(state.currentBlock);
				state.currentBlock.clear();
			}
				
		}
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == SDLK_ESCAPE){
				state.stage = state.stage ^ 1;
				if(state.stage % 2)
					SDL_SetRelativeMouseMode(SDL_TRUE);
				else						
					SDL_SetRelativeMouseMode(SDL_FALSE);
			}
			if(event.key.keysym.sym == SDLK_w){
				writeToFile("level.txt", state);
			}
			if(event.key.keysym.sym == SDLK_g){
				state.stage = 2;
			}
		}
	}
}
