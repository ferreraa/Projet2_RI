/* regarder la valeur pour le rouge le bleu et le vert 
diviser par 64
multiplier le rouge par 4
le vert par 16
additionnner tout le monde 
on as un nombre n entre 0 et 63

tab[n]++;

on normalise ensuite tab
* 
* 
*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "proc.h"
#include "rdjpeg.h"


//il faut mettre un nom de fileNameReq
//3/0;
#define fileNameReq "imageReq.png"
#define pathToUrls "urls.txt"

//number of pictures
#define N 9638


KEY images[N];




double distance_euclidienne(char *fileName1, char *fileName2){
	float h1[64];
	float h2[64];

  FILE* f1 = fopen(fileName1, "r");
  FILE* f2 = fopen(fileName2, "r");

  fread(h1, sizeof(float), 64, f1);
  fread(h2, sizeof(float), 64, f2);

  float res = 0;
  int i;
  for(i=0 ; i<64 ; i++) {
    res += (h1[i] - h2[i]) * (h1[i]*h2[i]);
  }

//  res = sqrtf(res);  //pas besoin pour le tri

  printf("%f\n",res);
  return res;
}

//set element i of the KEY array images[N]. sets the value to distance(fileReq, fileTargeted)
void setElementi(int i, char * fileTargeted) {
//  printf("setting image %i...\n",i);
  images[i].k = i;
  images[i].d = distance_euclidienne(fileNameReq, fileTargeted);
}


void sort() {
  qsort(images, N, sizeof(KEY), keyCompare);
}

int main(int argc, char *argv[])
{

	int i,j,nx,ny,num,x;

	CIMAGE cim;
	float histogramme[64];

	FILE* FO= fopen ("histo.txt", "w");
	int n;
    char** url =readList("urls.txt",&n);
    for (i=0; i<n;i++){
			//calcul de l'histogramme de ce fichier 
			
			/*------------------------------------------------*/
			/* lecture d'une image requête                    */
			/*------------------------------------------------*/
			printf ("Procesing %s ...\n",url[i]);
			read_cimage(url[i],&cim);
			printf(" je suis la\n");
			/*initialisation de l'histogramme*/
			for (x=0;x<64;x++){
				histogramme[x]=0.0;
			}

			printf("Largeur de l'image : %d\n",cim.nx);
			printf("Heuteur de l'image : %d\n",cim.ny);

			nx=cim.nx;
			ny=cim.ny;

			for (x = 0; x < nx; x++) {       /* ligne par ligne */
				for (j = 0; j < ny; j++) {   /* pixel par pixel */
					//printf("%4d",cim.r[x][j]);
					num=cim.r[x][j]/64+(cim.g[x][j]/64)*4+(cim.b[x][j]/64)*16;
					histogramme[num]++;
				}
				printf (".");
			}
			
			printf ("je fini de generer l'histo\n");

			//normalisation
			for (x=0;x<64;x++){
				histogramme[x]= histogramme[x]/(nx*ny);
			}
			/*------------------------------------------------*/

			fwrite(histogramme, sizeof (float), 64, FO);
			/*for (i=0;i<64;i++){
				
				//printf ("%f ",histogramme[i]);
			}*/

	}
	fclose(FO);

  exit(0);
}

