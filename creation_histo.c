


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
#include "proc.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "rdjpeg.h"

int main(int argc, char *argv[])
{
	int i,j,nx,ny,num;
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
			for (i=0;i<64;i++){
				histogramme[i]=0.0;
			}

			printf("Largeur de l'image : %d\n",cim.nx);
			printf("Heuteur de l'image : %d\n",cim.ny);

			nx=cim.nx;
			ny=cim.ny;

			for (j = 0; j < nx; j++) {       /* ligne par ligne */
			for (i = 0; i < ny; i++) {   /* pixel par pixel */
				printf("%4d",cim.r[i][j]);
				num=cim.r[i][j]/64+(cim.g[i][j]/64)*4+(cim.b[i][j]/64)*16;
				histogramme[num]++;
			}
			}

			//normalisation
			for (i=0;i<64;i++){
				histogramme[i]= histogramme[i]/(nx*ny);
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

