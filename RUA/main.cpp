#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<Windows.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define LVL_WIDTH	1280
#define LVL_HEIGHT	960
#define UNICORN_SIZE 40
#define EASY_CONTROLS false
#define NORMAL_CONTROLS true
#define TIME 300
#define MAXRAND 6
#define SMALLRAND 1
#define MAX_JUMPS 2
#define LIVES 3
#define DASH_SPEED 5
#define BOOST 100
#define RANGE 170
#define UP_SPEED 1
#define DOWN_SPEED 1
#define GROUND_SPEED 1
#define INFINITE_START 320
#define SPACE_LIVES 60
#define GROUND_DIST 150
#define POS_G3 4
#define POS_G4 1.5


typedef struct sprite
{
	SDL_Surface* surface;
	int x;
	int y;
};


// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};



// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


//defines a sprite position 
sprite defineSprite(sprite object, SDL_Surface* surface, int posx, int posy)
{
	object.surface = surface;
	object.x = posx;
	object.y = posy;
	return object;
}


//changes the controls of the game
void controlChange(bool* changeControls)
{
	if (*changeControls == true)
	{
		*changeControls = false;
	}
	else
	{
		*changeControls = true;
	}
}


//initiates the jump sequence
void jump(sprite* Object, bool roundEnd, bool* down)
{
	if ((*Object).y > -100 && roundEnd == false)
	{
		(*Object).y = (*Object).y + 10;
		if ((*Object).y <= -100) {
			*down = true;
		}
	}
}


//For infinite effect of grounds without obstacles 
void infiniteEffect1(sprite* Ground, SDL_Surface* ground, int infinite_start)
{
	if ((*Ground).x < -ground->w / 2) {
		(*Ground).x = SCREEN_WIDTH + infinite_start;
	}
}


//For infinite effect of grounds with obstacles 
void infiniteEffect2(sprite* Ground, SDL_Surface* ground, sprite* Obstacle, int infinite_start)
{
	if ((*Ground).x < -ground->w / 2) {
		(*Obstacle).x = SCREEN_WIDTH + infinite_start;
		(*Ground).x = SCREEN_WIDTH + infinite_start;
	}
}


//creates the rectangle boundaries for sprites
SDL_Rect createBox(sprite Sprite, SDL_Surface* surface, SDL_Rect box)
{
	box.w = surface->w;
	box.h = surface->h;
	box.x = Sprite.x - box.w / 2;
	box.y = Sprite.y;
	return box;
}


//function to generate random number between 1-7, for effect of a randomly generated game
int randomNumber()
{
	srand(time(NULL));
	int randomNo;
	randomNo = rand() % MAXRAND + SMALLRAND;
	return randomNo;
}


//stops the game after collision with an Obstacle
void ObjectCollision(sprite* Unicorn, sprite* Obstacle, bool collisionO, bool* roundEnd, bool* down, bool* roll, double* finalTime, double worldTime)
{
	if (collisionO == true && *roundEnd == false)
	{
		*roundEnd = true;
		*down = false;
		//(*Unicorn).y = (*Obstacle).y -UNICORN_SIZE+1;
		(*Unicorn).x = (*Obstacle).x - UNICORN_SIZE + 1;
		*roll = false;
		*finalTime = worldTime;
	}
}


void printDetails(bool collisionO, bool collisionOs, char* text, double worldTime, double fps, SDL_Surface* screen, SDL_Surface* charset)
{
	if (collisionO == false && collisionOs == false) {
		sprintf(text, "Robot Unicorn Attack, elapsed time = %.1lf s  %.0lf frames / s", worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
	}
}


//Resets the games values after losing a life
void resetGame(bool* roundEnd, bool* collisionO, bool* collisionOs, bool* collisionTerrain, int* t1, int* t2, double* worldTime, sprite* Unicorn, sprite* Lives, sprite* Ground, sprite* Ground2, sprite* Ground3, sprite* Ground4, sprite* Obstacle, sprite* ObstacleS, SDL_Surface* ground2, int groundy)
{
	*roundEnd = false;//resets these values
	*collisionO = false;
	*collisionOs = false;
	*collisionTerrain = false;

	*t1, * t2, * worldTime = 0;//Timers reset

	(*Unicorn).x = UNICORN_SIZE;
	(*Lives).x = UNICORN_SIZE;
	(*Ground).x = SCREEN_WIDTH / 2;
	(*Ground2).x = (*Ground).x + 2 * SCREEN_WIDTH - GROUND_DIST;
	(*Ground3).x = randomNumber() * (*Ground).x + 3 * SCREEN_WIDTH;
	(*Ground4).x = randomNumber() * (*Ground).x + 3 * SCREEN_WIDTH;
	(*Obstacle).x = (*Ground2).x;
	(*ObstacleS).x = (*Ground4).x;

	(*Unicorn).y = SCREEN_HEIGHT / 2 + UNICORN_SIZE;//back to og position
	(*Lives).y = UNICORN_SIZE;
	(*Ground).y = groundy;
	(*Ground2).y = groundy;
	(*Ground3).y = groundy - SCREEN_HEIGHT / POS_G3;
	(*Ground4).y = groundy - SCREEN_HEIGHT / POS_G4;
	(*Obstacle).y = (*Unicorn).y;
	(*ObstacleS).y = (*Ground4).y + ground2->h;
}


//Collision checks for the bottomest grounds and keeps unicorn above them
void collisionBasicGround(bool collisionGround, sprite* Unicorn, sprite Ground, bool* down, int* count)
{
	if (collisionGround == true && (*Unicorn).y >= Ground.y - UNICORN_SIZE)
	{
		(*Unicorn).y = Ground.y - UNICORN_SIZE + 1;
		*down = false;
		*count = 0;
	}
}


//Keeps the unicorn above the ground for the floating Grounds
void collisionTop(bool collisionGround, sprite* Unicorn, sprite Ground, bool* down, int* count)
{
	if (collisionGround == true && (*Unicorn).y < Ground.y)
	{
		(*Unicorn).y = Ground.y - UNICORN_SIZE + 1;
		*down = false;
		*count = 0;
	}
}


//Checks for collision with the terrain sides
void collisionGrounds(bool collisionGround, sprite Unicorn, sprite Ground, bool* collisionTerrain, bool* roundEnd, bool* roll, bool* down, double* finalTime, double worldTime, int* count)
{
	if (collisionGround == true && (Unicorn.y == Ground.y || Unicorn.y > Ground.y))
	{
		*collisionTerrain = true;
		*roundEnd = true;
		*roll = false;
		*down = false;
		*finalTime = worldTime;
		*count = 0;
	}
}


//Checks the controls of the game and sets the ground state
void controls(bool* changeControls, bool* roll)
{
	if (*changeControls == NORMAL_CONTROLS)
	{
		*roll = true;
	}
	else {
		*roll = false;
	}
}

//Starts the movement of the ground, for infinity
//Dash is for the boost situation
void rollDash(bool roll, double delta, bool* dash, sprite* Ground, sprite* Ground2, sprite* Ground3, sprite* Ground4, sprite* Obstacle, sprite* ObstacleS, int* count, sprite* Unicorn, sprite* Lives)
{
	if (roll == true && (int)delta % 2 == 0)
	{
		if (*dash == false) {
			(*Ground).x-=GROUND_SPEED;
			(*Ground2).x-= GROUND_SPEED;
			(*Ground3).x-= GROUND_SPEED;
			(*Ground4).x-= GROUND_SPEED;
			(*Obstacle).x-= GROUND_SPEED;
			(*ObstacleS).x-= GROUND_SPEED;
		}
		if (*dash == true)
		{
			*count = 1;
			if ((*Unicorn).x > UNICORN_SIZE)
			{
				(*Ground).x -= DASH_SPEED;
				(*Ground2).x -= DASH_SPEED;
				(*Ground3).x -= DASH_SPEED;
				(*Ground4).x -= DASH_SPEED;
				(*Obstacle).x -= DASH_SPEED;
				(*ObstacleS).x -= DASH_SPEED;
				(*Lives).x--;
				(*Unicorn).x--;
			}
			else {
				*dash = false;
			}
		}
	}
}


//Moves the grounds and obstacles back to og position
void moveDown(bool* down, double delta, sprite* Ground, sprite* Ground2, sprite* Ground3, sprite* Ground4, sprite* Obstacle, sprite* ObstacleS)
{
	if (*down == true && (int)delta % 2 == 0)
	{
		if ((*Ground).y > LVL_HEIGHT)
		{
			(*Ground).y = SCREEN_HEIGHT / 2 + 2 * UNICORN_SIZE;
			*down = false;
		}
		else {
			(*Ground).y -= DOWN_SPEED;
			(*Ground2).y -= DOWN_SPEED;
			(*Ground3).y -= DOWN_SPEED;
			(*Ground4).y -= DOWN_SPEED;
			(*Obstacle).y -= DOWN_SPEED;
			(*ObstacleS).y -= DOWN_SPEED;
		}
	}
}


void moveUP(bool* up, bool* down, double delta, bool* onepress, int* range, sprite* Ground, sprite* Ground2, sprite* Ground3, sprite* Ground4, sprite* Obstacle, sprite* ObstacleS)
{
	if (*up == true && (int)delta % 2 == 0)
	{

		if (*onepress == false)//So as to get range only once
		{
			*range = (*Ground).y + RANGE;//Max range at which unicorn can jump
			if (*range < UNICORN_SIZE)
			{
				*down = true;
				*up = false;
			}
			*onepress = true;
		}
		if ((*Ground).y > *range) {
			*up = false;
			*down = true;
		}
		(*Ground).y += UP_SPEED;
		(*Ground2).y += UP_SPEED;
		(*Ground3).y += UP_SPEED;
		(*Ground4).y += UP_SPEED;
		(*Obstacle).y += UP_SPEED;
		(*ObstacleS).y += UP_SPEED;
	}
}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int t1, t2, quit, frames, rc, time, tk1, tk2;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	double finalTime = 0;
	int range, count = 0;//count is used to limit the max times unicorn can jump
	int newgame = 0;
	int groundy = SCREEN_HEIGHT / 2 + 2 * UNICORN_SIZE, groundx = SCREEN_WIDTH / 2, loop = 1, unicornx = UNICORN_SIZE, unicorny = SCREEN_HEIGHT / 2 + UNICORN_SIZE + 1;
	int no_lives = LIVES, livesx = UNICORN_SIZE, livesy = UNICORN_SIZE;
	bool collisionTerrain = false;//Checks for collision of terrain, used for lives
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface* unicorn;
	SDL_Surface* ground;
	SDL_Surface* ground2;
	SDL_Surface* obstacle;
	SDL_Surface* obstacleS;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("printf output goes here\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}


	// fullscreen mode
	/*rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
									 &window, &renderer);*/
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");//for scaling quality
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);//sets a device independent resolution for rendering.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//set the color used for drawing operations

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	const SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "RESTART" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "QUIT" },
	};
	const SDL_MessageBoxButtonData buttons2[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "NEW GAME" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "QUIT" },
	};

	const SDL_MessageBoxColorScheme colorScheme = {
		   { /* .colors (.r, .g, .b) */
			   /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
			   { 255,   0,   0 },
			   /* [SDL_MESSAGEBOX_COLOR_TEXT] */
			   {   0, 255,   0 },
			   /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
			   { 255, 255,   0 },
			   /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
			   {   0,   0, 255 },
			   /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
			   { 255,   0, 255 }
		   }
	};

	const SDL_MessageBoxData messageboxData = {
			SDL_MESSAGEBOX_INFORMATION, /* .flags */
			NULL, /* .window */
			"WASTED", /* .title */
			"You died, choose Restart if you want to continue", /* .message */
			SDL_arraysize(buttons), /* .numbuttons */
			buttons, /* .buttons */
			&colorScheme /* .colorScheme */
	};

	const SDL_MessageBoxData messageboxDataMenu = {
			SDL_MESSAGEBOX_INFORMATION, /* .flags */
			NULL, /* .window */
			"MENU", /* .title */
			"Welcome To Robot Unicorn Attack", /* .message */
			SDL_arraysize(buttons2), /* .numbuttons */
			buttons2, /* .buttons */
			&colorScheme /* .colorScheme */
	};

	// wy≥πczenie widocznoúci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	unicorn = SDL_LoadBMP("./unicorn.bmp");
	if (unicorn == NULL) {
		printf("SDL_LoadBMP(unicorn.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	ground = SDL_LoadBMP("./groundLong.bmp");
	if (ground == NULL) {
		printf("SDL_LoadBMP(groundLong.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(unicorn);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	ground2 = SDL_LoadBMP("./groundLong.bmp");
	if (ground2 == NULL) {
		printf("SDL_LoadBMP(groundLong.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(unicorn);
		SDL_FreeSurface(ground);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	obstacle = SDL_LoadBMP("./obstacle.bmp");
	if (obstacle == NULL) {
		printf("SDL_LoadBMP(obstacle.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(unicorn);
		SDL_FreeSurface(ground);
		SDL_FreeSurface(ground2);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	obstacleS = SDL_LoadBMP("./Stalactite.bmp");
	if (obstacleS == NULL) {
		printf("SDL_LoadBMP(Stalactite.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(unicorn);
		SDL_FreeSurface(ground);
		SDL_FreeSurface(ground2);
		SDL_FreeSurface(obstacle);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	sprite Unicorn{};
	Unicorn = defineSprite(Unicorn, unicorn, unicornx, unicorny);

	sprite Lives{};
	Lives = defineSprite(Lives, unicorn, livesx, livesy);

	sprite Ground{};
	Ground = defineSprite(Ground, ground, groundx, groundy);

	sprite Ground2{};
	Ground2 = defineSprite(Ground2, ground2, groundx + 2 * SCREEN_WIDTH - GROUND_DIST, groundy);

	sprite Ground3{};
	Ground3 = defineSprite(Ground3, ground2, groundx + 3 * SCREEN_WIDTH, groundy - SCREEN_HEIGHT / POS_G3);

	sprite Ground4{};
	Ground4 = defineSprite(Ground4, ground2, groundx + 2 * SCREEN_WIDTH, groundy - SCREEN_HEIGHT / POS_G4);

	sprite Obstacle{};
	Obstacle = defineSprite(Obstacle, obstacle, groundx + 2 * SCREEN_WIDTH, unicorny);

	sprite ObstacleS{};
	ObstacleS = defineSprite(ObstacleS, obstacleS, groundx + 2 * SCREEN_WIDTH, groundy - SCREEN_HEIGHT / POS_G4 + ground2->h);



	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int pink = SDL_MapRGB(screen->format, 0xFF, 0xC0, 0xCB);

	t1 = SDL_GetTicks();//Returns an unsigned 32-bit value representing the number of milliseconds since the SDL library initialized.

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;
	time = 0;

	bool roll = false;//for movement of the floor
	bool down = false;//for gravity effect
	bool roundEnd = false;//checks for the end of the round
	bool changeControls = false;//for changing controls with d
	bool onepress = false;
	bool up = false;//Allows the unicorn to go up
	bool dash = false;//used to move the ground faster



	while (!quit) {
		t2 = SDL_GetTicks();

		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += etiSpeed * delta;

		//Starts the movement of the ground, for infinity
		//Dash is for the boost situation
		rollDash(roll, delta, &dash, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS, &count, &Unicorn, &Lives);

		//Moves the grounds and obstacles back to og position
		moveDown(&down, delta, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS);


		//Moves Obstacles and Grounds
		moveUP(&up, &down, delta, &onepress, &range, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS);


		SDL_FillRect(screen, NULL, pink);


		DrawSurface(screen, Unicorn.surface, Unicorn.x, Unicorn.y);

		//Draws 2 Grounds
		DrawSurface(screen, Ground.surface, Ground.x, Ground.y);
		DrawSurface(screen, Ground2.surface, Ground2.x, Ground.y);
		DrawSurface(screen, Ground3.surface, Ground3.x, Ground3.y);
		DrawSurface(screen, Ground4.surface, Ground4.x, Ground4.y);

		//Draws the lives
		int dist = 0;
		for (int i = 0; i < no_lives; i++)
		{
			DrawSurface(screen, Lives.surface, Lives.x + dist, Lives.y);
			dist += SPACE_LIVES;
		}
		//Draws obstacles
		DrawSurface(screen, Obstacle.surface, Obstacle.x, Obstacle.y);
		DrawSurface(screen, ObstacleS.surface, ObstacleS.x, ObstacleS.y);

		//Adds infinite ground effect
		int infinite_start = INFINITE_START;
		infiniteEffect1(&Ground, ground, infinite_start);
		infiniteEffect1(&Ground3, ground2, infinite_start * 2);

		infiniteEffect2(&Ground2, ground2, &Obstacle, infinite_start);
		infiniteEffect2(&Ground4, ground2, &ObstacleS, infinite_start * 3);
		//fps
		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);

		SDL_Rect boxU{}, boxO{}, boxOs{}, boxG{}, boxG2{}, boxG3{}, boxG4{}, result;
		//box for Unicorn
		boxU = createBox(Unicorn, unicorn, boxU);
		//box for obstacle
		boxO = createBox(Obstacle, obstacle, boxO);
		boxOs = createBox(ObstacleS, obstacleS, boxOs);

		//boxes for grounds
		boxG = createBox(Ground, ground, boxG);
		boxG2 = createBox(Ground2, ground2, boxG2);
		boxG3 = createBox(Ground3, ground2, boxG3);
		boxG4 = createBox(Ground4, ground2, boxG4);

		bool collisionO = SDL_IntersectRect(&boxU, &boxO, &result);//colision with obstacles
		bool collisionOs = SDL_IntersectRect(&boxU, &boxOs, &result);//colision with Stalactites
		bool collisionG = SDL_IntersectRect(&boxU, &boxG, &result);//collision with ground
		bool collisionG2 = SDL_IntersectRect(&boxU, &boxG2, &result);//collision with ground2
		bool collisionG3 = SDL_IntersectRect(&boxU, &boxG3, &result);//collision with floating ground3
		bool collisionG4 = SDL_IntersectRect(&boxU, &boxG4, &result);//collision with floating ground4



		//Collision checks
		collisionBasicGround(collisionG, &Unicorn, Ground, &down, &count);
		collisionBasicGround(collisionG2, &Unicorn, Ground2, &down, &count);

		collisionTop(collisionG3, &Unicorn, Ground3, &down, &count);
		collisionTop(collisionG4, &Unicorn, Ground4, &down, &count);

		collisionGrounds(collisionG3, Unicorn, Ground3, &collisionTerrain, &roundEnd, &roll, &down, &finalTime, worldTime, &count);
		collisionGrounds(collisionG4, Unicorn, Ground4, &collisionTerrain, &roundEnd, &roll, &down, &finalTime, worldTime, &count);

		//Adds gravity in case no collision
		if (collisionG3 == false && collisionG == false && collisionG2 == false && collisionG4 == false && up != true && changeControls == NORMAL_CONTROLS)
		{
			down = true;
		}

		ObjectCollision(&Unicorn, &Obstacle, collisionO, &roundEnd, &down, &roll, &finalTime, worldTime);
		ObjectCollision(&Unicorn, &ObstacleS, collisionOs, &roundEnd, &down, &roll, &finalTime, worldTime);

		if (roundEnd == true && (collisionO == true || collisionOs == true || collisionTerrain == true)) {
			sprintf(text, "Robot Unicorn Attack, end time = %.1lf s  %.0lf frames / s", finalTime, fps);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		}

		//Displays pop up box in scenario of Death
		if ((collisionO == true || collisionOs == true || collisionTerrain == true || Ground.y < -LVL_HEIGHT) && no_lives > 0)
		{
			no_lives--;
			int buttonid;
			if (SDL_ShowMessageBox(&messageboxData, &buttonid) < 0) {
				SDL_Log("error displaying message box");
				return 1;
			}
			if (buttonid == -1) {
				SDL_Log("no selection");
			}
			if (buttonid == 0)
			{
				SDL_Delay(TIME);//game pauses for specified time
				controls(&changeControls, &roll);
				resetGame(&roundEnd, &collisionO, &collisionOs, &collisionTerrain, &t1, &t2, &worldTime, &Unicorn, &Lives, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS, ground2, groundy);
			}
			if (buttonid == 1)
			{
				quit = 1;
			}
		}
		if (no_lives < 1)
		{
			SDL_FillRect(screen, NULL, czarny);
			int buttonid2;
			if (SDL_ShowMessageBox(&messageboxDataMenu, &buttonid2) < 0) {
				SDL_Log("error displaying message box");
				return 1;
			}
			if (buttonid2 == -1) {
				SDL_Log("no selection");
			}
			if (buttonid2 == 0)
			{
				SDL_Delay(TIME);//game pauses for specified time
				controls(&changeControls, &roll);
				resetGame(&roundEnd, &collisionO, &collisionOs, &collisionTerrain, &t1, &t2, &worldTime, &Unicorn, &Lives, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS, ground2, groundy);
				no_lives = LIVES;
			}
			if (buttonid2 == 1)
			{
				quit = 1;
			}
		}

		printDetails(collisionO, collisionOs, text, worldTime, fps, screen, charset);
		//printDetails(collisionOs, text, worldTime, fps, screen, charset);

		sprintf(text, "Esc - exit, \030 - To start, \031 - slower");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL); //copies a portion of the texture to the current rendering target.
		SDL_RenderPresent(renderer);//updates the screen with any rendering performed since the previous call.

		//tk1 = SDL_GetTicks();


		// handling of events (if there were any)
		if (SDL_PollEvent(&event)) {

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = 1;
					break;
				case SDLK_d:
					controlChange(&changeControls);
					if (changeControls == NORMAL_CONTROLS)
					{
						roll = true;
					}
					else {
						roll = false;
					}
					break;
				case SDLK_n:
				{
					SDL_Delay(TIME);//game pauses for specified time
					controls(&changeControls, &roll);
					resetGame(&roundEnd, &collisionO, &collisionOs, &collisionTerrain, &t1, &t2, &worldTime, &Unicorn, &Lives, &Ground, &Ground2, &Ground3, &Ground4, &Obstacle, &ObstacleS, ground2, groundy);

					break;
				}
				case  SDLK_UP:
				{
					if (changeControls == EASY_CONTROLS) {
						//jump(&Unicorn, roundEnd, &down);
						jump(&Ground, roundEnd, &down);
						jump(&Ground2, roundEnd, &down);
						jump(&Ground3, roundEnd, &down);
						jump(&Ground4, roundEnd, &down);
						jump(&Obstacle, roundEnd, &down);
						jump(&ObstacleS, roundEnd, &down);
					}
					break;
				}
				case SDLK_RIGHT:
				{
					if (changeControls == EASY_CONTROLS) {
						if (roundEnd == false) {
							roll = true;
						}
					}
					break;
				}
				case SDLK_LEFT:
				{
					if (changeControls == EASY_CONTROLS) {
						roll = false;
					}
					break;
				}
				case SDLK_z:
				{
					if (changeControls == NORMAL_CONTROLS && roundEnd == false)
					{
						count++;
						if (count > MAX_JUMPS)
						{
							count = 0;
							down = true;
							up = false;
							break;
						}
						down = false;
						up = true;
					}
					break;
				}
				case SDLK_x:
					if (changeControls == NORMAL_CONTROLS)
					{
						Unicorn.x += BOOST;
						Lives.x += BOOST;
						dash = true;
					}
					break;
				}
			}
			if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym)
				{
				case SDLK_UP: {
					if (changeControls == EASY_CONTROLS && (collisionO == false || collisionOs == false || collisionTerrain == false)) {
						down = true;
					}
					break;
				}
				case SDLK_RIGHT:
				{
					if (changeControls == EASY_CONTROLS) {
						roll = false;
					}
					break;
				}
				case SDLK_z: {
					onepress = false;

					break;
				}
				}
			}

			if (event.type == SDL_QUIT) {
				quit = 1;
			}
		};
		frames++;
	};



	// freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(unicorn);
	SDL_FreeSurface(ground);
	SDL_FreeSurface(ground2);
	SDL_FreeSurface(obstacle);
	SDL_FreeSurface(obstacleS);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}
