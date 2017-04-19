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

//calcule la distance euclidienne entre JE SAIS PAS QUOI et l'histogramme du fichier requete qui en variable globale (BIZARRE)
//PARAMETRE :fileTargeted : image comparée.
//SORTIE : float : distance entre JE SAIS PAS 
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
//PARAMETRE : descripteur de fichier filTargeted
void setElementi(int i, FILE * fileTargeted) {
  printf("setting image %i...\n",i);
  images[i].k = i;
  images[i].d = distance_euclidienne(fileTargeted);
}


// trie les images 
void sort() {
  qsort(images, N, sizeof(KEY), keyCompare);
}



//BIZARRE : prend un nom de fichier et pas une URL ????
// calcule l'histogramme normalisee d'une image sift. 
//parametre un histogramme vide de taille 256, et le nom d'un fichier sift a ouvrir ????
// retourne l'histogramme de l'image dont le nom est passe en parametre normalise
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
	float histogramme[256];
	FILE* FO= fopen ("histo_sift.txt", "r");
	int n,i;
	n=100;
    //char** url =readList("urls.txt",&N);
    
    //comment on recupere le nom des images ????
    
    //possible problemme si couleur et sift utilise images
    images = malloc(sizeof(KEY)*N);
    for (i=0; i<n;i++){
		process_histogramme_sift (histogramme,"nom");
		fwrite(histogramme, sizeof (float), 256, FO);
	}
}

//créé l'histogramme d'une image donnée	
//PARAMETTRE : un tableau vide de taille 64, une url d'image 
//RETOURNE :l'histogramme de l'image.
void process_histogramme_couleur (float* histogramme, char * url) {
	int i,j,nx,ny,num,x;

	CIMAGE cim;
	printf ("j'essaie de lire l'image : url : %s\n", url );
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

// cree un fichier contennat tout les histogramme des images 
//PARAMETTRE: ecrit dans le fichier histo.txt, et lit la liste des urls dans le fichier urls.txt
//RETOUR : les histogrammes des images sont ecrit dans le fichier histo.txt
void creation_histos_couleur() {

	float histogramme[64];
	FILE* FO= fopen ("histo.txt", "r");
	int i;
    char** url =readList("urls.txt",&N);
    images = malloc(sizeof(KEY)*N);
    int n=100;
    for (i=0; i<n;i++){
		process_histogramme_couleur (histogramme,url[i]);
		fwrite(histogramme, sizeof (float), 64, FO);
	}
}


// cherche une image en fonction de sont histogramme de couleur 
//PARAMETTRE : l'url de l'image requette et le nombre d'image a retourner , se sert du fichier histo.txt prerequis ce fichier contient les histogramme de toute les images de la base
//RETOUR : les images reponses	
void chercher_image_couleur(char * url, int nb_retour){
	printf ("je commence a chercher l'image couleur\n");
	FILE* FO= fopen ("histo.txt", "r");
	printf ("j'ai ouvert le fichier histo.txt\n");
	//TODO verifier que ce fichier n'est pas vide 
	int i;
	float histogramme[64]; 
	process_histogramme_couleur(histogramme,url);
	printf(" je calcule l'histogramme de cette image \n");
	//TODO possible problemme 
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
	//TODO changer ca 
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
		fprintf(Fres, "<IMG SRC=\"%i\">\n",url[images[i].k]); //TODO trouver l'url de l'image n° images[i].k
	}	

	fclose(Fres);
	fclose(FO);
}

void chercher_image_combined(){
//TODO
}

int main(int argc, char *argv[])
{
	int nb_retour;
	char url[300];
	//generer l'histogramme des images en fonction de la couleur :
	//creation_histos_couleur();

	//generer l'histogramme des images en fonction du clustering : 
	//creation_histos_sift();

	//chercher une image en fonction des couleurs :
	printf ("entrez une url \n");
	
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	scanf("%i",&nb_retour);
	chercher_image_couleur(url, nb_retour);

	//chercher une image en fonction des cluster :
//	printf ("entrez une url \n");
//	scanf("%s", url);
//	printf("entrez un nombre de retour\n");
	
//	scanf("%i",&nb_retour);
//	chercher_image_sift(url, nb_retour);

	//cherche une image en fonction des deux :
//	chercher_image_combined();
	exit(0);
}

