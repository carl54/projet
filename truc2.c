//g++ truc2.c `sdl-config --cflags --libs` -o truc
#include "SDL.h"
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 400
#define MAP_WIDTH 24 
#define MAP_HEIGHT 24
#define WALL_WIDTH 16
#define PERSO_WIDTH 16
#define fov M_PI/3

int gameover;
float x = 8;
float y = 16;
int perso_x,perso_y; //coordonnees du personnage
char map[MAP_WIDTH*MAP_HEIGHT+1]="\
############\
#  #    #  #\
#  #  ###  #\
## #  #    #\
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
	     case SDLK_LEFT:
	       if(mat_perso[perso_x][perso_y-1]!='#'){
		 mat_perso[perso_x][perso_y]=' ';
		 perso_y--;
		 mat_perso[perso_x][perso_y]='0';
                 
	       }
	       break;
	     case SDLK_RIGHT:
	       if(mat_perso[perso_x][perso_y+1]!='#'){
		 mat_perso[perso_x][perso_y]=' ';
		 perso_y++;
		 mat_perso[perso_x][perso_y]='0';
                 
	       }
	       break;
	     case SDLK_UP:
	       if(mat_perso[perso_x-1][perso_y]!='#'){
		 mat_perso[perso_x][perso_y]=' ';
		 perso_x--;
		 mat_perso[perso_x][perso_y]='0';
                 
	       } 
	       break;
	     case SDLK_DOWN:
	       if(mat_perso[perso_x+1][perso_y]!='#'){
		 mat_perso[perso_x][perso_y]=' ';
		 perso_x++;
		 mat_perso[perso_x][perso_y]='0';
                 
	       }
	       break;
	     }
	   break;
       }
}



void draw(SDL_Surface *screen, int perso_x, int perso_y){
    int i,j;
    float a;
    int w = screen->w/2;
    SDL_Rect wall,perso,tmp;
    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));
    for (i=0;i<24;i++){
        for (j=0;j<24;j++){
            if (mat_perso[j][i] == '#'){
                wall.w = WALL_WIDTH;
                wall.h = WALL_WIDTH;
                wall.x = i*WALL_WIDTH+w;
                wall.y = j*WALL_WIDTH;
                SDL_FillRect(screen,&wall,SDL_MapRGB(screen->format,255,0,0));
            }
        }
    }
    perso.w = PERSO_WIDTH;
    perso.h = PERSO_WIDTH;
    perso.x = perso_y*PERSO_WIDTH+400;
    perso.y = perso_x*PERSO_WIDTH;
    SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
    //M_PI/2 regarde a droite
    //M_PI*2 regarde en bas
    //M_PI regarde en haut 
    a=M_PI;
    for (i=0; i<w; i++) { // draw the "3D" view + visibility cone
        float ca = (1.-i/float(w)) * (a-fov/2.) + i/float(w)*(a+fov/2.);
        for (float t=0; t<20; t+=.05) {
            float cx = perso_x+cos(ca)*t;
            float cy = perso_y+sin(ca)*t;
            int idx = int(cx)+int(cy)*MAP_WIDTH;
            if (mat_perso[idx%MAP_WIDTH][idx/MAP_WIDTH]!=' ') {
                int h = screen->h/t;
                tmp.w = 1;
                tmp.h = h;
                tmp.x = i;
                tmp.y = (screen->h-h)/2;
                //(screen->h-h)/2
                if (mat_perso[idx%MAP_WIDTH][idx/MAP_WIDTH]=='#'){
                    SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 255, 0,0));
		break;
                }
               // if (mat_perso[idx%MAP_WIDTH][idx/MAP_WIDTH]=='0') {
               //     SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 0, 0,0));
               // }
               // break;
            }
        }
    }


}

int main (int argc, char*args[]){
    int i, j;
    SDL_Surface *screen, *screen2;
    // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // create window
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    // set keyboard repeat
    SDL_EnableKeyRepeat(1, 100);

    gameover=0;

    /*perso_x =400+32;//coordonees initiales du perso
    perso_y = 32;*/

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
            if(mat_perso[j][i]!='#'){
                mat_perso[j][i]='0';
		perso_x=j;
		perso_y=i;
                goto label;

            }
        }
    }
    label:
    for(int i=0;i<24;i++){
        for(int j=0;j<24;j++){
            printf("%c", mat_perso[i][j]);
        }
        printf("\n");
	}
    while (!gameover){
        SDL_Event event;

        // look for an event
        if (SDL_PollEvent(&event)) {
            HandleEvent(event);
        }
        draw(screen,perso_x,perso_y);

        // update the screen
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    //SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;

}
