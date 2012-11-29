#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include "timer.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <cmath>

// Constants
const int SCREEN_HEIGHT = 640;
const int SCREEN_WIDTH = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 60;

// Surfaces
SDL_Surface *screen = NULL;
SDL_Surface *grid = NULL;
SDL_Surface *grid_over = NULL;
SDL_Surface *grid_down = NULL;
SDL_Surface *grid_highlight = NULL;
SDL_Surface *background = NULL;

// Event structure
SDL_Event event;

// Co-ords of colours
SDL_Rect clips[4];

// Functiooooons

bool init()
{
    // Initialise everything
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;

    screen = SDL_SetVideoMode(SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_BPP, SDL_SWSURFACE);
    if (screen == NULL) return false;

    SDL_WM_SetCaption("Memory", NULL);

    // Initialise random function
    srand (time (NULL));

    return true;
}

SDL_Surface *load_image(std::string filename)
{
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimisedImage = NULL;

    loadedImage = IMG_Load(filename.c_str());
    if (loadedImage != NULL)
    {
        optimisedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
    }

    return optimisedImage;
}

bool load_files()
{
    grid = load_image("assets/grid.png");
    if (grid == NULL) return false;

    grid_over = load_image("assets/grid_over.png");
    if (grid_over == NULL) return false;

    grid_down = load_image("assets/grid_down.png");
    if (grid_down == NULL) return false;

    grid_highlight = load_image("assets/grid_highlight.png");
    if (grid_highlight == NULL) return false;

    background = load_image("assets/background.png");
    if (background == NULL) return false;

    return true;
}

void clean_up()
{
    // Free memory
    SDL_FreeSurface(grid);
    SDL_FreeSurface(background);
    SDL_FreeSurface(grid_over);
    SDL_FreeSurface(grid_down);
    SDL_FreeSurface(grid_highlight);

    // Quit SDL
    SDL_Quit();
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect* clip = NULL)
{
    // offsets
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    // Blit
    SDL_BlitSurface(source, clip, destination, &offset);
}
void test()
{
    apply_surface(0, 0, grid, screen);
    SDL_Flip(screen);
    SDL_Delay(5000);

    apply_surface(0, 0, background, screen);
    apply_surface(0, 0, grid, screen, &clips[0]);
    SDL_Flip(screen);
    SDL_Delay(2000);

    apply_surface(0, 0, background, screen);
    apply_surface(320, 0, grid, screen, &clips[1]);
    SDL_Flip(screen);
    SDL_Delay(2000);

    apply_surface(0, 0, background, screen);
    apply_surface(0, 240, grid, screen, &clips[2]);
    SDL_Flip(screen);
    SDL_Delay(2000);

    apply_surface(0, 0, background, screen);
    apply_surface(320, 240, grid, screen, &clips[3]);
    SDL_Flip(screen);
    SDL_Delay(2000);

    apply_surface(0, 0, background, screen);
    apply_surface(0, 0, grid_over, screen);
    SDL_Flip(screen);
    SDL_Delay(2000);

    apply_surface(0, 0, background, screen);
    apply_surface(0, 0, grid_down, screen);
    SDL_Flip(screen);
    SDL_Delay(2000);
}

using namespace std;

int main(int argc, char* argv[])
{
    if (!init()) return 1;
    if (!load_files()) return 1;

    // Define clip regions
    // red
    clips[0].x = 0;
    clips[0].y = 0;
    clips[0].w = 320;
    clips[0].h = 240;

    // blue
    clips[1].x = 320;
    clips[1].y = 0;
    clips[1].w = 320;
    clips[1].h = 240;

    // green
    clips[2].x = 0;
    clips[2].y = 240;
    clips[2].w = 320;
    clips[2].h = 240;

    // yellow
    clips[3].x = 320;
    clips[3].y = 240;
    clips[3].w = 320;
    clips[3].h = 240;

    // Frame rate regulation
    // Current frame
    int frame = 0;

    // Whether or not to cap the frame rate
    bool cap = true;

    // The frame regulator
    Timer fps;

    bool quit = false;

    // Function to test images
    if (argc == 2) test();

    // Event variables
    int x;
    int y;

    int mouseLocation = 0;

    bool mouseClicked = 0;
    bool mouseReleased = 0;

    bool mouseDown = 0;
    bool mouseDownPrev = 0;

    bool spacePressed = 0;

    bool spaceDown = 0;
    bool spacePrev = 0;

    // Game variables
    SDL_Rect *clip = NULL;
    //

    enum {STATE_NOGAME, STATE_DEMO, STATE_PLAY, STATE_WINNER};
    enum {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};

    int curTile;

    int curLength = 2;
    queue<int> tiles;
    vector<int> vTiles;

    int gameState = STATE_NOGAME;


    // State variables
    // Demo

    Timer gameTimer;

    printf("Starting game...\n");

    while (!quit)
    {
        // Start the frame timer
        fps.start();

        // EVENTS
        // -------------------
        // Get mouse position, clicking or not
        // Check for end game
        
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                            quit = true;
                            break;

                        case SDLK_SPACE:
                            spaceDown = 1;
                            break;

                        default:;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_SPACE:
                            spaceDown = 0;
                            break;

                        default:;
                    }
                    break;

                // Mouse moved
                case SDL_MOUSEMOTION:
                    x = event.motion.x;
                    y = event.motion.y;
                    break;

                // Mouse button clicked
                // Only interested in left mouse
                case SDL_MOUSEBUTTONDOWN:
                    mouseDown = 1;
                    break;

                case SDL_MOUSEBUTTONUP:
                    mouseDown = 0;
                    break;


                default:;
            }
        }

        // Get mouse location
        // Top
        if (y < 240)
        {
            // Top left
            if (x < 320) mouseLocation = TOP_LEFT;
            // Top right
            else mouseLocation = TOP_RIGHT;
        }
        // Bottom
        else
        {
            // Bottom left
            if (x < 320) mouseLocation = BOTTOM_LEFT;
            // Bottom right
            else mouseLocation = BOTTOM_RIGHT;
        }

        // Check if mouse has been clicked
        if (mouseDownPrev == 0 && mouseDown == 1) {
            mouseClicked = 1;
        }

        else {
            mouseClicked = 0;
        }

        // Check if mouse has been released
        if (mouseDownPrev == 1 && mouseDown == 0) {
            mouseReleased = 1;
        }
        
        else {
            mouseReleased = 0;
        }

        // Check if space has been pressed
        if (spacePrev == 0 && spaceDown == 1) {
            spacePressed = 1;
        }

        else {
            spacePressed = 0;
        }

        mouseDownPrev = mouseDown;
        spacePrev = spaceDown;


        // LOGIC
        // ----------------------
        // mouseLocation stores the mouses location,
        // mouseClicked tells you if the mouse has been clicked this frame
        // spacePressed tells you if spacebar has been pressed
        
        switch( gameState )
        {
            case STATE_NOGAME:
                if (spacePressed) {
                    gameState = STATE_DEMO;
                    gameTimer.start();

                    // Generate sequence
                    int temp;
                    int pTemp = -1;
                    vTiles.clear();
                    while (!tiles.empty()) tiles.pop();

                    for (int i = 0; i < curLength; i++) {
                        temp = rand() % 4;
                        while (temp == pTemp) {
                            temp = rand() % 4;
                        }

                        tiles.push(temp);
                        vTiles.push_back(temp);

                        pTemp = temp;
                    }
                    
                    curTile = tiles.front();;
                }
                break;

            case STATE_DEMO:
                if (gameTimer.get_ticks() > 1000*curLength) {
                    gameTimer.stop();

                    curTile = tiles.front();
                    tiles.pop();

                    gameState = STATE_PLAY;
                }
                else {
                    curTile = vTiles[(int)(floor ( gameTimer.get_ticks()/1000 ) )];
                }

                break;

            case STATE_PLAY:
                if (mouseReleased) {
                    if (!tiles.empty()) {
                        if (mouseLocation == curTile) {
                            curTile = tiles.front();
                            tiles.pop();
                        }
                        else {
                            gameState = STATE_NOGAME;
                            curLength -= 1;
                            if (curLength <= 0) curLength = 1;
                        }
                    }
                    else {
                        if (mouseLocation == curTile) {
                            curLength += 1;
                            gameState = STATE_WINNER;

                            gameTimer.start();
                        }
                        else {
                            curLength -= 1;
                            if (curLength <= 0) curLength = 1;
                            gameState = STATE_NOGAME;
                        }
                    }
                }
                break;

            case STATE_WINNER:
                if (gameTimer.get_ticks() > 3000) {
                    gameTimer.stop();
                    gameState = STATE_NOGAME;
                }
                break;

            default:;
        }

        // RENDERING

        switch ( gameState )
        {
            case STATE_NOGAME:
                apply_surface(0, 0, grid, screen);
                break;

            case STATE_DEMO:
                apply_surface(0, 0, grid, screen);
                clip = &clips[curTile];

                apply_surface(clip->x, clip->y, grid_highlight, screen, clip);
                break;

            case STATE_PLAY:
                apply_surface(0, 0, grid, screen);
                clip = &clips[mouseLocation];

                if (mouseDown) {
                    apply_surface(clip->x, clip->y, grid_down, screen, clip);
                }
                else {
                    apply_surface(clip->x, clip->y, grid_over, screen, clip);
                }

                break;

            case STATE_WINNER:
                apply_surface(0, 0, grid_highlight, screen);
                clip = &clips[mouseLocation];

                apply_surface(clip->x, clip->y, grid, screen, clip);
                break;

            default:;
        }


        // Update the screen
        if (SDL_Flip(screen) == -1) return 1;

        // Increase the frame counter by 1
        frame++;

        // If we want to cap the frame rate
        if (cap && (fps.get_ticks() < 1000 / FRAMES_PER_SECOND))
        {
            // Sleep the remaining frame time
            SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
        }
    }

    printf("Game quit, cleaning up...\n");
    clean_up();

    return 0;
}
