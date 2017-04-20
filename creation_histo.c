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



int N; //Nombre d'images dans la base
KEY* images;
int hist_size;

//calcule la distance euclidienne entre JE SAIS PAS QUOI et l'histogramme du fichier requete qui en variable globale (BIZARRE)
//PARAMETRE :fileTargeted : image comparée.
//SORTIE : float : distance entre JE SAIS PAS 
float distance_euclidienne(FILE *fileTargeted, float*histogramme) {

//  int hist_size = sizeof(histogramme)/sizeof(float);   aïe aïe aïe, C, tu baisses dans mon estime en m'empêchant de faire ça !

  float h2[hist_size]; //histogramme de l'image comparée

  fread(h2, sizeof(float), hist_size, fileTargeted);

  float res = 0;
  int i;
  for(i=0 ; i<hist_size ; i++) {
    res += (histogramme[i] - h2[i]) * (histogramme[i]-h2[i]);
  }

  return res;
}

 
//set element i of the KEY array images[N]. sets the value to distance(FILE * fileTargeted)
//PARAMETRE : descripteur de fichier filTargeted
void setElementi(int i, FILE * fileTargeted, float* histogramme) {
//  printf("setting image %i...\n",i);
  images[i].k = i;
//  printf ("je suis la \n");
  images[i].d = distance_euclidienne(fileTargeted,histogramme);
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
	printf ("je commencea traiter l'image : %s\n", fname);
	int x;
	printf ("j'ouvre le fichier avec l'image \n");
	FILE * f = fopen(fname, "r");
	
	int num = 0,i=0;
	printf ("initialisation de l'histogramme\n");
	/*initialisation de l'histogramme*/
	for (x=0;x<256;x++){
		histogramme[x]=0.0;
	}

	while(fscanf(f, "%d",&num)!= EOF) {
		histogramme[num]++;
		i++;
	}
	
	printf ("normalisation\n");
	//normalisation
	for (x=0;x<256;x++){
		histogramme[x]= histogramme[x]/(i);
	}
	printf ("je fini de traiter l'image : %s\n", fname);
	
}


void creation_histos_sift(){

	float histogramme[256];
	printf ("Ouverture du fichier de sortie \n");
	FILE* FO= fopen ("histo_sift.txt", "r");
	printf ("Ouverture du fichier de sortie sucess \n");
	int i;
	
    char** nom =readList("../../../list.txt",&N);
    printf (" N= %i\n", N);
    //possible problemme si couleur et sift utilise images
	printf(" allocation des images\n");
  images = malloc(sizeof(KEY)*N);
	printf(" allocation des images sucess\n");
	//char * nom_image;
	//nom_image= malloc (sizeof (char )*3000);
	//nom_image = "sift/test/1nn";	
  for (i=0; i<N;i++){
		//printf("initialisation du nom \n");
		//strcat(nom_image,nom[i]); 
		printf(" Procesing image : %s \n", nom[i]);
		
		process_histogramme_sift (histogramme,nom[i]);
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

//	float *histogramme = malloc(sizeof(float)*64);
  float histogramme[64];

	FILE* FO= fopen ("histo.txt", "r");
	int i;
	//TODO probablement pas a faire ici 
    char** url =readList("urls.txt",&N);
    images = malloc(sizeof(KEY)*N);
    for (i=0; i<N;i++){
		process_histogramme_couleur (histogramme,url[i]);
		fwrite(histogramme, sizeof (float), 64, FO);
	}

	fclose(FO);
}


// cherche une image en fonction de sont histogramme de couleur 
//PARAMETTRE : l'url de l'image requette et le nombre d'image a retourner , se sert du fichier histo.txt prerequis ce fichier contient les histogramme de toute les images de la base
//RETOUR : les images reponses	
void chercher_image_couleur(char * url, int nb_retour, char ** url_list){
	printf ("je commence a chercher l'image couleur\n");
	FILE* Freading= fopen ("histo.txt", "r");
	printf ("j'ai ouvert le fichier histo.txt\n");
	//TODO verifier que ce fichier n'est pas vide 
	int i;
  float *histogramme = malloc(sizeof(float)*64);
  hist_size = 64;

	process_histogramme_couleur(histogramme,url);
	printf(" j'ai calculer l'histogramme de cette image \n");

	images = malloc(sizeof(KEY)*N);
	//TODO possible problemme 
	//FILE * fHistos = fopen(fileHisto,"rb");

	//TODO trouver une solution pour le parcours de ce fichier 
	for (i=0; i<N;i++){
   		printf ("Procesing %d/%d ...\n",i,N);
		setElementi(i, Freading,histogramme);
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
    }

	printf("sorting results...\n");

	sort();
	printf ("\n\n");
	for (i=0;i <N;i++){
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
	}
	printf ("\n\n");
	printf("creating res file...\n");

	FILE* Fres = fopen("out.html","w");

//	for(i=N-1 ; i > N-nb_retour+1; i--) {
	for(i=0 ; i < nb_retour ; i++) {		
		fprintf(Fres, "<IMG SRC=\"%s\">\n",url_list[images[i].k]);
	}	


	fclose(Fres);
	fclose(Freading);
  free(histogramme);
}	


void chercher_image_sift(char * url, int nb_retour, char ** url_list){
	FILE* FO= fopen ("histo_sift.txt", "r");
	int i;
//	float histogramme[256]; 
  float *histogramme = malloc(sizeof(float)*256);
  hist_size = 256;

	process_histogramme_sift(histogramme,url);

	FILE * fHistos = fopen(fileHisto,"rb");

	for (i=0; i<N;i++){
   		printf ("Procesing %d/%d ...\n",i,N);
		setElementi(i, fHistos, histogramme);
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
    }

	printf("sorting results...\n");

	sort();

	printf("creating res file...\n");

	FILE* Fres = fopen("out.html","w");

	for(i=0 ; i < nb_retour; i++) {
		fprintf(Fres, "<IMG SRC=\"%s\">\n",url_list[images[i].k]); //TODO trouver l'url de l'image n° images[i].k
	}	

	fclose(Fres);
	fclose(FO);
  free(histogramme);
}

void chercher_image_combined(){
//TODO
}

int main(int argc, char *argv[])
{
	int nb_retour;
	char url[300];
	char** url_list =readList("urls.txt",&N);

	//generer l'histogramme des images en fonction de la couleur :
	//creation_histos_couleur();

	//generer l'histogramme des images en fonction du clustering : 
//	printf ("Creation de l'histogramme sift\n");
//	creation_histos_sift();

	//chercher une image en fonction des couleurs :
	printf ("entrez une url \n");
	
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	scanf("%i",&nb_retour);
	chercher_image_couleur(url, nb_retour, url_list);

	//chercher une image en fonction des cluster :
	//printf ("entrez une url \n");
	//scanf("%s", url);
	//printf("entrez un nombre de retour\n");
	
	//scanf("%i",&nb_retour);
	//chercher_image_sift(url, nb_retour, url_list);

	//cherche une image en fonction des deux :
//	chercher_image_combined();
	exit(0);
}

