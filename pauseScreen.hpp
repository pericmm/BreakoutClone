#ifndef __PAUSE__
#define __PAUSE__
#include "structs.hpp"

namespace pauseScreen {



	void init(State& state);
	void eventHandler(SDL_Event& event,State& state);
	void draw(SDL_Renderer* renderer, State& state);

}

#endif