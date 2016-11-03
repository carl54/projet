//g++ lol.c `sdl-config --cflags --libs` -o lol
#include "SDL.h"
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 400
#define MAP_WIDTH 24 
#define MAP_HEIGHT 24
#define WALL_WIDTH 16
#define PERSO_WIDTH 16
#define FOV M_PI/3

int gameover;
float perso_angle,perso_x,perso_y;
char map[MAP_WIDTH*MAP_HEIGHT+1]="\
############\
## #    #  #\
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
char mat_perso[24][24];

//rempli la matrice
//enlever 24 apres
void fillMat(char map[], char mat[][24]){
    int comp, i, j;
    comp = 0;

    for(i = 0 ; i<24 ; i=i+2){
        for(j=0 ; j<24 ; j=j+2){
            switch(map[comp/2]){
                case '#':
                    mat[i][j]='#';
                    mat[i][j+1]='#';
                    mat[i+1][j]='#';
                    mat[i+1][j+1]='#';
                    break;
                default: break;
            }
            comp=comp+2;
        }
    }
}

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
		//tourner le perso a gauche
	     case SDLK_LEFT:
	       if(perso_angle > 23*M_PI/12){
		 perso_angle = perso_angle-2*M_PI;
	       }
	       perso_angle=perso_angle+M_PI/12;
	       break;

		//tourner le perso a droite
	     case SDLK_RIGHT:
	       if (perso_angle < M_PI/12){
		 perso_angle = perso_angle+2*M_PI;
	       }
	       perso_angle = perso_angle-M_PI/12;
	       break;

	     case SDLK_UP:
	       if(mat_perso[int(perso_y-sin(perso_angle)*0.5)][int(perso_x+cos(perso_angle)*0.5)]!='#'){
		 mat_perso[int(perso_y)][int(perso_x)]=' ';
		 perso_x = perso_x+cos(perso_angle)*0.5;
		 perso_y = perso_y-sin(perso_angle)*0.5;
		 mat_perso[int(perso_y)][int(perso_x)]='0';
                 
	       } 
	       break;
	     case SDLK_DOWN:
	      if(mat_perso[int(perso_y+sin(perso_angle)*0.5)][int(perso_x-cos(perso_angle)*0.5)]!='#'){
		 mat_perso[int(perso_y)][int(perso_x)]=' ';
		 perso_x = perso_x-cos(perso_angle)*0.5;
		 perso_y = perso_y+sin(perso_angle)*0.5;
		 mat_perso[int(perso_y)][int(perso_x)]='0';
                 
	       } 
	       break;
	     }
	   break;
       }
}

//les parametre sont en float pour savoir a partir de quelle colonne de pixel on affiche la texture
void drawTexture(SDL_Surface *screen, float x, float y){
	int pix;
	printf("%f, %f\n", x, y);
	x = floor(x*100);
	pix = int(x)%10;
	printf("%d\n",pix);
}

void draw(SDL_Surface *screen, SDL_Surface *sol){
  int i,j,w,h;
  float angle_vue,dist,angle_ray,ray_x,ray_y,t;
  SDL_Rect wall,perso,tmp,half_screen;
  half_screen.w = 200;
  half_screen.h = 200;
  half_screen.x=0;
  half_screen.y = 200;

  SDL_BlitSurface(sol,NULL,screen,&half_screen);
  half_screen.x=200;

  SDL_BlitSurface(sol,NULL,screen,&half_screen);
  half_screen.w = 400;
  half_screen.y = 0;
  half_screen.x=0;

  SDL_FillRect(screen,&half_screen,SDL_MapRGB(screen->format,143,223,232));
  w = SCREEN_WIDTH/2;
  for (i=0;i<24;i++){         //vue 2D
    for (j=0;j<24;j++){
      if (mat_perso[i][j] == '#'){
	wall.w = WALL_WIDTH;
	wall.h = WALL_WIDTH;
	wall.x = j*WALL_WIDTH+w;
	wall.y = i*WALL_WIDTH;
        if(i%2 == 0){
	    if(j%2 == 0){
	      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 255, 0,0));
	    }
	    else{
	      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 102, 69,0));
	    }
	  }
	  else{
	    if(j%2 == 0){
	      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 102, 69,0));
	    }
	    else{
	      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 255, 0,0));
	    }
	  }
      }
    }
  }
  perso.w = 9;
  perso.h = 9;
  perso.x = perso_x*PERSO_WIDTH+w-4;
  perso.y = perso_y*PERSO_WIDTH-4;
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
  perso.w = 1;
  perso.h = 1;
  perso.x = perso_x*PERSO_WIDTH+w;
  perso.y = perso_y*PERSO_WIDTH;
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,255,255,10));
  
  //M_PI/2 regarde en bas
  //M_PI*2 regarde a droite
  //M_PI regarde a gauche 
  //(3*M_PI)/2 regarde en haut
  //l'angle de vue est inversé par rapport a l'angle du perso car le repere est inversé en y donc les angles sont changés
  angle_vue = -perso_angle;
  
  for (i=0; i<w; i++) { // vue 3D
    angle_ray = angle_vue-(FOV/2)+i*(FOV/w);
    for (t=0; t<48; t+=.05) {
      ray_x = perso_x+cos(angle_ray)*t;
      ray_y = perso_y+sin(angle_ray)*t;
      
      if (mat_perso[int(ray_y)][int(ray_x)]!=' ') {
	dist = sqrt(pow((perso_x-ray_x),2)+pow((perso_y-ray_y),2));
	dist = dist*cos(fabs(angle_vue-angle_ray));
	h = 50*WALL_WIDTH/dist;
	tmp.w = 1;
	tmp.h = h;
	tmp.x = i;
	tmp.y = (SCREEN_HEIGHT-h)/2;
	if (mat_perso[int(ray_y)][int(ray_x)] == '#'){
		drawTexture(screen, ray_x, ray_y);
	  if(int(ray_y)%2 == 0){
	    if(int(ray_x)%2 == 0){
	      SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 255, 0,0));
	    }
	    else{
	      SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 102, 69,0));
	    }
	  }
	  else{
	    if(int(ray_x)%2 == 0){
	      SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 102, 69,0));
	    }
	    else{
	      SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 255, 0,0));
	    }
	  }
	  break;
	}
	/*
	if (mat_perso[int(ray_x)][int(ray_y)]=='0') {
	  SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 0, 0,0));
	}
	break;
	*/
      }
    }
  } 
}

int main (int argc, char*args[]){
    int i, j;
    SDL_Surface *screen,*sol;
    // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // create window
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
sol = SDL_LoadBMP("Grass_double.bmp");
    // set keyboard repeat
    SDL_EnableKeyRepeat(1, 100);

    gameover=0;


    //rempli la matrice avec ' '
    for(i=0;i<24;i++){
        for(j=0;j<24;j++){
            mat_perso[i][j]=' ';
         }
    }

    fillMat(map, mat_perso);

    //placement du perso dans la premiere case vide de la matrice
    for(i=0;i<24;i++){
        for(j=0;j<24;j++){
            if(mat_perso[i][j]!='#'){
                mat_perso[i][j]='0';
		perso_x=j+0.1;
		perso_y=i+0.1;
                goto label;

            }
        }
    }
    label:

    perso_angle = 0;

    /*
     for(int i=0;i<24;i++){
        for(int j=0;j<24;j++){
            printf("%c", mat_perso[i][j]);
        }
        printf("\n");
	}
    */
    
    while (!gameover){
        SDL_Event event;

        // look for an event
        if (SDL_PollEvent(&event)) {
            HandleEvent(event);
        }

        draw(screen,sol);

        // update the screen
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;

}
