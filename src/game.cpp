#include "game.h"
#include "utils.h"
#include "input.h"
#include "image.h"
#include "json.hpp"
#include <fstream>

#include <cmath>

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

#define NOTE_SCORE 100

Game* Game::instance = NULL;

Image font;
Image minifont;
Image sprite;
Color bgcolor(130, 80, 100);

// MIO RITMO
int score = 0;
std::string score_message;

double difference = 0.0;

int current_left = 0;
int current_down = 0;
int current_up = 0;
int current_right = 0;

std::vector<Note> left_notes;
std::vector<Note> down_notes;
std::vector<Note> up_notes;
std::vector<Note> right_notes;

Song song;
double song_delay = 3.0;
bool music_started = false;
int note_count;
int max_score;

Synth::Sample* mario_sample = nullptr;
Synth::Sample* hit_sample = nullptr;

// RITMO ASSETS
Image note_sprites;
Image runner_sprites;
Image progress_bar;
Image arrow_background;
Image run_background;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	this->renderer = NULL;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;

	font.load("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.load("data/mini-font-white-4x6.tga"); //load bitmap-font image
	sprite.load("data/spritesheet.tga"); //example to load an sprite

	// RITMO
	note_sprites.load("data/arrows.tga");
	runner_sprites.load("data/miku_run_1.tga");
	progress_bar.load("data/progress_bar.tga");
	arrow_background.load("data/arrow_background.tga");
	run_background.load("data/run_background.png");

	enableAudio(); //enable this line if you plan to add audio to your application
	mario_sample = synth.loadSample("data/mario3.wav");
	hit_sample = synth.loadSample("data/mouseclick.wav");
	//.osc1.amplitude = 0.5;

	song = { "Mario 1-1", "data/mario.txt" };
	load_notes(song.file_direction);
	note_count = left_notes.size() + down_notes.size() + up_notes.size() + right_notes.size();
	max_score = note_count * NOTE_SCORE;
}

//what to do when the image has to be draw
void Game::render(void)
{
	//Create a new Image (or we could create a global one if we want to keep the previous frame)
	Image framebuffer(160, 120); //do not change framebuffer size

	//add your code here to fill the framebuffer
	//...

	//some new useful functions
		framebuffer.fill( bgcolor );								//fills the image with one color
		//framebuffer.drawLine( 0, 0, 100,100, Color::RED );		//draws a line
		//framebuffer.drawImage( sprite, 0, 0 );					//draws full image
		//framebuffer.drawImage( sprite, 0, 0, framebuffer.width, framebuffer.height );			//draws a scaled image
		//framebuffer.drawImage( sprite, 0, 0, Area(0,0,14,18) );	//draws only a part of an image
		//framebuffer.drawText( "Hello World", 0, 0, font );				//draws some text using a bitmap font in an image (assuming every char is 7x9)
		//framebuffer.drawText( toString(time), 1, 10, minifont,4,6);	//draws some text using a bitmap font in an image (assuming every char is 4x6)
		
	draw_rhythm_scene(framebuffer);
	draw_notes(framebuffer, up_notes, UP);
	draw_notes(framebuffer, left_notes, LEFT);
	draw_notes(framebuffer, down_notes, DOWN);
	draw_notes(framebuffer, right_notes, RIGHT);
	draw_progress_bar(framebuffer);
		
	//send image to screen
	showFramebuffer(&framebuffer);
}

void Game::update(double seconds_elapsed)
{
	//Add here your update method
	//...
	if (!music_started && time >= song_delay)
	{
		synth.playSample(mario_sample, 1.0f, false);
		music_started = true;
	}

	update_notes(left_notes, current_left);
	update_notes(down_notes, current_down);
	update_notes(up_notes, current_up);
	update_notes(right_notes, current_right);

	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) //if key up
	{
	}
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) //if key down
	{
	}
	/*
	if (current_note >= size)
	{
		return;
	}*/

	if (Input::wasKeyPressed(SDL_SCANCODE_LEFT)) {
		if (current_left < left_notes.size()) {
			Note& note = left_notes[current_left];
			difference = fabs(note.time - (time - song_delay));

			if (difference <= 0.3) {
				evaluate_note(difference);
				note.completed = true;
				current_left++;
			}
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN)) {
		if (current_down < down_notes.size()) {
			Note& note = down_notes[current_down];
			difference = fabs(note.time - (time - song_delay));
			
			if (difference <= 0.3) {
				evaluate_note(difference);
				note.completed = true;
				current_down++;
			}
		}
	}
	
	if (Input::wasKeyPressed(SDL_SCANCODE_UP)) {
		if (current_up < up_notes.size()) {
			Note& note = up_notes[current_up];
			difference = fabs(note.time - (time - song_delay));

			if (difference <= 0.3) {
				evaluate_note(difference);
				note.completed = true;
				current_up++;
			}
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_RIGHT)) {
		if (current_right < right_notes.size()) {
			Note& note = right_notes[current_right];
			difference = fabs(note.time - (time - song_delay));
			
			if (difference <= 0.3) {
				evaluate_note(difference);
				note.completed = true;
				current_right++;
			}
		}
	}

	// podria evitar duplicidad aqui
	if (Input::wasKeyPressed(SDL_SCANCODE_LEFT) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT) || Input::wasKeyPressed(SDL_SCANCODE_UP) || Input::wasKeyPressed(SDL_SCANCODE_DOWN)) 
	{
		synth.playSample(hit_sample, 0.5f, false);
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_A)) //if key A was pressed
	{
	
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) //if key Z was pressed
	{
	}

	//to read the gamepad state
	if (Input::gamepads[0].isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (Input::gamepads[0].direction & PAD_UP) //left stick pointing up
	{
		bgcolor.set(0, 255, 0);
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseMove(SDL_MouseMotionEvent event)
{
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	#ifdef USE_OPENGL
		glViewport( 0,0, width, height );
	#endif
	window_width = width;
	window_height = height;
}

//sends the image to the framebuffer of the GPU
void Game::showFramebuffer(Image* img)
{
	float startx = -1.0; float starty = -1.0;
	float width = 2.0; float height = 2.0;

	//center in window
	float real_aspect = window_width / (float)window_height;
	float desired_aspect = img->width / (float)img->height;
	float diff = desired_aspect / real_aspect;
	width *= diff;
	startx = -diff;

#ifdef USE_OPENGL
	static GLuint texture_id = -1;
	static GLuint shader_id = -1;
	if (!texture_id)
		glGenTextures(1, &texture_id);

	//upload as texture
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
	glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST); glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(startx, starty + height);
	glTexCoord2f(1.0, 0.0); glVertex2f(startx + width, starty + height);
	glTexCoord2f(1.0, 1.0); glVertex2f(startx + width, starty);
	glTexCoord2f(0.0, 1.0); glVertex2f(startx, starty);
	glEnd();
#else
	static SDL_Texture* texture = NULL;
	static int tex_width = 0;
	static int tex_height = 0;
	if (!texture || tex_width != img->width || tex_height != img->height)
	{
		if(texture)
			SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_TARGET, img->width, img->height);
		tex_width = img->width;
		tex_height = img->height;
	}

	//SDL_RenderClear(renderer);
	SDL_Rect rect = { 0, 0, (int)img->width, (int)img->height };
	SDL_UpdateTexture(texture, &rect, img->pixels, img->width*4);
	SDL_RenderCopy(renderer, texture, NULL, NULL);

#endif
	/* this version resizes the image which is slower
	Image resized = *img;
	//resized.quantize(1); //change this line to have a more retro look
	resized.scale(window_width, window_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (1) //flip
	{
	glRasterPos2f(-1, 1);
	glPixelZoom(1, -1);
	}
	glDrawPixels( resized.width, resized.height, GL_RGBA, GL_UNSIGNED_BYTE, resized.pixels );
	*/
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
	Uint8* stream,
	int    len)
{
	static double audio_time = 0;

	memset(stream, 0, len);//clear
	if (!Game::instance)
		return;

	Game::instance->onAudio((float*)stream, len / sizeof(float), audio_time, audio_spec);
	audio_time += len / (double)audio_spec.freq;
}

void Game::enableAudio()
{
	SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
	audio_spec.freq = 48000;
	audio_spec.format = AUDIO_F32;
	audio_spec.channels = 1;
	audio_spec.samples = 1024;
	audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */
	SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(nullptr, 0, &audio_spec, nullptr, 0);
	if (!audio_device) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudioDevice(audio_device, 0);
}

void Game::onAudio(float *buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
	//fill the audio buffer using our custom retro synth
	synth.generateAudio(buffer, len, audio_spec);
}

// COSAS AÑADIDAS QUE QUIZAS NO ME SIRVEN
GameMap* loadGameMap(const char* filename) {
	using json = nlohmann::json;
	std::ifstream f(filename);
	if (!f.good())
		return nullptr;
	json jData = json::parse(f);

	int w = jData["width"];
	int h = jData["height"];
	int numLayers = jData["layers"].size();

	GameMap* map = new GameMap(w, h);
	//Allocate memory for data inside each layer
	map->layers = new sLayer[numLayers];
	map->tile_width = jData["tilewidth"];
	map->tile_height = jData["tileheight"];

	for (int l = 0; l < numLayers; l++) {
		//Allocate memory for data inside each layer
		map->layers[l].data = new sCell[w * h];
		json layer = jData["layers"][l];
		for (int x = 0; x < map->width; x++) {
			for (int y = 0; y < map->height;y++) {
				int index = x + y * map->width;
				int tileId = layer["data"][index].get<int>() - 1;
				sCell& cell = map->getCell(x, y, l);
				cell.tileId = tileId;
				if (l == 0) {
					if (tileId == 652) { //Change Id if you want
						cell.type = EMPTY;
					}
					else if (tileId == 910) { //Change Id If you want
						cell.type = WALL;
					}
				}
			}
		}
	}
	return map;
}

// MIO RITMO
void draw_rhythm_scene(Image& framebuffer)
{
	// background
	framebuffer.drawImage(run_background, 0, 0);
	framebuffer.drawImage(arrow_background, 0, 0);
	// arrow background

	framebuffer.drawImage(note_sprites, 10, 60, Area(0, 0, 17, 17));	//left arrow
	framebuffer.drawImage(note_sprites,	10, 100, Area(21, 0, 17, 17));	//down arrow
	framebuffer.drawImage(note_sprites,	10, 40, Area(42, 0, 17, 17));	//up arrow
	framebuffer.drawImage(note_sprites,	10, 80, Area(63, 0, 17, 17));	//right arrow

}

void draw_progress_bar(Image& framebuffer) {
	if (max_score <= 0)
		return;

	float progress = (float)score / max_score;

	if (progress > 1.0f)
		progress = 1.0f;

	if (progress < 0.0f)
		progress = 0.0f;

	int bar_width = (int)(progress * 158);

	framebuffer.drawRectangle(1, 35, bar_width, 3, Color::CYAN);
	framebuffer.drawImage(progress_bar, 0, 0);

	framebuffer.drawImage(runner_sprites, bar_width - 18, 4);
}

void draw_notes(Image& framebuffer, std::vector<Note>& notes, int key) {
	float start_x = 180;
	float target_x = 5;
	double travel_time = 1.0;
	double late_time = 0.3;
	double current_time = Game::instance->time - song_delay;
	for (int i = 0; i < notes.size(); i++)
	{
		Note& note = notes[i];
		if (note.completed)
			continue;
		double spawn_time = note.time - travel_time;

		if (current_time < spawn_time)
			continue;

		// tiempo despues de la nota
		if (current_time > note.time + late_time) //cambiar
			continue;

		double progress = (current_time - spawn_time) / travel_time;
		float x = start_x - progress * (start_x - target_x);

		if (key == UP) framebuffer.drawImage(note_sprites, x, 40, Area(42, 20, 17, 17));
		else if (key == LEFT) framebuffer.drawImage(note_sprites, x, 60, Area(0, 20, 17, 17));
		else if (key == RIGHT) framebuffer.drawImage(note_sprites, x, 80, Area(63, 20, 17, 17));
		else if (key == DOWN) framebuffer.drawImage(note_sprites, x, 100, Area(21, 20, 17, 17));
	}
}

void evaluate_note(double difference) {
	if (difference < 0.1) {
		score += NOTE_SCORE;
		score_message = "PERFECT!";
	}
	else if (difference < 0.2) {
		score += NOTE_SCORE / 2;
		score_message = "GOOD";
	}
	else if (difference < 0.3) {
		score += NOTE_SCORE / 10;
		score_message = "MID";
	}
	else {
		score_message = "MISS";
	}
}

void update_notes(std::vector<Note>& notes, int& current_index)
{
	double current_time = Game::instance->time - song_delay;
	const double late_time = 0.3;

	while (current_index < (int)notes.size())
	{
		Note& note = notes[current_index];

		if (note.completed) { 
			current_index++; 
		}
        else if (current_time > note.time + late_time) {
			note.completed = true;
			current_index++;
		}
		else {
			break;
		}
	}
}

int load_notes(const std::string& filename) {
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Failed to open notes file: " << filename << std::endl;
		return -1;
	}

	double time;
	std::string key;

	while (file >> time >> key) {
		Note Note = { time, false };
		if (key == "U") {
			up_notes.push_back(Note);
		}
		else if (key == "L") {
			left_notes.push_back(Note);
		}
		else if (key == "R") {
			right_notes.push_back(Note);
		}
		else if (key == "D") {
			down_notes.push_back(Note);
		}
	}

	return 1;
}