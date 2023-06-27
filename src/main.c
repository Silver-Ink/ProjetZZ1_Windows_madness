#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h> 
#include "main.h"

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    SDL_Log("Start\n");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Failed to initialise SDL\n");
        return -1;
    }
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    SDL_Log("We compiled against SDL version %u.%u.%u ...\n",
        compiled.major, compiled.minor, compiled.patch);

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("SDL_Image could not initialize! SDL_image error : %s\n", IMG_GetError());
        return -1;
    }


    SDL_Window* lstWindow[WIN_COUNT];
    SDL_Renderer* lstRenderer[WIN_COUNT];
    int red = 255;
    int green = 0;
    int blue = 0;
    int speed = 1;

    for (int i = 0; i < WIN_COUNT; i++)
    {
        //Creation des fenêtres 
        SDL_Window* window = SDL_CreateWindow("Demo Game",
        (i+1)*WINDOW_WIDTH ,
        (i+1)*WINDOW_HEIGHT ,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE);
        if (window == NULL)
        {
            SDL_Log("Could not create a window number %d : %s", i, SDL_GetError());
            return -1;
        }
        lstWindow[i] = window;



        //Creation des renderers
        SDL_Renderer* renderer =
            SDL_CreateRenderer(window,
                -1,
                SDL_RENDERER_ACCELERATED |
                SDL_RENDERER_PRESENTVSYNC);
        
        if (renderer == NULL)
        {
            SDL_Log("Could not create a renderer number %d : %s", i, SDL_GetError());
            return -1;
        }
        lstRenderer[i] = renderer;
    }


    const Uint8* kbState; //Etat courant du clavier
    Uint32 msState; //Etat courant de la souris
    int msX, msY; //Position du curseur de la souris
    msState = SDL_GetMouseState(&msX, &msY);
    int old_msX = msX, old_msY = msY; //Position du curseur de la souris à la frame précédente

    int _running = 1;
    while (_running)
    {
        //Gestion des événements clavier
        kbState = SDL_GetKeyboardState(NULL);
        old_msX = msX, old_msY = msY;
        msState = SDL_GetMouseState(&msX, &msY); 
        SDL_Log("%d\n", msState);
        //arret du programme
        if (kbState[SDL_SCANCODE_ESCAPE])
        {
            _running = 0;
        }

        //Déplacment de toutes les fenêtres
        int wSpeed = kbState[SDL_SCANCODE_LCTRL] ? 3 : 1;
        for (int i = 0; i < WIN_COUNT; i++)
        {
            if (kbState[SDL_SCANCODE_UP])
            {
                moveWindow(lstWindow[i], 0, -wSpeed);
            }
            if (kbState[SDL_SCANCODE_DOWN])
            {
                moveWindow(lstWindow[i], 0, wSpeed);
            }
            if (kbState[SDL_SCANCODE_RIGHT])
            {
                moveWindow(lstWindow[i], wSpeed, 0);
            }
            if (kbState[SDL_SCANCODE_LEFT])
            {
                moveWindow(lstWindow[i], -wSpeed, 0);
            }
        }

        SDL_Log("cursor at %d : %d\n", msX, msY);
        
        
        for (int i = 0; i < WIN_COUNT; i++)
        {
            DrawOutOfScreen(lstRenderer[i], lstWindow[i], 5, red, green, blue);  
            Uint32 wState = SDL_GetWindowFlags(lstWindow[i]);
            if (wState & SDL_WINDOW_MOUSE_FOCUS)
            {
                SDL_SetRenderDrawColor(lstRenderer[i],255,255,255,255);
                SDL_RenderDrawLine(lstRenderer[i], msX, msY, old_msX, old_msY);
                SDL_SetRenderDrawColor(lstRenderer[i],red,green,blue,255);
            }
        }


        
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type) {                         
                case SDL_QUIT:
                //Ne fonctionne pas quand plusieurs fenêtres d'ouvertes ?
                    break;
                case SDL_MOUSEBUTTONDOWN:                     // Click souris   
                if (SDL_GetMouseState(NULL, NULL) & 
                    SDL_BUTTON(SDL_BUTTON_LEFT) ) 
                {

                } 
                else if (SDL_GetMouseState(NULL, NULL) & 
                        SDL_BUTTON(SDL_BUTTON_RIGHT) )    
                {

                }
                break;
            }
        }

        for (int i = 0; i < WIN_COUNT; i++)
        {
            SDL_RenderPresent(lstRenderer[i]);
            
        }
        
        //Change la couleur progressivement
        if (red == 255 && blue <= 0 && green < 255)
        {green += speed;}
        else if (red >= 1 && blue <= 0 && green == 255)
        {red -= speed;}
        else if (red <= 0 && blue < 255 && green == 255)
        {blue += speed;}
        else if (red <= 0 && blue == 255 && green >= 1)
        {green -= speed;}
        else if (red < 255 && blue == 255 && green <= 0)
        {red += speed;}
        else if (red == 255 && blue >= 1 && green <= 0)
        {blue-= speed;}

    }


    //fin du programme
    for (int i = 0; i < WIN_COUNT; i++)
    {
        SDL_DestroyWindow(lstWindow[i]);
        SDL_DestroyRenderer(lstRenderer[i]);
    }
    IMG_Quit();
    SDL_Quit();

    return 0;
}

void DrawOutOfScreen(SDL_Renderer* renderer, SDL_Window* window, int margin, int red, int green, int blue)
{

    SDL_DisplayMode screen;
    SDL_GetCurrentDisplayMode(0, &screen);

    int x, y; //position et taille de la fenetre
    int w, h;
    SDL_GetWindowPosition(window, &x, &y);
    SDL_GetWindowSize(window, &w, &h);

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    if (x+w > screen.w)
    {
        SDL_Rect rectangle;
        rectangle.x = screen.w - x;
        rectangle.y = 0;
        rectangle.w = x + w - screen.w;
        rectangle.h = h;

        SDL_RenderFillRect(renderer, &rectangle);
    }
    if (y+h > screen.h)
    {

        SDL_Rect rectangle;
        rectangle.x = 0;
        rectangle.y = screen.h - y;
        rectangle.w = w;
        rectangle.h = y + h - screen.h;

        SDL_RenderFillRect(renderer, &rectangle);
    }

}

void moveWindow(SDL_Window* win, int shiftX, int shiftY)
{
        int x, y;
        SDL_GetWindowPosition(win, &x, &y);
        SDL_SetWindowPosition(win, x+shiftX, y+shiftY);
}

