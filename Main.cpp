//SDL and system libraries
#include <SDL_mixer.h>
#include <iostream>
#include <assert.h>
#include <stddef.h>
//local classes
#include "button.h"
#include "Texture.h"
#include "timer.h"
#include "particle.h"

//Particle count
const int TOTAL_PARTICLES = 200;
//number of dialog lines
const int TOTAL_SCRIPTS = 8;
//number of dialog pages
const int TOTAL_PAGES = 8;

const int TOTAL_DATA = 3;
//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
//buttons on the title screen + back button for screens like options
const int TOTAL_BUTTONS = 6;
//number of game states
const int TOTAL_STATES = 5;
const int SCREEN_FPS=60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

const int TAO_ANIMATION_FRAMES = 8;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window renderer
SDL_Renderer* renderer = NULL;

//The window we'll be rendering to
SDL_Window* window = NULL;

//Scene textures
Texture title;
Texture titleTexture;
Texture creditsTexture;
Texture optionsTexture;
Texture loadGameTexture;
Texture chapterSelectTexture;
Texture chapter1BG;
Texture chapter1BG2;
Texture chapter1BG3;
Texture chapter1BG4;
Texture dialogBox;

//Script Textures
Texture scriptTexture[TOTAL_PAGES][TOTAL_SCRIPTS];

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

TTF_Font *font = NULL;
//Rendered texture
Texture textTexture;
Texture FPSTextTexture;

//Buttons objects
button buttons[ TOTAL_BUTTONS ];

//animation textures
Texture tao[TAO_ANIMATION_FRAMES];

//SDL_Rect tempRect = {0,0,0,0};

//file read/write stuff
Sint32 gData[ TOTAL_DATA ];
SDL_RWops* file = SDL_RWFromFile( "savegame/save.gsf", "r+b" );
int chapter1complete;
int currentPage;
int currentScript;

//particle objects
Particle particles[ TOTAL_PARTICLES ];

void renderParticles();

int gameState;

void renderParticles(){
//Go through particles
for( int i = 0; i < TOTAL_PARTICLES; ++i )
{
    //Delete and replace dead particles
    if( particles[i].isDead() )
    {
        particles[i].createParticle(renderer);
    }
}

//Show particles
for( int i = 0; i < TOTAL_PARTICLES; ++i )
{
    particles[i].render(renderer,particles[i].renderColor);
}

}

bool init()
{

	//Initialization flag
	bool success = true;
	gameState = 0;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0  )
	{
		printf( "\n SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "\n Warning: Linear texture filtering not enabled!" );
		}

		window = SDL_CreateWindow( "Go Homeless!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "\n Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
			if( renderer == NULL )
			{
				printf( "\n Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				for(int i=0;i<TOTAL_PARTICLES;i++){
                    particles[i].createParticle(renderer);
                }

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "\n SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}

        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
            {
                printf( "\n SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

        if( TTF_Init() == -1 )
        {
            printf( "\n SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }

	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	buttons[0].buttonName="back";
	buttons[1].buttonName="new";
	buttons[2].buttonName="load";
	buttons[3].buttonName="options";
	buttons[4].buttonName="credits";
	buttons[5].buttonName="chapter1";

//chapter 1, page 1
	scriptTexture[0][0].scriptString.str( "5 years ago, I stopped working regular jobs." );
	scriptTexture[0][1].scriptString.str("Since then I've mostly begged for money on the street corner");
	scriptTexture[0][2].scriptString.str("with cardboard signs that say things like 'Anything Helps'.");
    scriptTexture[0][3].scriptString.str("I stopped working because I noticed a few major problems with");
    scriptTexture[0][4].scriptString.str("our socio-economic system.  self-driving cars were commercially viable,");
    scriptTexture[0][5].scriptString.str("but the state of California red-taped self driving cars. It made sense");
    scriptTexture[0][6].scriptString.str("why... it would greatly affect the need for labor in the workforce.");
    scriptTexture[0][7].scriptString.str("                                Press LMB or Enter to continue...");
//chapter 1, page 2
    scriptTexture[1][0].scriptString.str("If the car is driving, the person behind the wheel [but why tf");
    scriptTexture[1][1].scriptString.str("would there be a wheel?] can't be held liable, which makes the car driving");
    scriptTexture[1][2].scriptString.str("system responsible, but their cars drive better than humans, so that");
    scriptTexture[1][3].scriptString.str("won't be much of a problem.  They would probably insure their own");
    scriptTexture[1][4].scriptString.str("cars, which would reduce auto insurance agents.  Besides that, ");
    scriptTexture[1][5].scriptString.str("it's hard to find probable cause when the car is driving, which");
    scriptTexture[1][6].scriptString.str("reduces the need for police on the streets.");
    scriptTexture[1][7].scriptString.str("                                Press LMB or Enter to continue...");
//chapter 1, page 3
    scriptTexture[2][0].scriptString.str("when self-driving cars are freely allowed, the need for cars drops.");
    scriptTexture[2][1].scriptString.str("One car can be used by an entire family by summoning it as needed");
    scriptTexture[2][2].scriptString.str("when not in active use.  But mainly, drivers would lose jobs, taxi");
    scriptTexture[2][3].scriptString.str("drivers, gig-economy transportation drivers, and big-rig truckers.");
    scriptTexture[2][4].scriptString.str("Self-driving cars would require a black box like airplanes for liability");
    scriptTexture[2][5].scriptString.str("as well as bug fixes.  The goal is 100% safety on the road. 30,000");
    scriptTexture[2][6].scriptString.str("automobile deaths yearly would be greatly reduced by self driving cars.");
    scriptTexture[2][7].scriptString.str("                                Press LMB or Enter to continue...");

//chapter 1, page 4
    scriptTexture[3][0].scriptString.str("The only problem with changing over to large scale automation in areas");
    scriptTexture[3][1].scriptString.str("like driving is the job problem.  The only way to deal with a job market");
    scriptTexture[3][2].scriptString.str("that is smaller than the labor force is to implement Universal Basic Income.");
    scriptTexture[3][3].scriptString.str("Universal Basic Income is the idea that everyone, no matter their employment");
    scriptTexture[3][4].scriptString.str("status receives a certain amount of money.  debtors can not garnish it, and");
    scriptTexture[3][5].scriptString.str("you still receive it if you are working...  ");
    scriptTexture[3][6].scriptString.str("My mission is Universal basic income.");
    scriptTexture[3][7].scriptString.str("                                Press LMB or Enter to continue...");

//chapter 1, page 5
    scriptTexture[4][0].scriptString.str("You may be asking yourself: Won't people lose the incentive ");
    scriptTexture[4][1].scriptString.str("to work if they receive UBI?  and yes, they will.  Millions of");
    scriptTexture[4][2].scriptString.str("Americans are working minimum wage jobs that don't provide a ");
    scriptTexture[4][3].scriptString.str("living wage.  The companies that provide these jobs fight against");
    scriptTexture[4][4].scriptString.str("worker's rights, because nothing is more important to them than");
    scriptTexture[4][5].scriptString.str("the bottom line.  In my last job I was working in a call center for");
    scriptTexture[4][6].scriptString.str("a big telecom company.  bedbugs and mandatory overtime for all.");
    scriptTexture[4][7].scriptString.str("                                Press LMB or Enter to continue...");
//chapter 1, page 6
    scriptTexture[5][0].scriptString.str("Since that job I've worked for myself mostly. Besides begging on");
    scriptTexture[5][1].scriptString.str("street corners, I have knitted several scarves although I never made");
    scriptTexture[5][2].scriptString.str("any money on them, only gave them away, except one that a friend");
    scriptTexture[5][3].scriptString.str("tried on and walked off with... sigh.  I've painted a lot of paintings");
    scriptTexture[5][4].scriptString.str("and sold a few of them.  acrylic + canvas.  I'm an avid gamer and");
    scriptTexture[5][5].scriptString.str("since I'm basically retired, I play a lot of pc/android games.");
    scriptTexture[5][6].scriptString.str("I have a bachelor's degree in computer science, so I'm programming this.");
    scriptTexture[5][7].scriptString.str("                                Press LMB or Enter to continue...");

    scriptTexture[6][0].scriptString.str("If universal basic income is instated, the ultra rich know that");
    scriptTexture[6][1].scriptString.str("eventually we will move toward income equality across the planet.");
    scriptTexture[6][2].scriptString.str("That means people will be doing what they want to instead of what");
    scriptTexture[6][3].scriptString.str("they must.  Since my last job I've been doing my part in keeping");
    scriptTexture[6][4].scriptString.str("the demand for labor up by not working.  I want a future where");
    scriptTexture[6][5].scriptString.str("employers don't low-ball new employees on wages, and perhaps we");
    scriptTexture[6][6].scriptString.str("can return to a time where employers search for employees.");
    scriptTexture[6][7].scriptString.str("                                Press LMB or Enter to continue...");

    scriptTexture[7][0].scriptString.str("I grew up playing PC and console videogames.  I knew I wanted");
    scriptTexture[7][1].scriptString.str("to make videogames.  It's why I got into computer science.");
    scriptTexture[7][2].scriptString.str("unfortunately the market for game developers is too competitive,");
    scriptTexture[7][3].scriptString.str("so I'm making this game independently in my spare time, with zero");
    scriptTexture[7][4].scriptString.str("budget. The jobs in the computer science field are cut-throat and");
    scriptTexture[7][5].scriptString.str("don't actually pay as much as I thought they would. I'm done making");
    scriptTexture[7][6].scriptString.str("online surveys for market research.");
    scriptTexture[7][7].scriptString.str("                                Press LMB or Enter to continue...");

	//scriptTexture[0].scriptString="5 years ago, I stopped working regular jobs.";
	//scriptTexture[1].scriptString="Since then I've mostly begged for money on the street corner with cardboard signs that say things like 'Anything Helps'.";
    //std::cout << "button name: " << gButtons[0].buttonName;

    if( file == NULL )
    {
        printf( "\n Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );

        //Create file for writing
        file = SDL_RWFromFile( "savegame/save.gsf", "w+b" );
        if( file != NULL )
        {
            printf( "\n New file created!\n" );

            //Initialize data
            for( int i = 0; i < TOTAL_DATA; ++i )
            {
                gData[ i ] = 0;
                SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
            }
            chapter1complete=0;
            currentPage=0;
            currentScript=0;

            //Close file handler
            SDL_RWclose( file );
        }
        else
        {
            printf( "Error: Unable to create file! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
    }//File exists
    else
    {
        printf( "Loading Save Game Data...!\n" );
        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWread( file, &gData[ i ], sizeof(Sint32), 1 );
        }
        chapter1complete= gData[0];
        currentPage=gData[1];
        currentScript = gData[2];

        SDL_RWclose( file );
    }
    //Initialize data textures
   // gDataTextures[ 0 ].loadFromRenderedText( std::to_string( gData[ 0 ] ), highlightColor );
    /*for( int i = 0; i < TOTAL_DATA; ++i )
    {
        //printf( std::to_string( gData[ i ] ) );
        std::cout << "\n gamesave item: " << std::to_string( gData[ i ] );
        //gDataTextures[ i ].loadFromRenderedText( std::to_string( gData[ i ] ), textColor );
    }*/

    std::cout << "\n chapter1complete: " << std::to_string( gData[ 0 ] );
    std::cout << "\n currentPage: " << std::to_string( gData[ 1 ] );
    std::cout << "\n currentScript: " << std::to_string( gData[ 2 ] );

    if( !tao[0].loadFromFile( "images/animations/tao/tao1.png",renderer ) )
	{
		printf( "\n Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[1].loadFromFile( "images/animations/tao/tao2.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[2].loadFromFile( "images/animations/tao/tao3.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[3].loadFromFile( "images/animations/tao/tao4.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[4].loadFromFile( "images/animations/tao/tao5.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[5].loadFromFile( "images/animations/tao/tao6.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[6].loadFromFile( "images/animations/tao/tao7.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	if( !tao[7].loadFromFile( "images/animations/tao/tao8.png",renderer ) )
	{
		printf( "Failed to load tao animation texture!\n" );
		success = false;
	}
	//Load button textures for title screen
	if( !buttons[0].buttonTexture.loadFromFile( "images/buttons/Back.png",renderer ) )
	{
		printf( "Failed to load Back button texture!\n" );
		success = false;
	}
	else if(!buttons[4].buttonTexture.loadFromFile("images/buttons/Credits.png",renderer))
    {
		printf( "Failed to load Credits button texture!\n" );
		success = false;
	}
	else if(!buttons[2].buttonTexture.loadFromFile("images/buttons/LoadGame.png",renderer))
    {
		printf( "Failed to load LoadGame button texture!\n" );
		success = false;
	}
	else if(!buttons[1].buttonTexture.loadFromFile("images/buttons/NewGame.png",renderer)){
        printf("Failed to load NewGame button texture!\n");
        success = false;
	}
	else if(!buttons[3].buttonTexture.loadFromFile("images/buttons/Options.png",renderer))
    {
		printf( "Failed to load Options button texture!\n" );
		success = false;
	}
	else if(!buttons[5].buttonTexture.loadFromFile("images/buttons/chapter1.png",renderer))
    {
		printf( "Failed to load Chapter 1 button texture!\n" );
		success = false;
	}
	else
	{
		//set titlescreen button positions
		for( int i = 0; i < TOTAL_BUTTONS; ++i )
		{
            buttons[ i ].setPosition( ((i*160)-80), SCREEN_HEIGHT - 140 );
		}
		buttons[0].setPosition(600,20);
		buttons[5].setPosition(20,20);

	}

    if( !title.loadFromFile( "images/title.png",renderer ) )
	{
		printf( "Failed to load title screen texture!\n" );
		success = false;
	}
	if( !titleTexture.loadFromFile( "images/defaulttitlescreen.png",renderer ) )
	{
		printf( "Failed to load title screen texture!\n" );
		success = false;
	}
	if( !creditsTexture.loadFromFile( "images/creditsscreen.png",renderer ) )
	{
		printf( "Failed to load credits screen texture!\n" );
		success = false;
	}if( !optionsTexture.loadFromFile( "images/optionsscreen.png",renderer) )
	{
		printf( "Failed to load options screen texture!\n" );
		success = false;
	}if( !loadGameTexture.loadFromFile( "images/loadgamescreen.png",renderer ) )
	{
		printf( "Failed to load load game screen texture!\n" );
		success = false;
	}if( !chapterSelectTexture.loadFromFile( "images/chapterSelect.png",renderer ) )
	{
		printf( "Failed to load chapter select Screen texture!\n" );
		success = false;
	}if( !chapter1BG.loadFromFile( "images/signflying800x600.png",renderer ) )
	{
		printf( "Failed to load chapter 1 Background Screen texture!\n" );
		success = false;
	}if( !chapter1BG2.loadFromFile( "images/parkbench.png",renderer ) )
	{
		printf( "Failed to load chapter 1 Background 2 Screen texture!\n" );
		success = false;
	}if( !chapter1BG3.loadFromFile( "images/bluewave.png",renderer ) )
	{
		printf( "Failed to load chapter 1 Background 3 Screen texture!\n" );
		success = false;
	}if( !chapter1BG4.loadFromFile( "images/please.png",renderer ) )
	{
		printf( "Failed to load chapter 1 Background 4 Screen texture!\n" );
		success = false;
	}

	if( !dialogBox.loadFromFile( "images/dialogbox1.png",renderer ) )
	{
		printf( "Failed to load chapter 1 Background Screen texture!\n" );
		success = false;
	}
    dialogBox.setAlpha(192);

	//Load music
	music = Mix_LoadMUS( "music/Radioactive Rain.mp3" );
	if( music == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	//Load sound effects
	gScratch = Mix_LoadWAV( "sounds/titleitemselect1.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gHigh = Mix_LoadWAV( "sounds/titleitemselect2.wav" );
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMedium = Mix_LoadWAV( "sounds/titleitemselect3.wav" );
	if( gMedium == NULL )
	{
		printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gLow = Mix_LoadWAV( "sounds/titleitemselect4.wav" );
	if( gLow == NULL )
	{
		printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	font = TTF_OpenFont( "fonts/Tapeworm.ttf", 16 );
	if( font == NULL )
    {
        printf( "Failed to load Tapeworm font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 55, 55, 55 };
        for(int j=0;j<TOTAL_PAGES;j++){
            for(int i=0;i<TOTAL_SCRIPTS;i++){
                if( !scriptTexture[j][i].loadFromRenderedText( scriptTexture[j][i].scriptString.str().c_str(), textColor,font, renderer ) )
                {
                    printf( "Failed to render text texture!\n" );
                    success = false;
                }
            }
        }

    }



	return success;
}

void close()
{

    //Open data for writing
    SDL_RWops* file = SDL_RWFromFile( "savegame/save.gsf", "w+b" );
    if( file != NULL )
    {
        //Save data

        gData[0] = chapter1complete;
        gData[1] = currentPage;
        gData[2] = currentScript;

        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
        }

        //Close file handler
        SDL_RWclose( file );
    }
    else
    {
        printf( "Error: Unable to save file! %s\n", SDL_GetError() );
    }

    FPSTextTexture.free();

    for(int i = 0; i<TOTAL_BUTTONS;i++){
        buttons[i].buttonTexture.free();
    }
for(int j = 0; j < TOTAL_PAGES; j++){
    for(int i=0;i<TOTAL_SCRIPTS;i++){
        scriptTexture[j][i].free();
    }
}
for(int i=0;i<TAO_ANIMATION_FRAMES;i++){
    tao[i].free();
}
    title.free();
	titleTexture.free();
	chapterSelectTexture.free();
	loadGameTexture.free();
	optionsTexture.free();
	creditsTexture.free();
	chapter1BG.free();
	chapter1BG2.free();
	chapter1BG3.free();
	chapter1BG4.free();
	dialogBox.free();

	//Free the sound effects
	Mix_FreeChunk( gScratch );
	Mix_FreeChunk( gHigh );
	Mix_FreeChunk( gMedium );
	Mix_FreeChunk( gLow );
	gScratch = NULL;
	gHigh = NULL;
	gMedium = NULL;
	gLow = NULL;

	textTexture.free();

	TTF_CloseFont( font );
    font = NULL;

	//Free the music
	Mix_FreeMusic( music );
	music = NULL;

	//Destroy window
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{

			//Main loop flag
			bool quit = false;

			/*for( int i = 0; i < TOTAL_PARTICLES; ++i )
        {
            particles[ i ] = new Particle(100,100,gRedTexture,gBlueTexture,gGreenTexture);
        }*/

			//starting music
            //Mix_PlayMusic( gMusic, -1 );

            //Set text color as black
			SDL_Color textColor = { 0, 0, 0, 255 };
            //The frames per second timer
			timer fpsTimer;

			//The frames per second cap timer
			timer capTimer;

			//timer for dialog for chapter1
			timer chapter1Timer;
            chapter1Timer.start();
            timer animationTimer;
            animationTimer.start();
			//page tracker
			//int currentPage = 0;
			//int currentScript = 0;

			//Frame of animation
			int aniFrame = 0;

			//In memory text stream
			std::stringstream timeText;

			//Start counting frames per second
			int countedFrames = 0;
			fpsTimer.start();

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{


			    //Start cap timer
				capTimer.start();
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

                    if(e.type == SDL_MOUSEBUTTONDOWN){
                        if(gameState == 5)
                        {
                            if (currentScript<TOTAL_SCRIPTS-1)
                            {
                                currentScript+=1;
                                chapter1Timer.stop();
                                chapter1Timer.start();
                            }
                            else
                            {
                                if(currentPage<TOTAL_PAGES-1)
                                {
                                    currentPage+=1;
                                    currentScript=0;
                                    chapter1Timer.stop();
                                    chapter1Timer.start();
                                }
                                else
                                {
                                    //currentPage=0;
                                    printf(" end of chapter 1 dialog return to game state = 0 \n");
                                    gameState = 0;
                                    chapter1complete=1;
                                }

                            }
                        }
                        if(gameState == 1)//chapter select
                        {
                            //chapter1Timer.stop();
                            //chapter1Timer.start();
                            currentPage = 0;
                            currentScript = 0;
                            chapter1complete = 0;
                            chapter1Timer.stop();
                            chapter1Timer.start();
                        }
                        if(gameState == 2){
                            currentScript-=1;
                            chapter1Timer.stop();
                            chapter1Timer.start();
                        }
                        /*
                        if(gameState == 5 && chapter1Timer.isStarted() && chapter1Timer.getTicks()<21000)//chapter 1
                        {
                            //don't ask me why but setTicks must be negative to increase timer.
                            std::cout << "\n chapter 1 timer ticks before set ticks: " << chapter1Timer.getTicks();
                            //chapter1Timer.setTicks(-3000);//if ch1 is running and player wants to speed up text.
                            //std::cout << "\n chapter 1 timer ticks after set ticks: " << chapter1Timer.getTicks();
                            //printf("chapter 1 timer ticks",chapter1Timer.getTicks());
                        }*/
                    }

					//Handle key press
					if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{


                            case SDLK_SPACE:
                                if(gameState == 5){
                                    //chapter1Timer.stop();
                                    //chapter1Timer.start();


                                    if(currentScript<TOTAL_SCRIPTS-1)
                                    {
                                        currentScript+=1;
                                        chapter1Timer.stop();
                                        chapter1Timer.start();
                                    }
                                    else
                                    {
                                        if(currentPage<TOTAL_PAGES-1)
                                        {
                                            currentPage+=1;
                                            currentScript=0;
                                            chapter1Timer.stop();
                                            chapter1Timer.start();
                                        }
                                        else
                                        {
                                            //currentPage=0;
                                            printf("end of chapter 1 dialog return to game state = 0 \n");
                                            gameState = 0;
                                            chapter1complete=1;
                                        }

                                    }
                                }
                                break;


							//Play high sound effect
							case SDLK_1:
							Mix_PlayChannel( -1, gHigh, 0 );
							break;

							//Play medium sound effect
							case SDLK_2:
							Mix_PlayChannel( -1, gMedium, 0 );
							break;

							//Play low sound effect
							case SDLK_3:
							Mix_PlayChannel( -1, gLow, 0 );
							break;

							//Play scratch sound effect
							case SDLK_4:
							Mix_PlayChannel( -1, gScratch, 0 );
							break;

							case SDLK_9:
							//If there is no music playing
							if( Mix_PlayingMusic() == 0 )
							{
								//Play the music
								Mix_PlayMusic( music, -1 );
							}
							//If music is being played
							else
							{
								//If the music is paused
								if( Mix_PausedMusic() == 1 )
								{
									//Resume the music
									Mix_ResumeMusic();
								}
								//If the music is playing
								else
								{
									//Pause the music
									Mix_PauseMusic();
								}
							}
							break;

							case SDLK_0:
							//Stop the music
							Mix_HaltMusic();
							break;
						}
					}
					//Handle button events
					for( int i = 0; i < TOTAL_BUTTONS; ++i )
					{

						gameState = buttons[ i ].handleEvent(gameState,buttons[i].buttonName, &e );


					}

				}

				//Calculate and correct fps
				float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
				if( avgFPS > 2000000 )
				{
					avgFPS = 0;
				}

				//Set text to be rendered
				timeText.str( "" );
				timeText << "Average Frames Per Second (With Cap) " << avgFPS;

				//Render text
				if( !FPSTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor, font,renderer ) )
				{
					printf( "Unable to render FPS texture!\n" );
				}

				//Clear screen
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( renderer );


                if(gameState == 0)//Title Screen
                {
                    titleTexture.render( 0, 0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer );

                    renderParticles();
                    title.render(200, 0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    //Render objects
                    FPSTextTexture.render( 20, 20,NULL,0.0,NULL,SDL_FLIP_NONE,renderer );

                    //render title screen buttons
                    for(int i=1;i<TOTAL_BUTTONS;i++)
                    {
                        if(i!=5)
                            buttons[i].buttonTexture.render(buttons[i].getPositionX(),buttons[i].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    }
                }
                else if(gameState == 1){//chapter select screen
                    chapterSelectTexture.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer );
                    //chapter 1 button
                    buttons[5].buttonTexture.render(buttons[5].getPositionX(),buttons[5].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    //back button
                    buttons[0].buttonTexture.render(buttons[0].getPositionX(),buttons[0].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                }
                else if(gameState == 2){
                        //chapter select screen
                    chapterSelectTexture.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    //chapter 1 button
                    buttons[5].buttonTexture.render(buttons[5].getPositionX(),buttons[5].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    //back button
                    buttons[0].buttonTexture.render(buttons[0].getPositionX(),buttons[0].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                }
                else if(gameState == 3){
                    optionsTexture.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    buttons[0].buttonTexture.render(buttons[0].getPositionX(),buttons[0].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                }
                else if(gameState == 5)
                {
                    //Chapter 1
                    for(int j = 0; j<TOTAL_PAGES;j++){
                        //render background & dialog box before script lines
                        switch(currentPage %4){
                            case 0:chapter1BG.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    dialogBox.render(0,400,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    break;
                            case 1:chapter1BG2.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    dialogBox.render(0,400,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    break;
                            case 2:chapter1BG3.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    dialogBox.render(0,400,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    break;
                            case 3:chapter1BG4.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    dialogBox.render(0,400,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                                    break;
                        }
                        for(int i = 0; i<TOTAL_SCRIPTS;i++){
                            //render script lines
                            if(i <= currentScript)
                            {
                                scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                            }
                            /*
                            switch(i){
                                case 0:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 1:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 2:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 3:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 4:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 5:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 6:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                                case 7:
                                    scriptTexture[currentPage][i].render(20,420 + (i*20),NULL,0.0,NULL,SDL_FLIP_NONE,gRenderer);
                                    break;
                            }*/
                        }
                        /*
                        if(chapter1Timer.getTicks() > 21000)
                                        chapter1Timer.pause();*/
                    }
                    switch(aniFrame){
                    case 0:tao[0].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 1:tao[1].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 2:tao[2].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 3:tao[3].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 4:tao[4].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 5:tao[5].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 6:tao[6].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    case 7:tao[7].render(750,350,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                        break;
                    }

                    buttons[0].buttonTexture.render(buttons[0].getPositionX(),buttons[0].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);

                }
                else{
                    creditsTexture.render(0,0,NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                    buttons[0].buttonTexture.render(buttons[0].getPositionX(),buttons[0].getPositionY(),NULL,0.0,NULL,SDL_FLIP_NONE,renderer);
                }


				//Update screen
				SDL_RenderPresent( renderer );
				++countedFrames;
				//If frame finished early
				int frameTicks = capTimer.getTicks();
				if( frameTicks < SCREEN_TICK_PER_FRAME )
				{
					//Wait remaining time
					SDL_Delay( SCREEN_TICK_PER_FRAME - frameTicks );
				}
                if(animationTimer.getTicks() / 500 > 1)
                {
                    ++aniFrame;
                    animationTimer.stop();
                    animationTimer.start();
                }

                //set script line
                if(chapter1Timer.getTicks()/1000 > 1){
                    if(currentScript<TOTAL_SCRIPTS-1){
                        currentScript++;
                        chapter1Timer.stop();
                        chapter1Timer.start();
                    }/*
                    else
                    {
                        if(currentPage < TOTAL_PAGES-1){
                            currentPage+=1;
                        }
                        else
                        {
                            currentPage=0;
                        }
                        currentScript=0;
                        chapter1Timer.stop();
                        chapter1Timer.start();
                    }*/
			}

				//Cycle animation
				if( aniFrame >= TAO_ANIMATION_FRAMES )
				{
					aniFrame = 0;
				}

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
