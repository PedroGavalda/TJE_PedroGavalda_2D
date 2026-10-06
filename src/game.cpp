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

//mio
double target_time = 5.0;
double difference = 0.0;
std::vector<Note> notes = {
	{3.0, SDL_SCANCODE_D, false},
	{3.5, SDL_SCANCODE_F, false},
	{4.5, SDL_SCANCODE_J, false},
	{5.0, SDL_SCANCODE_K, false}
};
int current_note = 0;
int score = 0;
std::string score_message;

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

	//enableAudio(); //enable this line if you plan to add audio to your application
	//synth.playSample("data/coin.wav",1,true);
	//synth.osc1.amplitude = 0.5;
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
		/*
		framebuffer.drawText("Time:", 0, 10, font);
		framebuffer.drawText(toString(time), 40, 10, minifont, 4, 6);

		framebuffer.drawText("Target:", 1, 20, font);
		if (current_note < notes.size()) { framebuffer.drawText(toString(notes[current_note].time),60, 25,minifont, 4, 6); }
		
		framebuffer.drawText("Difference:", 1, 30, font);
		framebuffer.drawText(toString(difference), 100, 35, minifont, 4, 6);

		framebuffer.drawText("Score:", 1, 40, font);
		framebuffer.drawText(toString(score), 60, 40, font);

		framebuffer.drawText("Rank:", 1, 50, font);
		framebuffer.drawText(score_message, 60, 50, font);
		*/
		framebuffer.drawText("Rank:", 1, 0, font);
		framebuffer.drawText(score_message, 40, 0, font);

		for (int i = 0; i < notes.size(); i++)
		{
			Note& note = notes[i];

			float start_y = 0;
			float target_y = 100;

			double travel_time = 1.0;
			double spawn_time = note.time - travel_time;
			
			if (time < spawn_time)
				continue;

			double progress = (time - spawn_time) / travel_time;

			if (progress > 1.0)
				continue;

			float y = 20 + progress * (target_y - start_y);
			float x;

			if      (note.key == SDL_SCANCODE_D) x = 15;
			else if (note.key == SDL_SCANCODE_F) x = 45;
			else if (note.key == SDL_SCANCODE_J) x = 75;
			else if (note.key == SDL_SCANCODE_K) x = 105;

			framebuffer.drawRectangle(x, y, 30, 5, 1);
		}

		framebuffer.drawLine(15, 20, 15, 100, Color::WHITE);
		framebuffer.drawLine(45, 20, 45, 100, Color::WHITE);
		framebuffer.drawLine(75, 20, 75, 100, Color::WHITE);
		framebuffer.drawLine(105, 20, 105, 100, Color::WHITE);
		framebuffer.drawLine(135, 20, 135, 100, Color::WHITE);
		
		framebuffer.drawLine(15, 100, 135, 100, Color::GRAY);
	//send image to screen
	showFramebuffer(&framebuffer);
}

void Game::update(double seconds_elapsed)
{
	//Add here your update method
	//...

	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) //if key up
	{
	}
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) //if key down
	{
	}

	if (current_note >= notes.size())
	{
		return;
	}
	
	Note& note = notes[current_note];

	if (Input::wasKeyPressed(SDL_SCANCODE_D) ||
		Input::wasKeyPressed(SDL_SCANCODE_F) ||
		Input::wasKeyPressed(SDL_SCANCODE_J) ||
		Input::wasKeyPressed(SDL_SCANCODE_K)) {

		if (Input::wasKeyPressed(note.key)) {
			difference = fabs(time - note.time);
			if (difference < 0.2) {
				score += 100;
				score_message = "PERFECT!";
			}
			else if (difference < 0.3) {
				score += 50;
				score_message = "GOOD";
			}
			else if (difference < 0.5) {
				score += 10;
				score_message = "MID";
			}
			else {
				score_message = "MISS";
			}
			note.completed = true;
			current_note++;
		}
		else {
			note.completed = true;
			current_note++;
			score_message = "ERROR";
		}
		
	}
	//example of 'was pressed'
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