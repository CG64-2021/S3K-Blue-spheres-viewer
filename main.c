#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>

#include "m_utils.h"

#define BASEWIDTH 320
#define BASEHEIGHT 224

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
//SDL_Surface* surface = NULL;
SDL_Texture* texture = NULL;

int closegame = 0; //Close game?
uint8_t stage[32*32]; //A special stage has a size of 32x32 spheres
uint16_t ringsleft; //How many rings left?

typedef struct
{
	SDL_Texture* sprite_sheet;
	SDL_Rect rect;
} sprite_t;

sprite_t spr_redball;
sprite_t spr_blueball;
sprite_t spr_whiteball;

typedef struct
{
	fixed_t x;
	fixed_t y;
} vector_t;

typedef struct
{
	vector_t pos;
	vector_t speed;
	fixedu_t angle;
} player_t;
player_t player;

void ParseStage(FILE* fp)
{
	fp = fopen("S31.bin", "rb");
	
	size_t i;
	for(i=0; i < 0x400; i=ftell(fp))
	{
		fread(&stage[i], 1, 1, fp);
	}
	
	//Parse player info
	fread(&player.angle, sizeof(fixedu_t), 1, fp);
	fread(&player.pos.x, sizeof(fixed_t), 1, fp);
	fread(&player.pos.y, sizeof(fixed_t), 1, fp);
	fread(&ringsleft, sizeof(ringsleft), 1, fp);
	
	//Convert endianess
	player.angle = SHORT(player.angle);
	player.pos.x = SHORT(player.pos.x);
	player.pos.y = SHORT(player.pos.y);
	ringsleft = SHORT(ringsleft);
}

void LoadSpriteSheet(sprite_t* spr, const char* filename)
{
	SDL_Surface* imgsurface = IMG_Load(filename);
	spr->sprite_sheet = SDL_CreateTextureFromSurface(renderer, imgsurface);
	SDL_FreeSurface(imgsurface);
}

void init()
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("S3K Blue Spheres", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMG_Init(IMG_INIT_PNG);
	
	LoadSpriteSheet(&spr_redball, "assets/collectables.png");
	LoadSpriteSheet(&spr_blueball, "assets/collectables.png");
	LoadSpriteSheet(&spr_whiteball, "assets/collectables.png");
	
	spr_blueball.rect.x = 0;
	spr_blueball.rect.y = 0;
	spr_blueball.rect.w = 32;
	spr_blueball.rect.h = 31;
	
	spr_redball.rect.x = 0;
	spr_redball.rect.y = 32;
	spr_redball.rect.w = 32;
	spr_redball.rect.h = 31;
	
	spr_whiteball.rect.x = 0;
	spr_whiteball.rect.y = 32+32;
	spr_whiteball.rect.w = 32;
	spr_whiteball.rect.h = 32;
}

void input()
{
	SDL_Event event;
	
	while(SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT) closegame = 1;
		
		if (event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_UP:
					player.pos.x += FixedMul((1 << FRACBITS), FixedCos(player.angle >> FRACBITS));
					player.pos.y -= FixedMul((1 << FRACBITS), FixedSin(player.angle >> FRACBITS));
				break;
				case SDLK_DOWN:
					player.pos.x -= FixedMul((1 << FRACBITS), FixedCos(player.angle >> FRACBITS));
					player.pos.y += FixedMul((1 << FRACBITS), FixedSin(player.angle >> FRACBITS)); 
				break;
				case SDLK_LEFT:
					player.angle -= (1 << FRACBITS);
				break;
				case SDLK_RIGHT:
					player.angle += (1 << FRACBITS);
				break;
			}
		}
	}
}

void update()
{
	
}

void draw()
{
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(renderer);
	
	size_t x, y;
	for(y=0; y < 32; ++y)
	{	
		for(x=0; x < 32; ++x)
		{
			fixed_t xx = (x << FRACBITS);
			fixed_t yy = (y << FRACBITS);
			
			//Transform the vertexes relative to the player
			fixed_t tx = xx - player.pos.x;
			fixed_t ty = yy - player.pos.y;
			
			//Rotate around player's view
			uint16_t angle = player.angle>>FRACBITS;
			fixed_t tz = FixedMul(tx, FixedCos(angle)) + FixedMul(ty, FixedSin(angle));
			tx = FixedMul(tx, FixedSin(angle)) - FixedMul(ty, FixedCos(angle));
			
			SDL_Rect sphere_rect;
			sphere_rect.x = (FixedInt(tx+player.pos.x) >> FRACBITS)*16;
			sphere_rect.y = (FixedInt(tz+player.pos.y) >> FRACBITS)*16;
			sphere_rect.w = 16;
			sphere_rect.h = 16;
			
			switch(stage[y*32+x])
			{
				//case 0: SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF); break;
				case 1: SDL_RenderCopy(renderer, spr_redball.sprite_sheet, &spr_redball.rect, &sphere_rect); break;
				case 2: SDL_RenderCopy(renderer, spr_blueball.sprite_sheet, &spr_blueball.rect, &sphere_rect); break;
				case 3: SDL_RenderCopy(renderer, spr_whiteball.sprite_sheet, &spr_whiteball.rect, &sphere_rect); break;
			}
		}
	}
	//player.angle += (1 << FRACBITS);
	//player.pos.x = (16 << FRACBITS);
	//player.pos.y = (16 << FRACBITS);
	
	SDL_Rect player_rect;
	player_rect.x = (FixedInt(player.pos.x) >> FRACBITS)*16;
	player_rect.y = (FixedInt(player.pos.y) >> FRACBITS)*16;
	player_rect.w = 16;
	player_rect.h = 16;
	SDL_SetRenderDrawColor(renderer, 0x0, 0xFF, 0x0, 0xFF);
	SDL_RenderFillRect(renderer, &player_rect);
	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x0, 0xFF);
	int x1, y1, x2, y2;
	x1 = player_rect.x+8;
	y1 = player_rect.y+8;
	x2 = x1 + (FixedInt(FixedMul(10 << FRACBITS, FixedCos(player.angle))) >> FRACBITS); 
	y2 = y1 - (FixedInt(FixedMul(10 << FRACBITS, FixedSin(player.angle))) >> FRACBITS);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	
	SDL_RenderPresent(renderer);
}

void close()
{
	
}

int main(int argc, char** argv)
{
	FILE* fp;
	
	init();
	ParseStage(fp);
	
	size_t fps = 60;
	uint32_t elapsed;
	uint32_t frameTime;
	
	while(!closegame)
	{
		elapsed = SDL_GetTicks();
		
		input();
		update();
		draw();
		
		frameTime = SDL_GetTicks() - elapsed;
		
		if (1000/fps > frameTime)
		{
			SDL_Delay((1000/fps) - frameTime);
		}
	}
	
	close();
	
	return 0;
}