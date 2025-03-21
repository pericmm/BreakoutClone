#include "level.hpp"
#include "pauseScreen.hpp"
#include "structs.hpp"
#include <algorithm>
#include <fstream>

#define ITER2POINT(x) (&(*(x)))

namespace level {

SDL_FPoint pointRotate(SDL_Point p, float angle){
	SDL_FPoint result;
	result.x = cos(angle) * p.x - sin(angle) * p.y;
	result.y = sin(angle) * p.x + cos(angle) * p.y;
	return result;
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

void init(State& state){
	state.stage = 2;
	readLevelFromFile("level.txt", state);
	state.balls.clear();
}

void moveBalls(State& state){
	for(auto ball = state.balls.begin(); ball != state.balls.end(); ball++){
		if(!ball->isFixed){
			ball->c.x += ball->speedVector.x;
			ball->c.y += ball->speedVector.y;
		}
	}
}


void ballColision(Sphere* ball, State& state){
	if(ball->c.x - ball->r<0 || ball->c.x + ball->r>1920)
		ball->speedVector.x = -ball->speedVector.x;
	if(ball->c.y - ball->r < 0 || colisionDetect(ball, {state.paddle.x, state.paddle.y}, {state.paddle.x + state.paddle.w, state.paddle.y}))
		ball->speedVector.y = -ball->speedVector.y;
	for (auto block = state.blocks.begin(); block != state.blocks.end(); block++ ) {
		for(auto point = block->begin(); point != block->end(); point++){
			if (colisionDetect(ball, *point, point+1==block->end()? *block->begin():*(point+1))){
				bounceBall(ball, *point, point+1==block->end()? *block->begin():*(point+1));
				auto blocktemp= block--;
				state.blocks.erase(blocktemp);
				break;
			}
		}
	}
	
}

bool colisionDetect(Sphere* ball, SDL_Point p1, SDL_Point p2){
	float angle = (p1.x-p2.x)==0?M_PIf/2:atan2((p1.y-p2.y) , (p1.x-p2.x));
	SDL_FPoint c= pointRotate({static_cast<int>(ball->c.x),static_cast<int>(ball->c.y)}, -angle);
	SDL_FPoint r1 = pointRotate(p1, -angle);
	SDL_FPoint r2 = pointRotate(p2, -angle);
	if( pow(c.y-r1.y,2) + pow(c.x - std::clamp(c.x, r1.x<r2.x?r1.x:r2.x, r1.x<r2.x?r2.x:r1.x),2) <= pow(ball->r,2)){
		return true;
	}
	return false;
}

void bounceBall(Sphere* ball, SDL_Point p1, SDL_Point p2){
	SDL_Point v = {p2.x-p1.x,p2.y-p1.y};
	SDL_Point vn = {-v.y, v.x};
	float scale = 1/static_cast<float>(v.x*vn.y-v.y*vn.x);
	SDL_FPoint temp = {scale * (vn.y * ball->speedVector.x - vn.x * ball->speedVector.y),
				scale * (-v.y *ball->speedVector.x + v.x * ball->speedVector.y)};
	ball->speedVector = {ball->speedVector.x - 2 * temp.y * vn.x, ball->speedVector.y - 2 * temp.y * vn.y};
}

void eventHandler(SDL_Event& event,State& state){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			state.stage = 0;
		}
		if(event.type == SDL_MOUSEMOTION && !(state.stage % 2)){
			if(state.paddle.x+event.motion.xrel>0
			&& state.paddle.x+state.paddle.w+event.motion.xrel<1920){
				state.paddle.x+=event.motion.xrel;
				for(auto ball = state.balls.begin(); ball != state.balls.end(); ball++){
					if(ball->isFixed){
						ball->c.x+=event.motion.xrel;
					}
				}
			}
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			for(auto ball = state.balls.begin(); ball != state.balls.end(); ball++){
				ball->isFixed=false;
			}

		}
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == SDLK_ESCAPE){
				pauseScreen::init(state);
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

void draw(State& state, SDL_Renderer* renderer, std::vector<SDL_Texture*>& textures){
	moveBalls(state);
	for(auto ball = state.balls.begin(); ball != state.balls.end();ball++){
		level::ballColision(ITER2POINT(ball),state);
		if(ball->c.y > 1010){
			state.balls.erase(ball);
		}
		if(state.balls.empty())
			break;
	}
	
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	for (auto block = state.blocks.begin(); block != state.blocks.end(); block++ ) {
		for(auto point = block->begin(); point != block->end(); point++)
			SDL_RenderDrawLine(renderer, point->x, point->y, 
					point+1==block->end()? (block->begin())->x:(point+1)->x,
					point+1==block->end()? (block->begin())->y:(point+1)->y);
	}

		
	SDL_RenderFillRect(renderer,&state.paddle);
	for(auto ball = state.balls.begin(); ball != state.balls.end(); ball++){
		SDL_Rect circumscribedSquare = ball->rect();
		SDL_RenderCopy(renderer, textures.at(0), NULL, &circumscribedSquare);
	}
}



}