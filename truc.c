//g++ truc.c `sdl-config --cflags --libs` -o truc
#include "SDL.h"
#include <stdlib.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 400
#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define WALL_WIDTH 32

int gameover;
int perso_x,perso_y; //coordonnees du personnage
char map[MAP_WIDTH*MAP_HEIGHT+1]="\
############\
#  #    #  #\
#  #  ###  #\
#  #  #    #\
#  #     ###\
#  ##      #\
#     ###  #\
#  ##   #  #\
#   #   #  #\
#   #   ####\
#   #      #\
############";

void HandleEvent(SDL_Event event)
{
  switch (event.type) 
    {
      // close button clicked 
    case SDL_QUIT:
      gameover = 1;
      break;
      
      // handle the keyboard 
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) 
	{
	case SDLK_ESCAPE:
	case SDLK_q:
	  gameover = 1;
	  break;
	case SDLK_LEFT:
	  if (map[perso_x-1+perso_y*MAP_WIDTH] != '#'){
	    map[perso_x+perso_y*MAP_WIDTH] = ' ';
	    perso_x--;
	  }
	  break;
	case SDLK_RIGHT:
	  if (map[perso_x+1+perso_y*MAP_WIDTH] != '#'){
	    map[perso_x+perso_y*MAP_WIDTH] = ' ';
	    perso_x++;
	  }
	  break;
	case SDLK_UP:
	  if (map[perso_x+(perso_y-1)*MAP_WIDTH] != '#'){
	    map[perso_x+perso_y*MAP_WIDTH] = ' ';
	    perso_y--;
	  }
	  break;
	case SDLK_DOWN:
	  if (map[perso_x+(perso_y+1)*MAP_WIDTH] != '#'){
	    map[perso_x+perso_y*MAP_WIDTH] = ' ';
	    perso_y++;
	  }
	  break;
	}
      break;
    }
}


void draw(SDL_Surface *screen){
  int i,j;
 
  SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));
  for (i=0;i<MAP_WIDTH;i++){
    for (j=0;j<MAP_HEIGHT;j++){
      if (map[i+j*MAP_WIDTH] == '#'){
	SDL_Rect wall;
	wall.w = WALL_WIDTH;
	wall.h = WALL_WIDTH;
	wall.x = i*WALL_WIDTH;
	wall.y = j*WALL_WIDTH;
	SDL_FillRect(screen,&wall,SDL_MapRGB(screen->format,255,0,0));
      }
      else{
	if(map[i+j*MAP_WIDTH] == '%'){
	SDL_Rect perso;
	perso.w = WALL_WIDTH;
	perso.h = WALL_WIDTH;
	perso.x = i*WALL_WIDTH;
	perso.y = j*WALL_WIDTH;
	SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
	}
      }
    }
  }
}

int main (int argc, char*args[]){
  SDL_Surface *screen;
  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  
  // create window
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

  // set keyboard repeat
  SDL_EnableKeyRepeat(200, 200);

  gameover=0; 
 
  perso_x = 6;//coordonees initiales du perso
  perso_y = 5;

  while (!gameover){
    SDL_Event event;
    
    // look for an event
    if (SDL_PollEvent(&event)) {
      HandleEvent(event);
    }
    map[perso_x+perso_y*MAP_WIDTH] = '%';
    draw(screen);

    // update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);
  }
  SDL_FreeSurface(screen);
  SDL_Quit();
  
  return 0;
}
