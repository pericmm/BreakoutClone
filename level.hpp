#ifndef __LEVEL__
#define __LEVEL__
#include "structs.hpp"

namespace level {
	void ballColision(Sphere* ball, State& state);
	bool colisionDetect(Sphere* ball, SDL_Point p1, SDL_Point p2);
	void bounceBall(Sphere* ball, SDL_Point p1, SDL_Point p2);
	void eventHandler(SDL_Event& event,State& state);
	void draw(State& state, SDL_Renderer* renderer, std::vector<SDL_Texture*>& textures);
}

#endif