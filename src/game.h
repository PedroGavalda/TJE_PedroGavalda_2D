/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "image.h"
#include "utils.h"
#include "synth.h"

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	SDL_Renderer* renderer;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	//audio
	Synth synth;

	//ctor
	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	void showFramebuffer(Image* img);

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseMove(SDL_MouseMotionEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	//audio stuff
	void enableAudio(); //opens audio channel to play sound
	void onAudio(float* buffer, unsigned int len, double time, SDL_AudioSpec &audio_spec); //called constantly to fill the audio buffer
};

// COSAS AÑADIDAS DE EJEMPLOQUE QUIZAS NO ME SIRVEN
enum eCellType: uint16 { EMPTY, START, WALL, DOOR, CHEST };
enum eItemType: uint16 { NOTHING, SWORD, POTION };

struct sCell {
	eCellType type;
	eItemType item;
	uint16 tileId;
};

struct sObject {
	eCellType type;
	Vector2 position;
};

struct sLayer {
	sCell* data;
};

class GameMap {
public:
	int width = 0;
	int height = 0;

	int tile_width = 8;
	int tile_height = 8;

	sLayer* layers = nullptr;

	GameMap() {}

	GameMap(int w, int h) {
		width = w;
		height = h;
	}

	sCell& getCell(int x, int y, int l) {
		return layers[l].data[x + y * width];
	}
};

// MIO RITMO

struct Song {
	std::string title;
	std::string file_direction;
};

struct Note {
	double time;
	bool completed;
};

struct Stage { 
	Song song;
	std::vector<Note> notes;
};



void drawNotes(Image& framebuffer, std::vector<Note>& notes, float x);

void evaluate_note(double difference);

#endif 