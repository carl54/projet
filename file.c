#include <stdio.h>
#include <stdlib.h>

#define MAP_WIDTH 60
#define MAP_HEIGHT 60

/*
 * elements classes dans l'ordre ascii
 * ` = mur
 * agms = levier monte, baisse, porte violet fermee, ouvert
 * bhnt = levier monte, baisse, porte orange fermee, ouvert
 * ciou = levier monte, baisse, porte blanc fermee, ouvert
 * djpv = levier monte, baisse, porte vert fermee, ouvert
 * ekqw = levier monte, baisse, porte marron fermee, ouvert
 * flrx = levier monte, baisse, porte noir fermee, ouvert
 * ^ = mur cassable
 * y = crane fin du jeu
 */
char map[MAP_WIDTH*MAP_HEIGHT+1]="\
````````````````````````````````````````d```````````````````\
`         `    `       m   `   `          `                `\
`         `  ` `       ``  ` ` `          `                `\
`         `  `         ``  ` ` `          `                `\
`         ^  `         ``  ` ` `          `                `\
`         ````         `b  ` ` `          `                `\
`         `a    ```````````` ` `          `                `\
`````n``````````             ` `          `                `\
`         ``               ``` `          `                `\
`         ^o               ``` `          `                `\
`         `````            ``` `````````  `                `\
`         ^# c`            ``` ^          `                `\
`         `````````````````````````````````                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
``````p````                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`         `                                                `\
`````y`````                                                `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
`                                                          `\
````````````````````````````````````````````````````````````";

void GetMap()
{
  int i;
  FILE* fichier = NULL;
  fichier = fopen("map.vuz","w");
  if(fichier !=NULL)
    {
      for (i=0;i<MAP_WIDTH*MAP_HEIGHT+1;i++)
	{
	  fputc(map[i],fichier);
	}
      printf("truc\n");
      fclose(fichier);
    }
  else{
    printf("FATAL_ERROR_404\n");
  }
}

int main()
{
  GetMap();

  return 0;
}
