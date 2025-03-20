#include "lvlCreator.hpp"
#include "level.hpp"
#include "structs.hpp"
#include "pauseScreen.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
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





std::vector<std::string> splitString(std::string s, char delimiter){
	std::vector<std::string> result;
	std::string temp= "";
	for(char c: s){
		if(c == delimiter || c==EOF){
			result.push_back(temp);
			temp = "";
			continue;
		}
		temp += c;
	}
	result.push_back(temp);
	return result;
}
std::vector<SDL_Point> stringToPoint(std::vector<std::string> input){
	std::vector<SDL_Point> result;
	for (int i=0; i<=input.size()-2; i=i+2 ) {
		result.push_back({std::stoi(input[i]),std::stoi(input[i+1])});
	}
	return result;
}
void readLevelFromFile(std::string fileName, State& state){
	std::string nextLine;
	std::ifstream ReadFile(fileName);
	std::vector<SDL_Point> block;
	int x,y;
	while(getline(ReadFile,nextLine)){
		state.blocks.push_back(stringToPoint(splitString(nextLine, ',')));
	}
	ReadFile.close();
}











int main(){
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	State state;




	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(1920,1080,0,&window, &renderer);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_Surface* surface = SDL_LoadBMP("large.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	std::vector<SDL_Texture*> textures;
	textures.push_back(texture);
	SDL_FreeSurface(surface);

	readLevelFromFile("level.txt", state);

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
				pauseScreen::draw(renderer);
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

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}