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



int N;
KEY* images;
float histogramme[64]; //l'histogramme du fichier requete


//fileTargeted : image comparée.
float distance_euclidienne(FILE *fileTargeted) {
  float h2[64]; //histogramme de l'image comparée

  fread(h2, sizeof(float), 64, fileTargeted);

  float res = 0;
  int i;
  for(i=0 ; i<64 ; i++) {
    res += (histogramme[i] - h2[i]) * (histogramme[i]-h2[i]);
  }
  return res;
}

//set element i of the KEY array images[N]. sets the value to distance(FILE * fileTargeted)
void setElementi(int i, FILE * fileTargeted) {
  printf("setting image %i...\n",i);
  images[i].k = i;
  images[i].d = distance_euclidienne(fileTargeted);
}


void sort() {
  qsort(images, N, sizeof(KEY), keyCompare);
}



void process_histogramme_sift(float* histogramme,char* fname) {
	int x;
	FILE * f = fopen(fname, "r");
	
	int num = 0,i=0;

	/*initialisation de l'histogramme*/
	for (x=0;x<256;x++){
		histogramme[x]=0.0;
	}

	while(fscanf(f, "%d",&num)!= EOF) {
		histogramme[num]++;
		i++;
	}
	//normalisation
	for (x=0;x<256;x++){
		histogramme[x]= histogramme[x]/(i);
	}
	
}


void creation_histos_sift(){
	
}

//créé l'histogramme d'une image donnée	
void process_histogramme_couleur (float* histogramme, char * url) {
	int i,j,nx,ny,num,x;

	CIMAGE cim;

	read_cimage(url,&cim);

	printf("Image lue\n");

	for(i=0; i<64; i++) {
		histogramme[i]= 0.0;
	}

	printf("Largeur de l'image : %d\n",cim.nx);
	printf("Hauteur de l'image : %d\n",cim.ny);

	nx=cim.nx;
	ny=cim.ny;

	for (x = 0; x < nx; x++) {       // ligne par ligne 
		for (j = 0; j < ny; j++) {   // pixel par pixel 
			num=cim.r[x][j]/64+(cim.g[x][j]/64)*4+(cim.b[x][j]/64)*16;
			histogramme[num]++;
		}
		printf (".");
	}

	//normalisation
	for (i=0;i<64;i++){
		histogramme[i]= histogramme[i]/(nx*ny);
	}
}


void creation_histos_couleur() {

	float histogramme[64];
	FILE* FO= fopen ("histo.txt", "r");
	int n,i;
    char** url =readList("urls.txt",&N);
    images = malloc(sizeof(KEY)*N);
    for (i=0; i<n;i++){
		process_histogramme_couleur (histogramme,url[i]);
		fwrite(histogramme, sizeof (float), 64, FO);
	}
}


	
void chercher_image_couleur(char * url, int nb_retour){
	FILE* FO= fopen ("histo.txt", "r");
	int i;
	float histogramme[64]; 
	process_histogramme_couleur(histogramme,url);

	FILE * fHistos = fopen(fileHisto,"rb");

	//TODO trouver une solution pour le parcours de ce fichier 
	int n=100;
	for (i=0; i<n;i++){
   		printf ("Procesing %d/%d ...\n",i,N);
		setElementi(i, fHistos);
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
    }

	printf("sorting results...\n");

	sort();

	printf("creating res file...\n");

	FILE* Fres = fopen("out.html","w");

	for(i=0 ; i < nb_retour; i++) {
		fprintf(Fres, "<IMG SRC=\"%i\">\n",url[images[i].k]);
	}	


	fclose(Fres);
	fclose(FO);
}	


void chercher_image_sift(char * url, int nb_retour){
		FILE* FO= fopen ("histo_sift.txt", "r");
int i;
	float histogramme[256]; 
	process_histogramme_sift(histogramme,url);

	FILE * fHistos = fopen(fileHisto,"rb");
	for (i=0; i<n;i++){
   		printf ("Procesing %d/%d ...\n",i,N);
		setElementi(i, fHistos);
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
    }

	printf("sorting results...\n");

	sort();

	printf("creating res file...\n");

	FILE* Fres = fopen("out.html","w");

	for(i=0 ; i < nb_retour; i++) {
		fprintf(Fres, "<IMG SRC=\"%i\">\n",url[images[i].k]); //TODO trouver l'url de l'image n° images[i].k
	}	


	fclose(Fres);
	fclose(FO);
}

void chercher_image_combined(){
}

int main(int argc, char *argv[])
{
	int nb_retour;
	char url[300];
	//generer l'histogramme des images en fonction de la couleur :
	creation_histos_couleur();
	//generer l'histogramme des images en fonction du clustering : 
	creation_histos_sift();
	//chercher une image en fonction des couleurs :
	printf ("entrez une url \n");
	
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	scanf("%i",&nb_retour);
	chercher_image_couleur(url, nb_retour);
	//chercher une image en fonction des cluster :
	printf ("entrez une url \n");
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	
	scanf("%i",&nb_retour);
	chercher_image_sift(url, nb_retour);
	//cherche une image en fonction des deux :
	chercher_image_combined();
	exit(0);
}

