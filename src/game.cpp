#include "game.h"
#include "utils.h"
#include "input.h"
#include "image.h"
#include "json.hpp"
#include <fstream>

#include <cmath>

Game* Game::instance = NULL;

Image font;
Image minifont;
Image sprite;
Color bgcolor(130, 80, 100);

// MIO RITMO

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

#define NOTE_SCORE 100

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

GameState game_state = MENU;
Song song;
std::vector<Song> songs = {
	{ "Mario 1-1", "data/mario.txt", "data/mario3.wav" },
};
int selected_song_index = 0;
double song_delay = 3.0;
double countdown_start = 0.0;
const double COUNTDOWN_DURATION = 3.0;
bool music_started = false;
int note_count;
int max_score;

float police_score = 0.0f;
const double POLICE_DELAY = 5.0;
int animation_index = 0;

// RITMO ASSETS
Image title;
Image title_miku;
Image note_sprites;
Image runner_sprites;
Image progress_bar;
Image arrow_background;
Image run_background;
Image police_sprites;

Synth::Sample* mario_sample = nullptr;
Synth::Sample* hit_sample = nullptr;
Synth::SamplePlayback* song_playback = nullptr;


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
	title.load("data/title.tga");
	title_miku.load("data/mikuPS2/0001.png");
	note_sprites.load("data/arrows.tga");
	runner_sprites.load("data/miku_run.tga");
	police_sprites.load("data/police.tga");
	progress_bar.load("data/progress_bar.tga");
	arrow_background.load("data/arrow_background.tga");
	run_background.load("data/run_background.png");

	enableAudio(); //enable this line if you plan to add audio to your application
	mario_sample = synth.loadSample("data/mario3.wav");
	hit_sample = synth.loadSample("data/mouseclick.wav");
	//.osc1.amplitude = 0.5;

	init_song(songs[selected_song_index]);
}

//what to do when the image has to be draw
void Game::render(void)
{
	Image framebuffer(160, 120); //do not change framebuffer size

	if (game_state == MENU) {
		framebuffer.fill(Color::BLACK);
		framebuffer.drawImage(title, 0, 0);
		framebuffer.drawImage(title_miku, 80, 60);
		framebuffer.drawText("PRESS X TO START", 35, 50, font);
	}
	else if (game_state == PLAYING)
	{
		draw_rhythm_scene(framebuffer);
		draw_notes(framebuffer, up_notes, UP);
		draw_notes(framebuffer, left_notes, LEFT);
		draw_notes(framebuffer, down_notes, DOWN);
		draw_notes(framebuffer, right_notes, RIGHT);
		draw_progress_bar(framebuffer);
	}
	else if (game_state == GAME_OVER)
	{
		framebuffer.drawText("GAME OVER", 35, 25, font);
		framebuffer.drawText("SCORE: " + std::to_string(score),35, 45, font);
		framebuffer.drawText("X: PLAY AGAIN", 35, 65, font);
		framebuffer.drawText("Z: MENU", 35, 80, font);
	}
	else if (game_state == SONG_COMPLETE) {
		framebuffer.fill(Color::BLACK);
		framebuffer.drawText("SONG COMPLETE!", 25, 25, font);
		framebuffer.drawText("SCORE: " + std::to_string(score),35, 50, font);
		framebuffer.drawText("X: PLAY AGAIN", 30, 75, font);
		framebuffer.drawText("Z:  MENU", 30, 90, font);
	}

	//send image to screen
	showFramebuffer(&framebuffer);
}

void Game::update(double seconds_elapsed)
{
	//Add here your update method
	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	
	if (game_state == MENU) {
		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			if (init_song(songs[selected_song_index])) {
				countdown_start = time;
				song_delay = time + COUNTDOWN_DURATION;
				game_state = PLAYING;
			}
		}
		return;
	}
	
	if (game_state == SONG_COMPLETE) {
		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			if (init_song(songs[selected_song_index])) {
				countdown_start = time;
				song_delay = time + COUNTDOWN_DURATION;
				game_state = PLAYING;
			}
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_Z)) {
			game_state = MENU;
		}

		return;
	}

	if (game_state == GAME_OVER) {
		if (song_playback != nullptr) {
			song_playback->stop();
			song_playback = nullptr;
		}

		music_started = false;

		if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			if (init_song(songs[selected_song_index])) {
				countdown_start = time;
				song_delay = time + COUNTDOWN_DURATION;
				game_state = PLAYING;
			}
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_Z)) {
			game_state = MENU;
		}

		return;
	}

	if (game_state != PLAYING)
		return;

	if (!music_started && time >= song_delay)
	{	
		song_playback = synth.playSample(mario_sample, 1.0f, false);
		//synth.playSample(mario_sample, 1.0f, false);
		music_started = true;
	}

	update_notes(left_notes, current_left);
	update_notes(down_notes, current_down);
	update_notes(up_notes, current_up);
	update_notes(right_notes, current_right);

	if (current_left >= left_notes.size() && current_down >= down_notes.size() && current_up >= up_notes.size() && current_right >= right_notes.size()) {	
		game_state = SONG_COMPLETE;
		return;
	}

	check_note(left_notes, current_left, SDL_SCANCODE_LEFT);
	check_note(down_notes, current_down, SDL_SCANCODE_DOWN);
	check_note(up_notes, current_up, SDL_SCANCODE_UP);
	check_note(right_notes, current_right, SDL_SCANCODE_RIGHT);

	// podria evitar duplicidad aqui
	if (Input::wasKeyPressed(SDL_SCANCODE_LEFT) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT) || Input::wasKeyPressed(SDL_SCANCODE_UP) || Input::wasKeyPressed(SDL_SCANCODE_DOWN))
	{
		synth.playSample(hit_sample, 0.5f, false);
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
	float police_progress = (float)police_score / max_score;

	if (progress > 1.0f) progress = 1.0f;
	if (police_progress > 1.0f) police_progress = 1.0f;
	if (progress < 0.0f) progress = 0.0f;
	if (police_progress < 0.0f) police_progress = 0.0f;

	int bar_width = (int)(progress * 158);
	int police_width = (int)(police_progress * 158);

	framebuffer.drawRectangle(1, 35, bar_width, 3, Color::CYAN);
	framebuffer.drawRectangle(1, 35, police_width, 3, Color::GREEN);
	framebuffer.drawImage(progress_bar, 0, 0);

	framebuffer.drawImage(runner_sprites, bar_width - 18, 4, Area(18 * animation_index, 0, 18, 30));
	framebuffer.drawImage(police_sprites, police_width - 38, 4 - animation_index);
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
		police_score += NOTE_SCORE;
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
			if (current_time > POLICE_DELAY)  police_score += NOTE_SCORE;
			current_index++;
		}
		else {
			break;
		}
	}

	if (police_score > score) {
		game_state = GAME_OVER;
	}
}


void check_note(std::vector<Note>& notes, int& current_index, SDL_Scancode key)
{
	if (!Input::wasKeyPressed(key)) return;

	if (current_index >= (int)notes.size()) return;

	Note& note = notes[current_index];
	double current_time = Game::instance->time - song_delay;
	double difference = fabs(note.time - (current_time));
	animation_index = 1 - animation_index;

	if (difference <= 0.3) {
		evaluate_note(difference);
		note.completed = true;
		if (current_time > POLICE_DELAY) police_score += NOTE_SCORE * (1.0f - (float)(difference / 0.3f));
		current_index++;
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

bool init_song(const Song& selected_song) {
	song = selected_song;

	up_notes.clear();
	left_notes.clear();
	right_notes.clear();
	down_notes.clear();

	current_up = 0;
	current_left = 0;
	current_right = 0;
	current_down = 0;

	score = 0;
	police_score = 0;
	score_message = "";
	difference = 0.0;
	music_started = false;

	if (load_notes(song.notes_direction) < 0) {
		std::cerr << "ERROR loading notes for " << song.title << std::endl;
		return false;
	}

	note_count = left_notes.size() + down_notes.size() + up_notes.size() +right_notes.size();
	max_score = note_count * NOTE_SCORE;

	return true;
}