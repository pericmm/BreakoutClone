#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <list>
#include <string>
#include <unistd.h>
#include <vector>
#include <cmath>

#define ITER2POINT(x) (&(*(x)))

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

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

std::list<std::vector<SDL_Point>> blocks;
std::list<Sphere> balls;
SDL_Rect paddle = {1920/2,1080-80, 100, 10};

void clearScreen(){
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_RenderClear(renderer);
}
void drawPauseScreen(){
	SDL_SetRenderDrawColor(renderer,0,0,255,255);
	SDL_Rect pauseScreen = {50,50,1800,800};
	SDL_RenderFillRect(renderer,&pauseScreen);
}

SDL_FPoint normalize(SDL_FPoint p){
	float scale = std::sqrt(p.x*p.x + p.y*p.y);
	p.x = p.x  / scale;
	p.y = p.y  / scale;
	return p;
}

void restartGame(){

	Sphere temp;
	SDL_Point ballPosition = {paddle.x + paddle.w / 2 + temp.r * 2, paddle.y -  temp.r};
	temp.c = {static_cast<float>(ballPosition.x),static_cast<float>(ballPosition.y)};
	SDL_FPoint ballSpeed = normalize({1, -1});
	temp.speedVector = ballSpeed;
	balls.push_back(temp);
}
void moveBalls(){
	for(auto ball = balls.begin(); ball != balls.end(); ball++){
		if(!ball->isFixed){
			ball->c.x += ball->speedVector.x;
			ball->c.y += ball->speedVector.y;
		}
	}
}
SDL_FPoint pointRotate(SDL_Point p, float angle){
	SDL_FPoint result;
	result.x = cos(angle) * p.x - sin(angle) * p.y;
	result.y = sin(angle) * p.x + cos(angle) * p.y;
	return result;
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
void ballColision(Sphere* ball){
	if(ball->c.x - ball->r<0 || ball->c.x + ball->r>1920)
		ball->speedVector.x = -ball->speedVector.x;
	if(ball->c.y - ball->r < 0 || colisionDetect(ball, {paddle.x, paddle.y}, {paddle.x + paddle.w, paddle.y}))
		ball->speedVector.y = -ball->speedVector.y;
	for (auto block = blocks.begin(); block != blocks.end(); block++ ) {
		for(auto point = block->begin(); point != block->end(); point++){
			if (colisionDetect(ball, *point, point+1==block->end()? *block->begin():*(point+1))){
				bounceBall(ball, *point, point+1==block->end()? *block->begin():*(point+1));
				auto blocktemp= block--;
				blocks.erase(blocktemp);
				break;
			}
		}
	}
	
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
void readLevelFromFile(std::string fileName){
	std::string nextLine;
	std::ifstream ReadFile(fileName);
	std::vector<SDL_Point> block;
	int x,y;
	while(getline(ReadFile,nextLine)){
		blocks.push_back(stringToPoint(splitString(nextLine, ',')));
	}
	ReadFile.close();
}





int main(){

	bool isRunning = true;
	bool isPaused = false;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(1920,1080,0,&window, &renderer);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_Surface* surface = SDL_LoadBMP("large.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	readLevelFromFile("level.txt");

	while (isRunning){
		if(balls.empty()){
			restartGame();
		}

		usleep(1000);
		clearScreen();

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				isRunning = false;
			}
			if(event.type == SDL_MOUSEMOTION && !isPaused){
				if(paddle.x+event.motion.xrel>0
				&& paddle.x+paddle.w+event.motion.xrel<1920){
					paddle.x+=event.motion.xrel;
					for(auto ball = balls.begin(); ball != balls.end(); ball++){
						if(ball->isFixed){
							ball->c.x+=event.motion.xrel;
						}
					}
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN){
				if(isPaused){
					isPaused=(!isPaused);
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}
				else{
					for(auto ball = balls.begin(); ball != balls.end(); ball++){
						ball->isFixed=false;
					}
				}

			}
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE){
					isPaused=(!isPaused);
					if(!isPaused)
						SDL_SetRelativeMouseMode(SDL_TRUE);
					else
						SDL_SetRelativeMouseMode(SDL_FALSE);
				}
			}
		}









		


		if(isPaused){
			drawPauseScreen();
			SDL_RenderPresent(renderer);
			continue;
		}




		

		
		moveBalls();
		for(auto ball = balls.begin(); ball != balls.end();ball++){
			ballColision(ITER2POINT(ball));
			if(ball->c.y > 1010){
				balls.erase(ball);
			}
			if(balls.empty())
				break;
		}
		
		SDL_SetRenderDrawColor(renderer,255,255,255,255);

		for (auto block = blocks.begin(); block != blocks.end(); block++ ) {
			for(auto point = block->begin(); point != block->end(); point++)
				SDL_RenderDrawLine(renderer, point->x, point->y, 
						point+1==block->end()? (block->begin())->x:(point+1)->x,
						point+1==block->end()? (block->begin())->y:(point+1)->y);
		}


		
		SDL_RenderFillRect(renderer,&paddle);
		for(auto ball = balls.begin(); ball != balls.end(); ball++){
			SDL_Rect circumscribedSquare = ball->rect();
			SDL_RenderCopy(renderer, texture, NULL, &circumscribedSquare);
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}