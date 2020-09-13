#include <thread>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "fonction.hpp"

void createAndInitWindow(int weight, int height);
void drawText();
void updateRender();
void drawRect(int posx, int posy);
void drawBestPath(Node & graphe, bool isBegin, int nbr_road);
int main(int argc, char ** argv);