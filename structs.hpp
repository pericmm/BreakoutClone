#ifndef __STRUCTS__
#define __STRUCTS__
#include <SDL2/SDL.h>
#include <list>
#include <vector>


struct Sphere{
	SDL_FPoint c;
	SDL_FPoint speedVector;
	int r = 10;
	bool isFixed = true;

	SDL_Rect rect(){
		SDL_Rect temp = {static_cast<int>(c.x-r),static_cast<int>(c.y-r),2*r,2*r};
		return temp;
	}
};
struct State{
	std::list<std::vector<SDL_Point>> blocks;
	std::list<Sphere> balls;
	std::vector<SDL_Point> currentBlock;
	SDL_Rect paddle = {1920/2,1080-80, 100, 10};
	SDL_Point currentPoint = {900, 500};
	int stage = 2;
};


#endif