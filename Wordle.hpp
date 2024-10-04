#pragma once

#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>

#define STR_LEN 5
#define MAX_TRY 6
#define DEBUG 0
#define RENDER_MODE 1

//graphics
#define TEXTURE_COUNT 26
#define SPRITE_SIZE 150
#define BLOCK_SIZE SPRITE_SIZE
#define GAP_SIZE 10
#define OFFSET (SPRITE_SIZE + GAP_SIZE) + GAP_SIZE //intentional gap in brackets
#define VRAM_X  (SPRITE_SIZE * STR_LEN + (STR_LEN + 1) * GAP_SIZE)
#define VRAM_Y  (SPRITE_SIZE * MAX_TRY + (MAX_TRY + 1) * GAP_SIZE)
#define SCREEN_X (BLOCK_SIZE * STR_LEN + (STR_LEN + 1) * GAP_SIZE)
#define SCREEN_Y (BLOCK_SIZE * MAX_TRY + (MAX_TRY + 1) * GAP_SIZE)

//colours
#define WHITE_RGB  { 40,  40,  40, 0 }
#define GRAY_RGB   { 55,  55,  55, 0 }
#define YELLOW_RGB { 50,  200, 200, 0 }
#define GREEN_RGB  { 40,  180, 40,  0 }
#define D_GRAY	   0x44444444

enum {
	WHITE, GRAY, YELLOW, GREEN
};

typedef struct pixel {
	
	uint8_t		b, g, r, a;
}	pixel;

typedef struct block {
	std::string str;
	uint8_t		state[STR_LEN];
}	block;

typedef struct gameInfo {
	std::string*	wordList;
	uint32_t		listLength;
	pixel**			textures;
	// vvv re-initialised after every loop vvv
	block*			grid;
	std::string		answer;
	bool			finished;
}	gameInfo;

//graphics
pixel**			initTextures();
void			initRender(uint32_t vramX, uint32_t vramY, uint32_t scrX, uint32_t scrY);
void			drawTexture(int x, int y, int size, int blockXCount, pixel* texture);
void			renderLoop(gameInfo* info);

//string tools
std::string		toUpper(std::string str);
bool			checkYellow(std::string word, std::string answer, UINT index);
bool			validWord(std::string word, std::string* word_list, uint32_t listLenght);
bool			handleWord(gameInfo* info, uint8_t y);
