//============================================================================
// Name        : magic.cpp
// Author      : louisfghbvc
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <bits/stdc++.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

using namespace std;

///Screen attributes
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 800;
const int SCREEN_BPP = 32;

///Card size
const int CARD_WIDTH = 123;
const int CARD_HEIGHT = 160;
const int CARD_MAX = 54;
const int CARD_GAMENUM = 27;
const int BUTTON_WIDTH = 332;
const int BUTTON_HEIGHT = 50;

///The surfaces
SDL_Surface *screen = NULL;
SDL_Surface *Background = NULL;
SDL_Surface *Card_image = NULL;
SDL_Surface *Button_image = NULL;

///The clip of image card
SDL_Rect Card_clip[CARD_MAX];

///Random card id array
int CardInd[CARD_GAMENUM];
bool Card_used[CARD_MAX];

///Card stack
vector<int> st[3];

///Background music
Mix_Music *backsound = NULL;

///The event structure
SDL_Event event;

///The frames per second
const int FRAMES_PER_SECOND = 15;

///The timer
class Timer
{
    private:
    ///The clock time when the timer started
    int startTicks;

    ///The ticks stored when the timer was paused
    int pausedTicks;

    ///The timer status
    bool paused;
    bool started;

    public:
    ///Initializes variables
    Timer();

    ///The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    ///Gets the timer's time
    int get_ticks();

    ///Checks the status of the timer
    bool is_started();
    bool is_paused();
};

//The button
class Button
{
    private:
    //The attributes of the button
    SDL_Rect box;

    //The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

    public:
    //Initialize the variables
    Button( int x, int y, int w, int h );

    //Handles events and set the button's sprite region
    bool handle_events();

    //Shows the button on the screen
    void show();

    void set_clip(SDL_Rect* clips);
};

///Button
Button b1(0, 600, BUTTON_WIDTH, BUTTON_HEIGHT);
Button b2(BUTTON_WIDTH + 30, 600, BUTTON_WIDTH, BUTTON_HEIGHT);
Button b3(2 * BUTTON_WIDTH + 60, 600, BUTTON_WIDTH, BUTTON_HEIGHT);
SDL_Rect BtnClips[3];

///Starts up SDL and creates window
bool init();

///Load files in project
bool load_files();

///Frees media and shuts down SDL
void close();

///Load image
SDL_Surface* load_image_alpha(string filename);

///Place surface to destination
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL);

///Set image clips
void set_clips();

///Show 21 random card
void show_cards();

///Set card index
void set_cardid();

void reshow_cards(int cnt);

int main ( int argc, char** argv )
{
    ///Quit flag
    bool quit = false;

    if(!init()) return 1;

    ///Place surface
    apply_surface(0, 0, Background, screen);
    set_cardid();
    show_cards();

    if( SDL_Flip( screen ) == -1 )
        return 1;

    SDL_Delay(2000);

    ///The frame rate regulator
    Timer fps;

    ///Guess time
    int guess = 0;

    while(!quit){
        ///Start the frame timer
        fps.start();

        ///While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            ///Handle events for the stick figure
            ///handle_events();

            if(guess < 3)
            {
                ///push button 1
                if(b1.handle_events())
                {
                    apply_surface(0, 0, Background, screen);
                    SDL_Flip( screen );
                    SDL_Delay(2000);
                    reshow_cards(++guess);

                }
                if(b2.handle_events())
                {
                    apply_surface(0, 0, Background, screen);
                    SDL_Flip( screen );
                    SDL_Delay(2000);
                    swap(st[1], st[0]);
                    reshow_cards(++guess);
                }
                if(b3.handle_events())
                {
                    apply_surface(0, 0, Background, screen);
                    SDL_Flip( screen );
                    SDL_Delay(2000);
                    swap(st[2], st[0]);
                    reshow_cards(++guess);
                }
            }
            else
            {
                apply_surface(0, 0, Background, screen);
                apply_surface(0, 0, Card_image, screen, &Card_clip[CardInd[0]]);
            }

            ///If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                ///Quit the program
                quit = true;

            }
        }

        ///Show button
        b1.show();
        b2.show();
        b3.show();

        ///Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        ///Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

    }

    ///Quit SDL
    close();

    return 0;
}

Timer::Timer()
{
    ///Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    ///Start the timer
    started = true;

    ///Unpause the timer
    paused = false;

    ///Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    ///Stop the timer
    started = false;

    ///Unpause the timer
    paused = false;
}

void Timer::pause()
{
    ///If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        ///Pause the timer
        paused = true;

        ///Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    ///If the timer is paused
    if( paused == true )
    {
        ///Unpause the timer
        paused = false;

        ///Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        ///Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    ///If the timer is running
    if( started == true )
    {
        ///If the timer is paused
        if( paused == true )
        {
            ///Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            ///Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    ///If the timer isn't running
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

bool init()
{
    ///Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    ///Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    ///If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    ///Set the window caption
    SDL_WM_SetCaption( "Magic1", NULL );

    ///Set Music
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
        return false;

    ///Load files
    if(load_files() == false)
    {
        return false;
    }

    ///Play Music
    if(Mix_PlayMusic(backsound, -1) == -1)
    {
        return false;
    }

    set_clips();

    ///If everything initialized fine
    return true;
}

SDL_Surface *load_image_alpha( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

	loadedImage = IMG_Load(filename.c_str() );

    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        SDL_FreeSurface( loadedImage );
    }

    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool load_files()
{
    //Load the sprite sheet
	Background = load_image_alpha( "./pic/fgo_background.jpg" );
	Card_image = load_image_alpha("./pic/cards.png");
	Button_image = load_image_alpha("./pic/button.jpg");
    backsound = Mix_LoadMUS("./music/jojo op.mp3");

    //If everything loaded fine
    return true;
}

void close()
{
    //Free the surface
    SDL_FreeSurface( Background );
    SDL_FreeSurface( screen );
    SDL_FreeSurface( Card_image );
    SDL_FreeSurface( Button_image );
	Mix_FreeMusic(backsound);

    //Quit SDL
    SDL_Quit();
}

void set_clips()
{
    //Clip the sprites
	for (int i = 0; i < CARD_MAX; i++) // 1920 x 1072
	{
        Card_clip[i].x = CARD_WIDTH * (i % 13);
        Card_clip[i].y = CARD_HEIGHT * (i / 13);
        Card_clip[i].w = CARD_WIDTH;
        Card_clip[i].h = CARD_HEIGHT;
	}

	///Clip the btn sprites
	for (int i = 0; i < 3; i++)
	{
        BtnClips[i].x = i * BUTTON_WIDTH;
        BtnClips[i].y = BUTTON_HEIGHT;
        BtnClips[i].w = BUTTON_WIDTH;
        BtnClips[i].h = BUTTON_HEIGHT;
	}

	b1.set_clip(&BtnClips[0]);
	b2.set_clip(&BtnClips[1]);
	b3.set_clip(&BtnClips[2]);
}

void show_cards()
{
    if(CARD_GAMENUM > CARD_MAX)
        return;

    const int card_col = 9;

    ///card's distance
    const int card_side = 30;

    for(int i = 0; i < CARD_GAMENUM / card_col; ++i)
    {
        for(int j = 0; j < card_col; ++j)
        {
            int ind = i * card_col + j;
            apply_surface((CARD_WIDTH + card_side) * j, (CARD_HEIGHT + card_side) * i, Card_image, screen, &Card_clip[CardInd[ind]]);
            st[i].push_back(CardInd[ind]);
        }
    }
}

void set_cardid()
{
    ///seed use system time
    int seed = time(NULL);
    srand(seed);

    memset(Card_used, 0, sizeof Card_used);

    int cnt = 0;

    while(cnt < CARD_GAMENUM){

        ///rand = 0 ~ CARD_MAX
        int rand_num = rand() % CARD_MAX;

        ///If used continue
        if(Card_used[rand_num]) continue;

        Card_used[rand_num] = true;
        CardInd[cnt++] = rand_num;
    }

}

void Button::set_clip(SDL_Rect* clips)
{
    clip = clips;
}

Button::Button( int x, int y, int w, int h )
{
    //Set the button's attributes
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;
}

bool Button::handle_events()
{
    //The mouse offsets
    int x = 0, y = 0;

    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN )
    {
        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                return true;
            }
        }
    }
    return false;
}

void Button::show()
{
    //Show the button
    apply_surface( box.x, box.y, Button_image, screen, clip );
}

void reshow_cards(int cnt)
{
    const int card_col = 9;

    ///card's distance
    const int card_side = 30;

    for(int i = 0; i < CARD_GAMENUM / card_col; ++i)
    {
        ///Initialize Index
        for(int j = 0; j < card_col; ++j)
        {
            int ind = i * card_col + j;
            CardInd[ind] = st[i][j];
        }
        st[i].clear();
    }

    for(int i = 0; i < CARD_GAMENUM / card_col; ++i)
    {
        if(cnt == 1 && i == 0)
        {
            for(int j = 0; j < 3; ++j)
                swap(CardInd[3 + j], CardInd[9 + j]);
            for(int j = 0; j < 3; ++j)
                swap(CardInd[6 + j], CardInd[18 + j]);
        }
        else if(cnt == 2 && i == 0)
        {
            swap(CardInd[1], CardInd[9]);
            swap(CardInd[2], CardInd[18]);
        }

        for(int j = 0; j < card_col; ++j)
        {
            int ind = i * card_col + j;
            apply_surface((CARD_WIDTH + card_side) * j, (CARD_HEIGHT + card_side) * i, Card_image, screen, &Card_clip[CardInd[ind]]);
            st[i].push_back(CardInd[ind]);
        }
    }

}
