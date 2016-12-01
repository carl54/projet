//g++ lol.c `sdl-config --cflags --libs` -o lol
#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH  600
#define SCREEN_HEIGHT 450
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define WALL_WIDTH 16
#define PERSO_WIDTH 16
#define FOV M_PI/3

typedef struct {
    float x;
    float y;
    float angle;
    int dir;
    int vie=60;
} monstre;
monstre monster;

typedef struct {
    int x, y, vie=45;
} murCassable;
murCassable mc1, mc2;
/*
 * elements classes dans l'ordre ascii
 * ` = mur
 * agms = levier monte, baisse, porte violet fermee, ouvert
 * bhnt = levier monte, baisse, porte orange fermee, ouvert
 * ciou = levier monte, baisse, porte blanc fermee, ouvert
 * djpv = levier monte, baisse, porte vert fermee, ouvert
 * ekqw = levier monte, baisse, porte marron fermee, ouvert
 * flrx = levier monte, baisse, porte noir fermee, ouvert
 */
int gameover, visionLevier, typeL;
int avancer, lateral, tourner, tir;
float perso_angle,perso_x,perso_y;
char map[MAP_WIDTH*MAP_HEIGHT+1]="\
``````f`r```````````````\
`         `    `       `\
`         e  ```       `\
`         `  `         `\
`         ^  ``        `\
`         ``           `\
`         q    ```     `\
`         ``   `       `\
c          d   `       `\
`          `   ```     `\
o          p           `\
`a`m`b`n````````````````";

char mat_perso[24][24], dir;
SDL_Surface *murDraw, *screen=SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0), *pistolet, *monstreDraw, *casque;
SDL_Rect posPistolet, rectPistolet, posCasque;

Uint32 getpixel(SDL_Surface *tex, int x, int y, int numText) {
  int texw = tex->w;
  int texh = tex->h-1;
  if (x<0 || y<0 || x>=texw || y>=texh) return 0;
  //printf("%d\n", x+texh * numText);
  Uint8 *p = (Uint8 *) tex->pixels + y * tex->pitch + (x+texh*numText) * tex->format->BytesPerPixel;
  return p[0] | p[1] << 8 | p[2] << 16;
}


void putpixel(SDL_Surface *sdl_screen_, int x, int y, Uint32 pixel) {
  if (x<0 || y<0 || x>=sdl_screen_->w || y>=sdl_screen_->h) return;
  if(pixel==SDL_MapRGB(pistolet->format, 0, 255, 255)) return;
  Uint8 *p = (Uint8 *)sdl_screen_->pixels + y*sdl_screen_->pitch + x*sdl_screen_->format->BytesPerPixel;
  for (int i=0; i<sdl_screen_->format->BytesPerPixel; i++) {
    p[i] = ((Uint8*)&pixel)[i];
  }
}

void fillMat(char map[], char mat[][24]){
  int comp, i, j;
  comp = 0;
  
  for(i = 0 ; i<24 ; i++){
    for(j=0 ; j<24 ; j++){
      mat[i][j]=map[comp];
      comp++;
    }
  }
}
//retoune 1 si le char désigne une case de levier levé, -1 si c'est un levier baissé, et 0 si ce n'en est pas un
int isLevier(char c){
  if(c>='a' && c<='f') return 1;
  if(c>='g' && c<='l') return -1;
  return 0;
}

int isPorte(char c){
  if(c>='m' && c<='r') return 1;
  return 0;
}
int isMur(char c){
  if(c>='^' && c<='`') return 1;
  return 0;
}
int isOpenDoor(char c){
  if(c>='s' && c<='x') return 1;
  return 0;
}

void openDoor(char c){
  
  for(int i=0;i<24;i++){
    for(int j=0;j<24;j++){
      if(mat_perso[i][j]==c+6){
        mat_perso[i][j]=c+12;
        printf("%c\n", c+12);
        goto label1;
      }
    }
  }
  
  label1:;
  
  
}

void closeDoor(char c ){
  for(int i=0;i<24;i++){
    for(int j=0;j<24;j++){
      if(mat_perso[i][j]==c+18){
        mat_perso[i][j]=c+12;
        printf("%c\n", mat_perso[i][j]);
        goto label2;
      }
    }
  }
  
  label2:;
}
void drawPistolet(SDL_Surface *screen, int numPistolet){
  rectPistolet.x=(233/3)*numPistolet;
  rectPistolet.w=(233/3);
  SDL_BlitSurface(pistolet, &rectPistolet, screen, &posPistolet);
}

float max(float a, float b) {
  return a<b ? b : a;
}
//les parametre sont en float pour savoir a partir de quelle colonne de pixel on affiche la texture
void drawTexture(SDL_Surface *screen, SDL_Surface *tex, float x, float y, SDL_Rect wall, int numText){
  int tx = max(fabs(x-floor(x+.5)), fabs(y-floor(y+.5)))*tex->h; // x-texcoord
  for (int i=0; i<wall.h; i++) {
    int ty = float(i)/float(wall.h)*tex->h;
    Uint32 color = getpixel(tex, tx, ty, numText);
    //if(color != SDL_MapRGB(pistolet->format, 0, 255, 255))
      putpixel(screen, wall.x, wall.y+i, color);
  }
}

/*void drawSol(SDL_Surface *screen, SDL_Rect sol, int numText, float wallDist){
  int tx, ty;
  float distance, posSolX, posSolY, weight;
  for (int i=sol.y; i < screen->h; i++) {
    distance= sol.h / (2.0*i-sol.h); //distance du pixel par rapport à la base du mur
    weight=distance/wallDist;
    //printf("%f\n", weight);
    posSolX=weight*int(sol.x);
    //printf("%f\n", posSolX);
    posSolY=weight*(int(sol.y))+(1.0-weight)*perso_x;
    printf("%f\n", posSolY);
    // printf("%f\n", posSolY);
    ty=int(posSolY*murDraw->h)%murDraw->h;
    //printf("%d\n",ty);
    tx=int(posSolX*murDraw->h)%murDraw->h;
    putpixel(screen, sol.x, i, getpixel(tx, ty,1));
    putpixel(screen, sol.x, i, getpixel(tx, ty,0));
  }
}*/

void draw(SDL_Surface *screen, int k) {
  int i, j, w, h, taille;
  float angle_vue, dist, angle_ray, ray_x, ray_y, t;
  SDL_Rect wall, perso, tmp, half_screen, lol, miniMap;
  half_screen.w = SCREEN_WIDTH/4;
  half_screen.h = SCREEN_HEIGHT;
  half_screen.x = 0;
  half_screen.y = 0;
  SDL_FillRect(screen, &half_screen, SDL_MapRGB(screen->format, 65, 69, 76));
  half_screen.x = SCREEN_WIDTH/4;
  
  SDL_FillRect(screen, &half_screen, SDL_MapRGB(screen->format, 65, 69, 76));
  half_screen.w = SCREEN_WIDTH;
  half_screen.y = 0;
  half_screen.x = 0;
  
  SDL_FillRect(screen, &half_screen, SDL_MapRGB(screen->format, 65, 69, 76));
  
  
  //M_PI/2 regarde en bas
  //M_PI*2 regarde a droite
  //M_PI regarde a gauche
  //(3*M_PI)/2 regarde en haut
  //l'angle de vue est invers� par rapport a l'angle du perso car le repere est invers� en y donc les angles sont chang�s
  w=524;
  angle_vue = -perso_angle;
  
  for (i = 74; i < w; i++) { // vue 3D
    angle_ray = angle_vue - (FOV / 2) + i * (FOV / w);
    taille = 0;
    for (t = 0; t < 48; t += .05) {
      ray_x = perso_x + cos(angle_ray) * t;
      ray_y = perso_y + sin(angle_ray) * t;
      
      if (mat_perso[int(ray_y)][int(ray_x)] != ' ' ||
          !isOpenDoor(mat_perso[int(ray_y)][int(ray_x)])) {
        dist = t;//sqrt(pow((perso_x-ray_x),2)+pow((perso_y-ray_y),2));
        dist = dist * cos(fabs(angle_vue - angle_ray));
        h = 50 * WALL_WIDTH / dist;
        tmp.w = 1;
        tmp.h = h;
        tmp.x = i;
        tmp.y = (SCREEN_HEIGHT - h) / 2;
        if(k==1 && i==SCREEN_WIDTH/2) {
          if (mat_perso[int(ray_y)][int(ray_x)] == '#') {
            printf("%d\n", monster.vie);
            monster.vie--;
          }
          if (mat_perso[int(ray_y)][int(ray_x)] == '^' || mat_perso[int(ray_y)][int(ray_x)] == '_') {
            printf("%d\n", mc1.vie);
            mc1.vie--;
            if(mc1.vie==15) mat_perso[int(ray_y)][int(ray_x)]='_';
            if (mc1.vie==0) mat_perso[int(ray_y)][int(ray_x)]=' ';
          }
        }
        if (mat_perso[int(ray_y)][int(ray_x)] >= '^' &&
            mat_perso[int(ray_y)][int(ray_x)] <= 'r') {
          
          
          drawTexture(screen, murDraw, ray_x, ray_y, tmp,
                      (mat_perso[int(ray_y)][int(ray_x)] - '^'));
          visionLevier = 1;
          tmp.h = (screen->h - tmp.h) / 2;
          //printf("h=%d\n", tmp.h);
          tmp.y = h + tmp.h;
          //drawSol(screen, tmp, 0, dist);
          //if(visionLevier) printf("%d\n", visionLevier);
          break;
        }
        if(mat_perso[int(ray_y)][int(ray_x)] == '#'&&monster.vie>0){
          drawTexture(screen, monstreDraw, ray_x, ray_y, tmp, 0);
          break;
        }
      } else {
        
        visionLevier = 0;
      }
    }
    
  }
  miniMap.x=472;
  miniMap.y=0;
  miniMap.w=SCREEN_WIDTH-miniMap.x;
  miniMap.h=153;
  SDL_FillRect(screen, &miniMap, SDL_MapRGB(screen->format, 0, 0, 0));
  w = 472;
  wall.w = (SCREEN_WIDTH-w)/MAP_WIDTH;
  wall.h = (SCREEN_HEIGHT-153)/MAP_HEIGHT;
  for (i = 0; i < MAP_WIDTH; i++) {         //vue 2D
    for (j = 0; j < MAP_HEIGHT; j++) {
      if (isMur(mat_perso[i][j]) || isPorte(mat_perso[i][j]) || isLevier(mat_perso[i][j])) {
        wall.x = j * wall.w + w;
        wall.y = i * wall.h;
        SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 0, 0, 176));
      }
    }
  }
  
  
  
  perso.w = 9;
  perso.h = 9;
  perso.x = perso_x*wall.w+w-4;
  perso.y = perso_y*wall.h-4;
  //SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,255,255,255));
  
  /*perso.w = 9;
  perso.h = 9;
  perso.x = monster.x*PERSO_WIDTH+w-4;
  perso.y = monster.y*PERSO_WIDTH-4;
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
  perso.w = 3;
  perso.h = 3;
  perso.x = monster.x*PERSO_WIDTH+w-1;
  perso.y = monster.y*PERSO_WIDTH-1;*/
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,255,0));
  SDL_BlitSurface(casque, NULL, screen, &posCasque);
  drawPistolet(screen, k);
}
void HandleEvent(SDL_Event event){
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
          gameover = 1;
          break;
          
          //tourner le perso a gauche
        case SDLK_LEFT:
          tourner=-1;
          break;
          
          //tourner le perso a droite
        case SDLK_RIGHT:
          tourner = 1;
          break;
        
        case SDLK_z:
          avancer=1;
          break;
        
        case SDLK_s:
          avancer=-1;
          break;
        
        case SDLK_d:
          lateral=1;
          break;
        
        case SDLK_q:
          lateral=-1;
          break;
        
        case SDLK_SPACE:
          if(visionLevier && (isLevier(mat_perso[int(perso_y+1)][int(perso_x)]) || isLevier(mat_perso[int(perso_y)][int(perso_x+1)])
                              ||isLevier(mat_perso[int(perso_y-1)][int(perso_x)]) || isLevier(mat_perso[int(perso_y)][int(perso_x-1)]))){
            if(typeL = isLevier(mat_perso[int(perso_y+1)][int(perso_x)])){
              mat_perso[int(perso_y+1)][int(perso_x)] += 6*typeL;
              if(typeL==1) openDoor(mat_perso[int(perso_y+1)][int(perso_x)]);
              if(typeL==-1) closeDoor(mat_perso[int(perso_y+1)][int(perso_x)]);
            }
            if(typeL = isLevier(mat_perso[int(perso_y)][int(perso_x+1)])){
              mat_perso[int(perso_y)][int(perso_x+1)] += 6*typeL;
              if(typeL==1) openDoor(mat_perso[int(perso_y)][int(perso_x+1)]);
              if(typeL==-1) closeDoor(mat_perso[int(perso_y)][int(perso_x+1)]);
            }
            if(typeL = isLevier(mat_perso[int(perso_y-1)][int(perso_x)])){
              mat_perso[int(perso_y-1)][int(perso_x)] += 6*typeL;
              if(typeL==1) openDoor(mat_perso[int(perso_y-1)][int(perso_x)]);
              if(typeL==-1) closeDoor(mat_perso[int(perso_y-1)][int(perso_x)]);
            }
            if(typeL = isLevier(mat_perso[int(perso_y)][int(perso_x-1)])){
              mat_perso[int(perso_y)][int(perso_x-1)] += 6*typeL;
              if(typeL==1) openDoor(mat_perso[int(perso_y)][int(perso_x)-1]);
              if(typeL==-1) closeDoor(mat_perso[int(perso_y)][int(perso_x)-1]);
            }
          }else{
            tir=1;
          }
          break;
      }
      break;
    case SDL_KEYUP:
      switch (event.key.keysym.sym)
      {
        case SDLK_LEFT:
          tourner=0;
          break;
        
        case SDLK_RIGHT:
          tourner = 0;
          break;
        
        case SDLK_z:
          avancer=0;
          break;
        
        case SDLK_s:
          avancer=0;
          break;
        
        case SDLK_d:
          lateral=0;
          break;
        
        case SDLK_q:
          lateral=0;
          break;
        
        case SDLK_SPACE:
          tir=0;
          break;
      }
  }
}

void deplacer() {
  switch (avancer) {
    case 1:
      if (!isMur(mat_perso[int(perso_y - sin(perso_angle) * 0.5)][int(
              perso_x + cos(perso_angle) * 0.5)]) && !isLevier(
              mat_perso[int(perso_y - sin(perso_angle) * 0.5)][int(
                      perso_x + cos(perso_angle) * 0.5)])
          && !isPorte(mat_perso[int(perso_y - sin(perso_angle) * 0.5)][int(
              perso_x + cos(perso_angle) * 0.5)])) {
        mat_perso[int(perso_y)][int(perso_x)] = ' ';
        perso_x = perso_x + cos(perso_angle) * 0.05;
        perso_y = perso_y - sin(perso_angle) * 0.05;
        mat_perso[int(perso_y)][int(perso_x)] = '0';
        
      }
      break;
    
    case -1:
      if (!isMur(mat_perso[int(perso_y + sin(perso_angle) * 0.5)][int(
              perso_x - cos(perso_angle) * 0.5)])
          && !isLevier(mat_perso[int(perso_y + sin(perso_angle) * 0.5)][int(
              perso_x - cos(perso_angle) * 0.5)])
          && !isPorte(mat_perso[int(perso_y + sin(perso_angle) * 0.5)][int(
              perso_x - cos(perso_angle) * 0.5)])) {
        mat_perso[int(perso_y)][int(perso_x)] = ' ';
        perso_x = perso_x - cos(perso_angle) * 0.05;
        perso_y = perso_y + sin(perso_angle) * 0.05;
        mat_perso[int(perso_y)][int(perso_x)] = '0';
      }
      break;
  }
  switch (lateral) {
    case 1:
      if (!isMur(mat_perso[int(perso_y - sin(perso_angle-M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle-M_PI/2) * 0.5)])
          && !isLevier(mat_perso[int(perso_y - sin(perso_angle+M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle-M_PI/2) * 0.5)])
          && !isPorte(mat_perso[int(perso_y - sin(perso_angle-M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle-M_PI/2) * 0.5)])) {
        mat_perso[int(perso_y)][int(perso_x)] = ' ';
        perso_x = perso_x+cos(perso_angle-M_PI/2)*0.05;
        perso_y = perso_y-sin(perso_angle-M_PI/2)*0.05;
        mat_perso[int(perso_y)][int(perso_x)] = '0';
      }
      break;
    
    case -1:
      if (!isMur(mat_perso[int(perso_y - sin(perso_angle+M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle+M_PI/2) * 0.5)])
          && !isLevier(mat_perso[int(perso_y - sin(perso_angle+M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle+M_PI/2) * 0.5)])
          && !isPorte(mat_perso[int(perso_y - sin(perso_angle+M_PI/2) * 0.5)][int(
              perso_x + cos(perso_angle+M_PI/2) * 0.5)])) {
        mat_perso[int(perso_y)][int(perso_x)] = ' ';
        perso_x = perso_x+cos(perso_angle+M_PI/2)*0.05;
        perso_y = perso_y-sin(perso_angle+M_PI/2)*0.05;
        mat_perso[int(perso_y)][int(perso_x)] = '0';
      }
      break;
  }
  
  
  
  
  switch (tourner){
    case 1:
      if (perso_angle < M_PI/12){
        perso_angle = perso_angle+2*M_PI;
      }
      perso_angle = perso_angle-M_PI/100;
      break;
    
    case -1:
      if(perso_angle > 23*M_PI/12){
        perso_angle = perso_angle-2*M_PI;
      }
      perso_angle=perso_angle+M_PI/100;
      break;
    
  }
}


void move_monster(){
  float tmp_x,tmp_y,tmp_angle;
  int sens;
  if(monster.vie>0) {
    if (monster.dir == 0) {
      monster.dir = rand() % 100;
      tmp_angle = fmod(rand(), M_PI / 2);
      sens = rand() % 2;
      if (sens == 1) {
        monster.angle = monster.angle + tmp_angle;
      } else {
        monster.angle = monster.angle - tmp_angle;
      }
    }
    tmp_x = monster.x + cos(monster.angle) * 0.05;
    tmp_y = monster.y - sin(monster.angle) * 0.05;
    if (mat_perso[int(tmp_y)][int(tmp_x)] >= '^' &&
        mat_perso[int(tmp_y)][int(tmp_x)] < 's') {
      monster.dir = 0;
    } else {
      mat_perso[int(monster.y)][int(monster.x)] = ' ';
      monster.x = tmp_x;
      monster.y = tmp_y;
      mat_perso[int(tmp_y)][int(tmp_x)] = '#';
      monster.dir--;
    }
  }else mat_perso[int(monster.y)][int(monster.x)] = ' ';
}
//les parametre sont en float pour savoir a partir de quelle colonne de pixel on affiche la texture
/*void drawTexture(SDL_Surface *screen, float x, float y, SDL_Rect wall, int numText){
  int tx = max(fabs(x-floor(x+.5)), fabs(y-floor(y+.5)))*murDraw->h; // x-texcoord
  for (int i=0; i<wall.h; i++) {
    int ty = float(i)/float(wall.h)*murDraw->h;
    Uint32 color = getpixel(tx, ty, numText);
    putpixel(screen, wall.x, wall.y+i, color);
  }
}*/

/*void draw(SDL_Surface *screen){
  int i,j,w,h,taille;
  float angle_vue,dist,angle_ray,ray_x,ray_y,t;
  SDL_Rect wall,perso,tmp,half_screen, lol;
  half_screen.w = 200;
  half_screen.h = 200;
  half_screen.x=0;
  half_screen.y = 200;
  
  SDL_BlitSurface(solDraw,NULL,screen,&half_screen);
  half_screen.x=200;
  
  SDL_BlitSurface(solDraw,NULL,screen,&half_screen);
  half_screen.w = 400;
  half_screen.y = 0;
  half_screen.x=0;
  
  SDL_FillRect(screen,&half_screen,SDL_MapRGB(screen->format,143,223,232));
  //w = SCREEN_WIDTH;
  w = SCREEN_WIDTH/2;
  for (i=0;i<24;i++){         //vue 2D
    for (j=0;j<24;j++){
      if (mat_perso[i][j] == '`'){
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
  
  perso.w = 9;
  perso.h = 9;
  perso.x = monster.x*PERSO_WIDTH+w-4;
  perso.y = monster.y*PERSO_WIDTH-4;
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,0,0));
  perso.w = 3;
  perso.h = 3;
  perso.x = monster.x*PERSO_WIDTH+w-1;
  perso.y = monster.y*PERSO_WIDTH-1;
  SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,0,255,0));
  
  //M_PI/2 regarde en bas
  //M_PI*2 regarde a droite
  //M_PI regarde a gauche
  //(3*M_PI)/2 regarde en haut
  //l'angle de vue est invers� par rapport a l'angle du perso car le repere est invers� en y donc les angles sont chang�s
  angle_vue = -perso_angle;
  
  for (i=0; i<w; i++) { // vue 3D
    angle_ray = angle_vue-(FOV/2)+i*(FOV/w);
    taille=0;
    for (t=0; t<48; t+=.05) {
      ray_x = perso_x+cos(angle_ray)*t;
      ray_y = perso_y+sin(angle_ray)*t;
      
      if (mat_perso[int(ray_y)][int(ray_x)]!=' ' || !isOpenDoor(mat_perso[int(ray_y)][int(ray_x)])) {
        dist = t;//sqrt(pow((perso_x-ray_x),2)+pow((perso_y-ray_y),2));
        dist = dist*cos(fabs(angle_vue-angle_ray));
        h = 50*WALL_WIDTH/dist;
        tmp.w = 1;
        tmp.h = h;
        tmp.x = i;
        tmp.y = (SCREEN_HEIGHT-h)/2;
        
        if (mat_perso[int(ray_y)][int(ray_x)] >= '`' && mat_perso[int(ray_y)][int(ray_x)] <= 'r') {
          drawTexture(screen, ray_x, ray_y, tmp, (mat_perso[int(ray_y)][int(ray_x)]-'`'));
          visionLevier=1;
          tmp.h=(screen->h-tmp.h)/2;
          //printf("h=%d\n", tmp.h);
          tmp.y=h+tmp.h;
          drawSol(screen, tmp, 0, dist);
          //if(visionLevier) printf("%d\n", visionLevier);
          break;
        }
        
      }else{
        
        visionLevier=0;
      }
    }
  }
}*/

int main (int argc, char*args[]){
  int i, j;
  //SDL_Surface *screen;
  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  pistolet=SDL_LoadBMP("pistolet.bmp");
  murDraw = SDL_LoadBMP("walltext.bmp");
  monstreDraw = SDL_LoadBMP("monstre.bmp");
  casque = SDL_LoadBMP("visionCasque.bmp");
  SDL_SetColorKey(pistolet, SDL_SRCCOLORKEY, SDL_MapRGB(pistolet->format, 0, 255, 255));
  SDL_SetColorKey(monstreDraw, SDL_SRCCOLORKEY, SDL_MapRGB(monstreDraw->format, 0, 255, 255));
  SDL_SetColorKey(casque, SDL_SRCCOLORKEY, SDL_MapRGB(casque->format, 0, 255, 255));
  SDL_SetColorKey(murDraw, SDL_SRCCOLORKEY, SDL_MapRGB(murDraw->format, 0, 255, 255));
  rectPistolet.y=0;
  rectPistolet.h=pistolet->h;
  posPistolet.w = SCREEN_WIDTH/4;
  posPistolet.h = SCREEN_WIDTH/4;
  posPistolet.x=SCREEN_WIDTH/2-pistolet->w/6;
  posPistolet.y = 342-pistolet->h;
  mc1.x=10;
  mc1.y=4;
  // create window
  //screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
  // set keyboard repeat
  SDL_EnableKeyRepeat(1, 100);
  
  gameover=0;
  
  
  srand(time(NULL));
  
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
      if(mat_perso[i][j]==' '){
        mat_perso[i][j]='0';
        perso_x=j+0.1;
        perso_y=i+0.1;
        goto label;
        
      }
    }
  }
  label:
  
  perso_angle = 0;
  
  do {
    monster.x = rand()%MAP_HEIGHT;
    monster.y = rand()%MAP_WIDTH;
  } while (mat_perso[int(monster.y)][int(monster.x)]!=' ');
  mat_perso[int(monster.y)][int(monster.x)] = '#';
  monster.angle = 0;
  monster.dir = 0;
  
  for(int i=0;i<24;i++){
    for(int j=0;j<24;j++){
      printf("%c", mat_perso[i][j]);
    }
    printf("\n");
  }
  
  
  while (!gameover){
    
    SDL_Event event;
    
    
    if(tir){
      
      for(int k = 0 ; k<45 ; k++){
        // look for an event
        if (SDL_PollEvent(&event)) {
          HandleEvent(event);
        }
        deplacer();
        move_monster();
        draw(screen, k/15);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
      }
    }else{
      // look for an event
      if (SDL_PollEvent(&event)) {
        HandleEvent(event);
      }
      deplacer();
      move_monster();
      draw(screen, 0);
      SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    
  }
  SDL_FreeSurface(screen);
  SDL_Quit();
  
  return 0;
  
}
