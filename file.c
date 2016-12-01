#include <stdio.h>
#include <stdlib.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

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
