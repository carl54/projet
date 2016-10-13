//g++ truc2.c `sdl-config --cflags --libs` -o truc
#include "SDL.h"
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 400
#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define WALL_WIDTH 32
#define PERSO_WIDTH 16
#define fov M_PI/3

int gameover;
float x = 6;
float y = 5;
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
                    if (map[((perso_x-400-PERSO_WIDTH)/WALL_WIDTH)+((perso_y/WALL_WIDTH))*MAP_WIDTH] != '#'){ //#=mur
                        map[(perso_x-400)/WALL_WIDTH+perso_y/WALL_WIDTH*MAP_WIDTH] = ' ';
                        perso_x = perso_x-PERSO_WIDTH;
                    }
                    if(mat_perso[perso_x-1][perso_y]!='#'){
                        mat_perso[perso_x][perso_y]=' ';
                        mat_perso[perso_x-1][perso_y]='0';
                        perso_x--;
                    }
                    break;
                case SDLK_RIGHT:
                    if (map[((perso_x-400+PERSO_WIDTH)/WALL_WIDTH)+((perso_y/WALL_WIDTH))*MAP_WIDTH] != '#'){
                        map[(perso_x-400)/WALL_WIDTH+perso_y/WALL_WIDTH*MAP_WIDTH] = ' ';
                        perso_x = perso_x+PERSO_WIDTH;
                        if(mat_perso[perso_x+1][perso_y]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x+1][perso_y]='0';
                            perso_x++;
                        }
                        break;
                        case SDLK_UP:
                            if (map[((perso_x-400)/WALL_WIDTH)+((perso_y-PERSO_WIDTH)/WALL_WIDTH)*MAP_WIDTH] != '#'){
                                map[(perso_x-400)/WALL_WIDTH+perso_y/WALL_WIDTH*MAP_WIDTH] = ' ';
                                perso_y = perso_y-PERSO_WIDTH;
                            }
                        if(mat_perso[perso_x][perso_y-1]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x][perso_y-1]='0';
                            perso_y--;
                        }
                        break;
                        case SDLK_DOWN:
                            if (map[((perso_x-400)/WALL_WIDTH)+((perso_y+PERSO_WIDTH)/WALL_WIDTH)*MAP_WIDTH] != '#'){
                                map[(perso_x-400)/WALL_WIDTH+perso_y/WALL_WIDTH*MAP_WIDTH] = ' ';
                                perso_y = perso_y+PERSO_WIDTH;
                            }
                        if(mat_perso[perso_x][perso_y+1]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x][perso_y+1]='0';
                            perso_y++;
                        }
                        break;
                    }
                    break;
            }

    }
}


void draw(SDL_Surface *screen, int perso_x, int perso_y){
    int i,j;
    float a;
    int w = screen->w/2;
    SDL_Rect wall,perso,tmp;
    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,255,255,255));
    for (i=0;i<MAP_WIDTH;i++){
        for (j=0;j<MAP_HEIGHT;j++){
            if (map[i+j*MAP_WIDTH] == '#'){
                wall.w = WALL_WIDTH;
                wall.h = WALL_WIDTH;
                wall.x = i*WALL_WIDTH+w;
                wall.y = j*WALL_WIDTH;
                SDL_FillRect(screen,&wall,SDL_MapRGB(screen->format,255,0,0));
            }
            else{
                perso.w = PERSO_WIDTH;
                perso.h = PERSO_WIDTH;
                perso.x = perso_x;
                perso.y = perso_y;
                SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
            }
        }
    }
    a=M_PI/2;
    for (i=0; i<w; i++) { // draw the "3D" view + visibility cone
        float ca = (1.-i/float(w)) * (a-fov/2.) + i/float(w)*(a+fov/2.);
        for (float t=0; t<20; t+=.05) {
            float cx = x+cos(ca)*t;
            float cy = y+sin(ca)*t;

            int idx = int(cx)+int(cy)*MAP_WIDTH;
            if (map[idx]!=' ') {
                int h = screen->h/t;
                tmp.w = 1;
                tmp.h = h;
                tmp.x = i;
                tmp.y = (screen->h-h)/2;
                //(screen->h-h)/2
                if (map[idx]=='#'){
                    SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 255, 0,0));
                }
                else {
                    SDL_FillRect(screen, &tmp, SDL_MapRGB(screen->format, 0, 0,0));
                }
                break;
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
            if(mat_perso[i][j]!='#'){
                mat_perso[i][j]='0';
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
    /*while (!gameover){
        SDL_Event event;

        // look for an event
        if (SDL_PollEvent(&event)) {
            HandleEvent(event);
        }
        map[(perso_x-400)/WALL_WIDTH+perso_y/WALL_WIDTH*MAP_WIDTH] = '%';
        draw(screen,perso_x,perso_y);

        // update the screen
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    //SDL_FreeSurface(screen);
    SDL_Quit();*/

    return 0;

}