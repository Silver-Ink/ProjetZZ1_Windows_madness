#ifndef main_h
#define main_h

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 200
#define WIN_COUNT 3
#define KEY_COUNT 322

void DrawOutOfScreen(SDL_Renderer* renderer, SDL_Window* window, int margin, int red, int green, int blue);
void moveWindow(SDL_Window* win, int shiftX, int shiftY);
void clearRenderer(SDL_Renderer* renderer, int current_red, int current_green, int current_blue);
#endif