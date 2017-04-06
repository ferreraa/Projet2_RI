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

#define fileNameReq "http://mrim.imag.fr/voc10/images/2008_000004.jpg"
#define pathToUrls "urls.txt"
#define fileHisto "histo.txt"


//number of pictures
#define N 9638


KEY images[N];
float histogramme[64]; //l'histogramme du fichier requete


//fileName1 : image comparée.
//h1 : histogramme de l'image recherchée
void distance_euclidienne(FILE *fileName1) {
//  float h1[64];
  float h2[64]; //histogramme de l'image comparée

//  fread(h1, sizeof(float), 64, f1);
  fread(h2, sizeof(float), 64, f2);

  float res = 0;
  int i;
  for(i=0 ; i<64 ; i++) {
    res += (histogramme[i] - h2[i]) * (histogramme[i]-h2[i]);
  }

//  res = sqrtf(res);  //pas besoin pour le tri

  printf("%f\n",res);
  return res;
}

//set element i of the KEY array images[N]. sets the value to distance(FILE * fileTargeted)
void setElementi(int i, FILE * fileTargeted) {
//  printf("setting image %i...\n",i);
  images[i]->k = i;
  images[i]->d = distance_euclidienne(fileTargeted);
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
/*
	int n;
    char** url =readList("urls.txt",&n);
    for (i=0; i<n;i++){
			//calcul de l'histogramme de ce fichier 
*/			
			/*------------------------------------------------*/
			/* lecture d'une image requête                    */
			/*------------------------------------------------*/
//			printf ("Procesing %s ...\n",url[i]);
//			read_cimage(url[i],&cim);
//			printf(" je suis la\n");
			/*initialisation de l'histogramme*/
/*			for (x=0;x<64;x++){
				histogramme[x]=0.0;
			}

			printf("Largeur de l'image : %d\n",cim.nx);
			printf("Heuteur de l'image : %d\n",cim.ny);

			nx=cim.nx;
			ny=cim.ny;

			for (x = 0; x < nx; x++) {       // ligne par ligne 
				for (j = 0; j < ny; j++) {   // pixel par pixel 
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
*/			/*------------------------------------------------*/
/*
			fwrite(histogramme, sizeof (float), 64, FO);
			/*for (i=0;i<64;i++){
				
				//printf ("%f ",histogramme[i]);
			}

	} */


//  FILE* freq = fopen(FileNameReq,'r');
//  fread(histogramme,sizeof(float),64,freq);

	int n;
  char** url =readList("urls.txt",&n);

  printf("urls lus\n");

//-----------------CREATION HISTOGRAMME REQUETE----------

	read_cimage(fileNameReq,&cim);

  printf("Image requete lue\n");

  for(i=0; i<64; i++) {
    histogramme[i]= 0.0;
  }

	printf("Largeur de l'image : %d\n",cim.nx);
	printf("Hauteur de l'image : %d\n",cim.ny);

	nx=cim.nx;
	ny=cim.ny;

	for (x = 0; x < nx; x++) {       // ligne par ligne 
		for (j = 0; j < ny; j++) {   // pixel par pixel 
			//printf("%4d",cim.r[x][j]);
			num=cim.r[x][j]/64+(cim.g[x][j]/64)*4+(cim.b[x][j]/64)*16;
			histogramme[num]++;
		}
		printf (".");
	}

	//normalisation
	for (i=0;i<64;i++){
		histogramme[i]= histogramme[i]/(nx*ny);
	}


//------------------------Fin de creation histogramme requete-------


  File * fHistos = fopen(fileHisto,'r');

  for (i=0; i<n;i++){
    //calcul de l'histogramme de ce fichier 

		/*------------------------------------------------*/
		/* lecture d'une image requête                    */
		/*------------------------------------------------*/
		printf ("Procesing %d/%d ...\n",i,N);

    setElementi(i, fHistos);
    
		printf(" je suis la\n");
	}

  printf("sorting results...\n);

  sort();

  printf("creating res file...\n");

  FILE* Fres = fopen("out.html",'w');

  for(i=0 ; i < n; i++) {
    fprintf(Fres, "<IMG SRC=\"%s\">\n",url[images[i].k]);
  }	


  fclose(Fres);
	fclose(FO);

  exit(0);
}

