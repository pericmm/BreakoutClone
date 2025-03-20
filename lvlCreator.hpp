#ifndef __LVL_CREATOR__
#define __LVL_CREATOR__
#include "structs.hpp"

namespace lvlCreator {
	void eventHandler(SDL_Event& event, State& state);
	void draw(State& state, SDL_Renderer* renderer);
}

#endif