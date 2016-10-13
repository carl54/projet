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
                    if(mat_perso[perso_x-1][perso_y]!='#'){
                        mat_perso[perso_x][perso_y]=' ';
                        mat_perso[perso_x-1][perso_y]='0';
                        perso_x--;
                    }
                    break;
                case SDLK_RIGHT:
                    if(mat_perso[perso_x+1][perso_y]!='#'){
                        mat_perso[perso_x][perso_y]=' ';
                        mat_perso[perso_x+1][perso_y]='0';
                        perso_x++;
                    }
                    break;
                case SDLK_UP:
                    if(mat_perso[perso_x][perso_y-1]!='#'){
                        mat_perso[perso_x][perso_y]=' ';
                        mat_perso[perso_x][perso_y-1]='0';
                        perso_y--;
                    }
                    break;
                case SDLK_DOWN:
                    if(mat_perso[perso_x][perso_y+1]!='#'){
                        mat_perso[perso_x][perso_y]=' ';
                        mat_perso[perso_x][perso_y+1]='0';
                        perso_y++;
                    }
                    break;
            }
            break;
    }
    for(int i=0;i<24;i++){
        for(int j=0;j<24;j++){
            printf("%c", mat_perso[i][j]);
        }
        printf("\n");
    }
}




int main (int argc, char*args[]){
    int i, j;
    SDL_Surface *screen, *screen2;
    // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    // set keyboard repeat
    //SDL_EnableKeyRepeat(1, 100);

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
                goto label;

            }
        }
    }
    label:

    while (!gameover){
        SDL_EVENT event;
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_QUIT: gameover=1;
                break;

            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_LEFT:
                        if(mat_perso[perso_x-1][perso_y]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x-1][perso_y]='0';
                            perso_x--;
                        }
                        break;
                    case SDLK_RIGHT:
                        if(mat_perso[perso_x+1][perso_y]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x+1][perso_y]='0';
                            perso_x++;
                        }
                        break;
                    case SDLK_UP:
                        if(mat_perso[perso_x][perso_y-1]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x][perso_y-1]='0';
                            perso_y--;
                        }
                        break;
                    case SDLK_DOWN:
                        if(mat_perso[perso_x][perso_y+1]!='#'){
                            mat_perso[perso_x][perso_y]=' ';
                            mat_perso[perso_x][perso_y+1]='0';
                            perso_y++;
                        }
                        break;

                }
                for(int i=0;i<24;i++){
                    for(int j=0;j<24;j++){
                        printf("%c", mat_perso[i][j]);
                    }
                    printf("\n");
                }

        }
    }

    return 0;
}
