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
  images[i].k = i;
  images[i].d = distance_euclidienne(fileTargeted,histogramme);
}


void setElementi_bis( int i , FILE * fileTargeted_c,FILE * fileTargeted_s, float* histogramme_c, float* histogramme_s){
	images[i].k = i;
	hist_size= 64;
	float dc=distance_euclidienne(fileTargeted_c,histogramme_c);
	
	hist_size=256;
	float ds= distance_euclidienne(fileTargeted_s,histogramme_s);
	printf ("dc= %f, ds= %f \n", dc, ds);
	images[i].d = dc+ds/2.0;
 }


// trie les images 
void sort() {
  qsort(images, N, sizeof(KEY), keyCompare);
}



//BIZARRE : prend un nom de fichier et pas une URL ????
// calcule l'histogramme normalisee d'une image sift. 
//parametre un histogramme vide de taille 256, et le nom d'un fichier sift a ouvrir ????
// retourne l'histogramme de l'image dont le nom est passe en parametre normalise


	int process_histogramme_sift(float* histogramme,char* fname) {
		int x;
		FILE * f = fopen(fname, "r");
		if (f!=NULL){
			int num = 0,i=0;
		
			/**initialisation de l'histogramme*/
			for (x=0;x<256;x++){
				histogramme[x]=0.0;
			}
			/**********************************/
			
			/**Calcul de l'histogramme ********/
			while(fscanf(f, "%d",&num)!= EOF) {
				histogramme[num]++;
				i++;
			}
			/**********************************/
			
			
			/**normalisation*******************/
			for (x=0;x<256;x++){
				histogramme[x]= histogramme[x]/(i);
			}
			/**********************************/
		}
		else {
			printf("EROR d'ouverture de l'image\n");
			return 0;
		}
		fclose (f);
		return 1;
	}


int creation_histos_sift(){
	float histogramme[256];
	FILE* FO= fopen ("histo_sift.txt", "w+");
	int i;

	char** nom =readList("list2.txt",&N);
	printf (" N= %i\n", N);

	printf(" allocation des images\n");
    images = malloc(sizeof(KEY)*N);
	
	for (i=0; i<N;i++){
		printf(" Procesing image : %s \n", nom[i]);
		process_histogramme_sift (histogramme,nom[i]);
		fwrite(histogramme, sizeof (float), 256, FO);
	}
	fclose (FO);
	return 1;
}

//créé l'histogramme d'une image donnée	
//PARAMETTRE : un tableau vide de taille 64, une url d'image 
//RETOURNE :l'histogramme de l'image.

	void process_histogramme_couleur (float* histogramme, char * url) {
		int i,j,nx,ny,num,x;

		CIMAGE cim;
		printf ("je lis l'image : url : %s\n", url );
		read_cimage(url,&cim); 

		printf("Image lue\n");

		for(i=0; i<64; i++) {
			histogramme[i]= 0.0;
		}

		nx=cim.nx;
		ny=cim.ny;

		for (x = 0; x < nx; x++) {       // ligne par ligne 
			for (j = 0; j < ny; j++) {   // pixel par pixel 
				num=cim.r[x][j]/64+(cim.g[x][j]/64)*4+(cim.b[x][j]/64)*16;
				histogramme[num]++;
			}
			printf (".");
		}
//		printf (".");
	


		//normalisation
		for (i=0;i<64;i++){
			histogramme[i]= histogramme[i]/(nx*ny);
		}
	}

// cree un fichier contenant tous les histogrammes des images 
//RETOUR : les histogrammes des images sont ecrit dans le fichier histo.txt
void creation_histos_couleur() {

	float histogramme[64];

	FILE* FO= fopen ("histo.txt", "r");
	int i;

	char** url =readList("urls.txt",&N);
	images = malloc(sizeof(KEY)*N); // j'ai l'impression que ca sert a rien ici 


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
	FILE* Freading= fopen ("histo.txt", "r");
	int i;
	float *histogramme = malloc(sizeof(float)*64);
	hist_size = 64;

	process_histogramme_couleur(histogramme,url);
	printf(" j'ai calculé l'histogramme de cette image \n");

	images = malloc(sizeof(KEY)*N);
	
	
	
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

	FILE* Fres = fopen("out.html","a");


	for(i=0 ; i < nb_retour ; i++) {		
		fprintf(Fres, "<IMG SRC=\"%s\">\n",url_list[images[i].k]);
	}	


	fclose(Fres);
	fclose(Freading);
  free(histogramme);
}	
 

void chercher_image_sift(char * nom, int nb_retour, char ** url_list){
	FILE* FO= fopen ("histo_sift.txt", "r");
	int i;
  	float *histogramme = malloc(sizeof(float)*256);
  	hist_size = 256;

	process_histogramme_sift(histogramme,nom);

	images = malloc(sizeof(KEY)*N);
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
		fprintf(Fres, "<IMG SRC=\"%s\">\n",url_list[images[i].k]); 
	}	

	fclose(Fres);
	fclose(FO);
  free(histogramme);
}

void chercher_image_combined(char * url,char * nom,  int nb_retour, char ** url_list){
	
	FILE* FO= fopen ("histo_sift.txt", "r");
	FILE* Freading= fopen ("histo.txt", "r");
	int i;
  	float *histogramme_s = malloc(sizeof(float)*256);
  	float *histogramme_c = malloc(sizeof(float)*64);
  	
	process_histogramme_sift(histogramme_s,nom);
	process_histogramme_couleur(histogramme_c,url);
	
	images = malloc(sizeof(KEY)*N);
	
	FILE * fHistos = fopen(fileHisto,"rb");

	for (i=0; i<N;i++){
   		printf ("Procesing %d/%d ...\n",i,N);
		setElementi_bis(i, Freading, FO, histogramme_c ,histogramme_s); 
		printf(" images[%i].k = %i, images[%i].d = %f\n",i,images[i].k,i,images[i].d);
    }

	
	printf("sorting results...\n");

	sort(); // regarder pour le tri

	printf("creating res file...\n");

	FILE* Fres = fopen("out.html","w");

	for(i=0 ; i < nb_retour; i++) {
		fprintf(Fres, "<IMG SRC=\"%s\">\n",url_list[images[i].k]); 
	}	

	fclose(Fres);
	fclose(FO);
	fclose(Freading);
  free(histogramme_s);
  free(histogramme_c);
  
  
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    printf("Il faut utiliser un argument pour utiliser ce programme.\n\"i\" pour initier les histogrammes couleur des images de la base\n\"s\" pour faire une recherche par couleur\n\"I\" pour initier les SIFTs locaux\n\"S\" pour faire une recherche par descripteur locaux sift\n\"C\" pour faire une recherche combinee\n");
    exit(0);
  }

  if(*argv[1]=='i') {
    //generer l'histogramme des images en fonction de la couleur :
    creation_histos_couleur();
  } else if(*argv[1]=='I') {
    //generer l'histogramme des images en fonction du clustering : 
    printf ("Creation de l'histogramme sift\n");
    creation_histos_sift();
  }
  else {
    int nb_retour;
    char url[300];
    char** url_list =readList("urls.txt",&N);      

    printf ("entrez une url \n");
  
    scanf("%s", url);
    printf("entrez un nombre de retour\n");
    scanf("%i",&nb_retour);

    FILE* Fres = fopen("out.html","w");
    fprintf(Fres,"Requete : <IMG SRC=%s>\nResultats :", url);
    fclose(Fres);
    if(*argv[1] == 's') {
      //chercher une image en fonction des couleurs :
      chercher_image_couleur(url, nb_retour, url_list);
    } else if(*argv[1]=='S') {
      //chercher une image en fonction des cluster :
      chercher_image_sift(url, nb_retour, url_list);
    } else {
      //recherche combinée
		printf ("entrez un nom \n");
		char nom [300];
		scanf("%s", nom);
      
      chercher_image_combined(url,nom, nb_retour, url_list);
    }
  }
  exit(0);    

/*
	//generer l'histogramme des images en fonction de la couleur :
	//TODO a tester 
	//creation_histos_couleur();

	//generer l'histogramme des images en fonction du clustering : 
//	printf ("Creation de l'histogramme sift\n");
//	creation_histos_sift();

	//chercher une image en fonction des couleurs :
	printf ("entrez une url \n");

	
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	scanf("%i",&nb_retour);

  FILE* Fres = fopen("out.html","w");
  fprintf(Fres,"Requete : <IMG SRC=%s>\nResultats :", url);
  fclose(Fres);

	chercher_image_couleur(url, nb_retour, url_list);

	//chercher une image en fonction des cluster :
	printf ("entrez une url \n");
	scanf("%s", url);
	printf("entrez un nombre de retour\n");
	
	scanf("%i",&nb_retour);
	chercher_image_sift(url, nb_retour, url_list);

	//cherche une image en fonction des deux :
//	chercher_image_combined();
	exit(0);
*/
}

