#include <stdio.h>
#include <stdlib.h>

#define MAP_WIDTH 12
#define MAP_HEIGHT 12

char map[MAP_WIDTH*MAP_HEIGHT+1]="\
############\
#          #\
#          #\
#          #\
#  #       #\
####       #\
#     #    #\
#     ##   #\
# ##   #   #\
# ##   #   #\
#  #   #   #\
############";

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
    printf("FATAL_ERROR_403\n");
  }
}

int main()
{
  GetMap();

  return 0;
}
