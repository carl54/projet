//g++ lol.c `sdl-config --cflags --libs` -o lol
#include "SDL.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


#define SCREEN_WIDTH  600
#define SCREEN_HEIGHT 450
#define MAP_WIDTH 60
#define MAP_HEIGHT 60
#define WALL_WIDTH 16
#define PERSO_WIDTH 16
#define FOV M_PI/3

typedef struct {
    float x;
    float y;
    float angle;
    int dir;
    int vie;
} monstre;
monstre monster[5];

typedef struct {
    int x, y, vie;
} murCassable;
murCassable mc[10];

int gameover, defaite=0, victoire=0, vie=100, visionLevier, visionFin, typeL;
int avancer, lateral, tourner, tir, murC;
float perso_angle,perso_x,perso_y;
char mat_perso[MAP_HEIGHT][MAP_WIDTH], dir;
SDL_Surface *murDraw, *screen, *pistolet, *monstreDraw, *casque;
SDL_Rect posPistolet, rectPistolet, posCasque, barreVie;

char menu(int i){
  
};
void FillMat()
{
  int i,j,murCass=0,mstr=0;
  char c;
  FILE* fichier = NULL;
  fichier = fopen("map.vuz","r");
  if(fichier !=NULL){
    for (i=0;i<MAP_WIDTH;i++){
      for (j=0;j<MAP_HEIGHT;j++){
        c=fgetc(fichier);
        mat_perso[i][j] = c;
        if(c=='^'){
          mc[murCass].x=i;
          mc[murCass].y=j;
          mc[murCass].vie=45;
          murCass++;
        }
        if(c=='#'){
          monster[mstr].x=j;
          monster[mstr].y=i;
          monster[mstr].angle=0;
          monster[mstr].dir=0;
          monster[mstr].vie=60;
          mstr++;
        }
        if(c=='0'){
          perso_x=j;
          perso_y=i;
          perso_angle = 0;
        }
      }
    }
    fclose(fichier);
  }else{
    printf("FATAL_ERROR_404\n");
  }
}

Uint32 getpixel(SDL_Surface *tex, int x, int y, int numText) {
  int texw = tex->w;
  int texh = tex->h-1;
  if (x<0 || y<0 || x>=texw || y>=texh) return 0;
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
  if(c=='^' || c=='^'+1 || c=='`' || c=='y') return 1;
  return 0;
}
int isOpenDoor(char c){
  if(c>='s' && c<='x') return 1;
  return 0;
}

void openDoor(char c){
  
  for(int i=0;i<MAP_HEIGHT;i++){
    for(int j=0;j<MAP_WIDTH;j++){
      if(mat_perso[i][j]==c+6){
        mat_perso[i][j]=c+12;
        goto label1;
      }
    }
  }
  
  label1:;
  
  
}

void closeDoor(char c ){
  for(int i=0;i<MAP_HEIGHT;i++){
    for(int j=0;j<MAP_WIDTH;j++){
      if(mat_perso[i][j]==c+18){
        mat_perso[i][j]=c+12;
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

int getMurCassable(int x, int y){
  for (int i=0 ; i<10 ; i++){
    if (mc[i].x==y && mc[i].y==x) return i;
  }
}

int getMonster(int x,int y) {
  int i,res=16;
  for (i=0;i<5;i++){
    if (int(monster[i].x)==x && int(monster[i].y)==y){
      res = i;
    }
  }
  return res;
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

void drawMonstre(SDL_Surface *screen, SDL_Surface *tex, float x, float y, SDL_Rect wall, int numText){
  int tx = max(fabs(x-floor(x+.5)), fabs(y-floor(y+.5)))*40; // x-texcoord
  for (int i=0; i<wall.h; i++) {
    int ty = float(i)/float(wall.h)*tex->h;
    Uint32 color = getpixel(tex, tx, ty, numText);
    //if(color != SDL_MapRGB(pistolet->format, 0, 255, 255))
    putpixel(screen, wall.x, wall.y+i, color);
  }
}


void draw(SDL_Surface *screen, int k) {
  int i, j, w, h, taille, mstri=0;
  float angle_vue, dist, angle_ray, ray_x, ray_y, t;
  SDL_Rect wall, perso, tmp, half_screen, lol, miniMap;
  half_screen.w = SCREEN_WIDTH;
  half_screen.h = SCREEN_HEIGHT;
  half_screen.x = 0;
  half_screen.y = 0;
  /*
  SDL_FillRect(screen, &half_screen, SDL_MapRGB(screen->format, 65, 69, 76));
  half_screen.x = SCREEN_WIDTH/4;
  
  SDL_FillRect(screen, &half_screen, SDL_MapRGB(screen->format, 65, 69, 76));
  half_screen.w = SCREEN_WIDTH;
  half_screen.y = 0;
  half_screen.x = 0;
  */
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
            mstri = getMonster(int(ray_x),int(ray_y));
            monster[mstri].vie--;
          }
          if (mat_perso[int(ray_y)][int(ray_x)] == '^' || mat_perso[int(ray_y)][int(ray_x)] == '_') {
            murC = getMurCassable(int(ray_x), int(ray_y));
            mc[murC].vie--;
            if(mc[murC].vie==15) mat_perso[int(ray_y)][int(ray_x)]='_';
            if (mc[murC].vie==0) mat_perso[int(ray_y)][int(ray_x)]=' ';
          }
        }
        
        if ((mat_perso[int(ray_y)][int(ray_x)] >= '^' &&
             mat_perso[int(ray_y)][int(ray_x)] <= 'r')) {
          
          drawTexture(screen, murDraw, ray_x, ray_y, tmp,(mat_perso[int(ray_y)][int(ray_x)] - '^'));
          
          visionLevier = 1;
          tmp.h = (screen->h - tmp.h) / 2;
          //tmp.y = h + tmp.h;
          //drawSol(screen, tmp, 0, dist);
          break;
        }
        if(mat_perso[int(ray_y)][int(ray_x)] == 'y' || mat_perso[int(ray_y)][int(ray_x)] == 'z'){
          visionFin = 1;
          drawTexture(screen, murDraw, ray_x, ray_y, tmp,mat_perso[int(ray_y)][int(ray_x)]-100);
          break;
        }
        
        if(mat_perso[int(ray_y)][int(ray_x)] == '#'|| mat_perso[int(ray_y)][int(ray_x)] == '$'){
          drawMonstre(screen, monstreDraw, ray_x, ray_y, tmp, (mat_perso[int(ray_y)][int(ray_x)]-'#'));
          break;
        }
      } else {
        visionFin = 0;
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
  wall.h = (180)/MAP_HEIGHT;
  for (i = 0; i < MAP_WIDTH; i++) {
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
  /*SDL_FillRect(screen,&perso,SDL_MapRGB(screen->format,255,255,255));
  
  perso.w = 9;
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
  barreVie.x=0;
  barreVie.y=0;
  barreVie.w=127;
  barreVie.h=153;
  SDL_FillRect(screen,&barreVie,SDL_MapRGB(screen->format,0,0,0));
  barreVie.y=153-vie;
  barreVie.h=vie;
  SDL_FillRect(screen,&barreVie,SDL_MapRGB(screen->format,255,0,0));
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
          if(visionLevier){
            if(typeL = isLevier(mat_perso[int(perso_y+1)][int(perso_x)])){
              mat_perso[int(perso_y+1)][int(perso_x)] += 6*typeL;
              if(typeL==1) openDoor(mat_perso[int(perso_y+1)][int(perso_x)]);
              if(typeL==-1) closeDoor(mat_perso[int(perso_y+1)][int(perso_x)]);
            } else {
              if (typeL = isLevier(mat_perso[int(perso_y)][int(perso_x + 1)])) {
                mat_perso[int(perso_y)][int(perso_x + 1)] += 6 * typeL;
                if (typeL == 1)
                  openDoor(mat_perso[int(perso_y)][int(perso_x + 1)]);
                if (typeL == -1)
                  closeDoor(mat_perso[int(perso_y)][int(perso_x + 1)]);
              } else {
                if (typeL = isLevier(
                        mat_perso[int(perso_y - 1)][int(perso_x)])) {
                  mat_perso[int(perso_y - 1)][int(perso_x)] += 6 * typeL;
                  if (typeL == 1)
                    openDoor(mat_perso[int(perso_y - 1)][int(perso_x)]);
                  if (typeL == -1)
                    closeDoor(mat_perso[int(perso_y - 1)][int(perso_x)]);
                } else {
                  if (typeL = isLevier(
                          mat_perso[int(perso_y)][int(perso_x - 1)])) {
                    mat_perso[int(perso_y)][int(perso_x - 1)] += 6 * typeL;
                    if (typeL == 1)
                      openDoor(mat_perso[int(perso_y)][int(perso_x) - 1]);
                    if (typeL == -1)
                      closeDoor(mat_perso[int(perso_y)][int(perso_x) - 1]);
                  }else{
                    tir=1;
                  }
                }
              }
            }
          }
          if(visionFin){
            if(typeL = mat_perso[int(perso_y+1)][int(perso_x)]=='y'){
              mat_perso[int(perso_y+1)][int(perso_x)] ++;
              victoire=1;
            } else {
              if (typeL = mat_perso[int(perso_y)][int(perso_x + 1)]=='y') {
                mat_perso[int(perso_y)][int(perso_x + 1)] ++;
                victoire=1;
              } else {
                if (typeL = mat_perso[int(perso_y - 1)][int(perso_x)]=='y') {
                  mat_perso[int(perso_y - 1)][int(perso_x)] ++;
                  victoire=1;
                } else {
                  if (typeL = mat_perso[int(perso_y)][int(perso_x - 1)]=='y') {
                    mat_perso[int(perso_y)][int(perso_x - 1)] ++;
                    victoire=1;
                  }
                }
              }
            }
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
      if (perso_angle < 0){
        perso_angle = fmod(perso_angle,2*M_PI);
      }
      perso_angle = perso_angle-M_PI/100;
      break;
    
    case -1:
      if(perso_angle > 2*M_PI){
        perso_angle = fmod(perso_angle,2*M_PI);
      }
      perso_angle=perso_angle+M_PI/100;
      break;
    
  }
}


/*void move_monster(){
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
}*/
void move_monster(){
  float tmp_x,tmp_y,tmp_angle;
  int sens;
  for (int i=0;i<5;i++){
    if(monster[i].vie>0) {
      if (monster[i].dir == 0) {
        monster[i].dir = rand() % 100;
        tmp_angle = fmod(rand(), M_PI / 2);
        sens = rand() % 2;
        if (sens == 1) {
          monster[i].angle = monster[i].angle + tmp_angle;
        } else {
          monster[i].angle = monster[i].angle - tmp_angle;
        }
      }
      tmp_x = monster[i].x + cos(monster[i].angle) * 0.05;
      tmp_y = monster[i].y - sin(monster[i].angle) * 0.05;
      if (isMur(mat_perso[int(tmp_y)][int(tmp_x)]) ||
          isLevier(mat_perso[int(tmp_y)][int(tmp_x)]) ||
          isPorte(mat_perso[int(tmp_y)][int(tmp_x)])) {
        monster[i].dir = 0;
      } else {
        if(mat_perso[int(tmp_y)][int(tmp_x)]=='0'){
          vie-=10;
          if(!vie) defaite=1;
          barreVie.y+=10;
          monster[i].dir = 0;
        }
        mat_perso[int(monster[i].y)][int(monster[i].x)] = ' ';
        monster[i].x = tmp_x;
        monster[i].y = tmp_y;
        mat_perso[int(monster[i].y)][int(monster[i].x)] = '#';
        monster[i].dir--;
      }
    }else{
      mat_perso[int(monster[i].y)][int(monster[i].x)]='$';
    }
  }
}

int main (int argc, char*args[]){
  int i, j;
  char  choix='b';
  do {
    switch (choix) {
      case 'b':
        printf(" _____   _   _____   __   _   _     _   _____   __   _   _   _   _____  \n"
                       "|  _  \\ | | | ____| |  \\ | | | |   / / | ____| |  \\ | | | | | | | ____| \n"
                       "| |_| | | | | |__   |   \\| | | |  / /  | |__   |   \\| | | | | | | |__   \n"
                       "|  _  { | | |  __|  | |\\   | | | / /   |  __|  | |\\   | | | | | |  __|  \n"
                       "| |_| | | | | |___  | | \\  | | |/ /    | |___  | | \\  | | |_| | | |___  \n"
                       "|_____/ |_| |_____| |_|  \\_| |___/     |_____| |_|  \\_| \\_____/ |_____| \n");
        choix='0';
        break;
      case '0':
        printf("1 - Jouer\n2 - Quitter\n");
        choix = getchar();
        break;
      case '1':
        screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        SDL_Init(SDL_INIT_VIDEO);
        pistolet = SDL_LoadBMP("pistolet.bmp");
        murDraw = SDL_LoadBMP("walltext.bmp");
        monstreDraw = SDL_LoadBMP("monstre.bmp");
        casque = SDL_LoadBMP("visionCasque.bmp");
        SDL_SetColorKey(casque, SDL_SRCCOLORKEY,
                        SDL_MapRGB(casque->format, 0, 255, 255));
        SDL_SetColorKey(pistolet, SDL_SRCCOLORKEY,
                        SDL_MapRGB(pistolet->format, 0, 255, 255));
        SDL_SetColorKey(monstreDraw, SDL_SRCCOLORKEY,
                        SDL_MapRGB(monstreDraw->format, 0, 255, 255));
        SDL_SetColorKey(murDraw, SDL_SRCCOLORKEY,
                        SDL_MapRGB(murDraw->format, 0, 255, 255));
        rectPistolet.y = 0;
        rectPistolet.h = pistolet->h;
        posPistolet.w = SCREEN_WIDTH / 4;
        posPistolet.h = SCREEN_WIDTH / 4;
        posPistolet.x = SCREEN_WIDTH / 2 - pistolet->w / 6;
        posPistolet.y = 342 - pistolet->h;
        // create window
        //screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
        // set keyboard repeat
        SDL_EnableKeyRepeat(1, 100);
        
        gameover = 0;
        
        
        srand(time(NULL));
        
        //rempli la matrice avec ' '
        for (i = 0; i < MAP_HEIGHT; i++) {
          for (j = 0; j < MAP_WIDTH; j++) {
            mat_perso[i][j] = ' ';
          }
        }
        FillMat();
        
        while (!gameover && !defaite && !victoire) {
          
          SDL_Event event;
          
          
          if (tir) {
            
            for (int k = 0; k < 45; k++) {
              // look for an event
              if (SDL_PollEvent(&event)) {
                HandleEvent(event);
              }
              deplacer();
              move_monster();
              draw(screen, k / 15);
              SDL_UpdateRect(screen, 0, 0, 0, 0);
            }
          } else {
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
        SDL_FreeSurface(murDraw);
        SDL_FreeSurface(monstreDraw);
        SDL_FreeSurface(pistolet);
        SDL_FreeSurface(casque);
        SDL_Quit();
        if(gameover){
          printf("     ___   _   _        _____    _____   _     _   _____   _   _____   \n"
                         "    /   | | | | |      |  _  \\  | ____| | |   / / /  _  \\ | | |  _  \\  \n"
                         "   / /| | | | | |      | |_| |  | |__   | |  / /  | | | | | | | |_| |  \n"
                         "  / / | | | | | |      |  _  /  |  __|  | | / /   | | | | | | |  _  /  \n"
                         " / /  | | | |_| |      | | \\ \\  | |___  | |/ /    | |_| | | | | | \\ \\  \n"
                         "/_/   |_| \\_____/      |_|  \\_\\ |_____| |___/     \\_____/ |_| |_|  \\_\\ \n");
          choix='2';
        }
        if(defaite){
          printf(" _     _   _____   _   _   _____        _____   _____   _____   _____                 ___  ___   _____   _____    _____  \n"
                         "| |   / / /  _  \\ | | | | /  ___/      | ____| |_   _| | ____| /  ___/               /   |/   | /  _  \\ |  _  \\  |_   _| \n"
                         "| |  / /  | | | | | | | | | |___       | |__     | |   | |__   | |___               / /|   /| | | | | | | |_| |    | |   \n"
                         "| | / /   | | | | | | | | \\___  \\      |  __|    | |   |  __|  \\___  \\             / / |__/ | | | | | | |  _  /    | |   \n"
                         "| |/ /    | |_| | | |_| |  ___| |      | |___    | |   | |___   ___| |            / /       | | | |_| | | | \\ \\    | |   \n"
                         "|___/     \\_____/ \\_____/ /_____/      |_____|   |_|   |_____| /_____/           /_/        |_| \\_____/ |_|  \\_\\   |_|   \n");
          choix='2';
        }
        
        if(victoire){
          sleep(2);
          printf(" _____   _____   _       _   _____   _   _____       ___   _____   _   _____   __   _   _____  \n"
                         "|  ___| | ____| | |     | | /  ___| | | |_   _|     /   | |_   _| | | /  _  \\ |  \\ | | /  ___/ \n"
                         "| |__   | |__   | |     | | | |     | |   | |      / /| |   | |   | | | | | | |   \\| | | |___  \n"
                         "|  __|  |  __|  | |     | | | |     | |   | |     / / | |   | |   | | | | | | | |\\   | \\___  \\ \n"
                         "| |     | |___  | |___  | | | |___  | |   | |    / /  | |   | |   | | | |_| | | | \\  |  ___| | \n"
                         "|_|     |_____| |_____| |_| \\_____| |_|   |_|   /_/   |_|   |_|   |_| \\_____/ |_|  \\_| /_____/ \n");
          choix='2';
        }
        break;
      default:
        choix='2';
        break;
    }
  }while(choix != '2');
  
  return 0;
  
}
