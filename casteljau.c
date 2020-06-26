#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

//gcc -o "casteljau.out" casteljau.c `sdl2-config --cflags --libs`

typedef struct Point Point;
struct Point{
	double x;
	double y;
};

#define HAUTEUR_FENETRE 1000
#define LARGEUR_FENETRE 1500


SDL_Window* pWindow = NULL;
SDL_Renderer* renderer;

int indexPoint;
size_t nbPoint;
size_t precision;
size_t epaisseur;
Point* tabp;
Point* pointActuel;


void toStringP(Point* p) {
	printf("x: %.2f, y: %.2f\n", p->x, p->y);
}

void afficherPoint(Point p, int epaisseur, int r, int g, int b){
	SDL_SetRenderDrawColor(renderer, r, g, b,   255);
	if (epaisseur <= 1) {
		epaisseur=1;
		SDL_RenderDrawPoint(renderer, p.x, p.y);
	} else {
		SDL_Rect rect;
		rect.x = p.x-(epaisseur/2); rect.y = p.y-(epaisseur/2);
		rect.w = epaisseur; rect.h = epaisseur;
		SDL_RenderFillRect(renderer, &rect);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0,   255);
}


/*
 * retourne le point millieu entre p1 et p2 dans buffer
 */
void millieu(Point p1, Point p2, Point* pbuffer){
	pbuffer->x = (p1.x + p2.x)/2;
	pbuffer->y = (p1.y + p2.y)/2;
}

void millieuv2(Point* p1, Point* p2, Point* pbuffer){
	pbuffer->x = (p1->x + p2->x)/2;
	pbuffer->y = (p1->y + p2->y)/2;
}


/*
 * Retourne les points généré par l'algorithme de Casteljau dans buffer.
 *
 * Par exemple pour 3 points donné (p1, p2 et p3), Il retournera les 5 points p1, p12, p123, p23 et p3;
 * où p12 est le point millieu de p1 et p2, et p123 est le point de millieu de p12 et p23, ainsi de suite. (voir schéma ci-dessous)
 *
 *	p1
 *		millieu(p1,p2) > p12
 *	p2							millieu(p12,p23) > p123
 *		millieu(p2,p3) > p23
 *	p3
 */
void getPointCalteljau(Point* tp, int size, Point* buffer){
	if (size == 1) {
		buffer[0] = tp[0];
	} else {
		buffer[0] = tp[0];
		buffer[(size*2)-2] = tp[size-1];

		for (size_t i=0; i<size-1; i++) {
			//millieu(tp[i], tp[i+1], tp+i);
			millieuv2(tp+i, tp+(i+1), tp+i);
			//tp+1->x
			//tp[1].x
		}
		getPointCalteljau(tp, size-1, buffer+1);
	}
}

/*
 * Trace la courbe de Bezier à partir des points d'origine avec l'aide des points de Casteljau.
 *
 * Pour chaque suite de points de Casteljau reçu, on raplique la fonction sur les n/2+1 premiers points,
 * puis sur les n/2 dernier points; ainsi de suite jusqu'à une certaine precision donnée ce qui permet de
 * tracer la courbe d'une precision plus ou moins fidèle.
 */
/*
void CasteljauRec(Point* tpinit, int sizei, int precision) {
	if (precision == 0) {
		for (size_t i=0; i<sizei; i++) {
			afficherPoint(tpinit[i], epaisseur, 255, 255, 255);

			//SDL_Delay(10);
			//SDL_RenderPresent(renderer);
		}
	}
	else {
		Point* buffer = malloc(sizeof(Point) * ((sizei*2)-1) );
		getPointCalteljau(tpinit, sizei, buffer);

		CasteljauRec(buffer, sizei, precision-1);
		CasteljauRec(buffer+(sizei-1), sizei, precision-1);
		free(buffer);
	}
}*/

void CasteljauRec(Point* tpinit, int sizei, int precision) {
	if (precision == 0) {
		for (size_t i=0; i<sizei; i++) {
			afficherPoint(tpinit[i], epaisseur, 255, 255, 255);
			//SDL_Delay(10);
			//SDL_RenderPresent(renderer);
		}
	}
	else {
		Point newPoint[(sizei*2)-1];
		getPointCalteljau(tpinit, sizei, newPoint);

		CasteljauRec(newPoint, sizei, precision-1);
		CasteljauRec(newPoint+(sizei-1), sizei, precision-1);
	}
}


/*
 * Appel CasteljauRec avec une copie des points d'origines pour ne pas influencer leurs coordonnées.
 */
void Casteljau(Point* tpinit, int sizei, int prec) {
	Point* tpinit_copy = malloc(sizeof(Point) * sizei);
	for (size_t i=0; i<sizei; i++) {
		tpinit_copy[i] = tpinit[i];
	}
	CasteljauRec(tpinit_copy, sizei, prec);
	free(tpinit_copy);
}


/*
 * Permet d'obtenir le bon index du prochain point quand on déplace la molette.
 */
int getNouvelIndex(int indexPoint, int nbPas, int limite) {
	if (((indexPoint+nbPas) < 0)) {
		return (indexPoint+nbPas)%limite + limite;
	}
	else {
		return (indexPoint+nbPas)%limite;
	}
}

/*
 * Retrace la courbe et les points d'origine avec les coordonnées actuelles.
 */
void actualiserCourbe() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0,   255);
	SDL_RenderClear(renderer);

	Casteljau(tabp, nbPoint, precision);
	afficherPoint(*pointActuel, 20, 0, 255, 0);
	for (size_t i=0; i<nbPoint; i++) {
		afficherPoint(tabp[i], 10, 255, 0, 0);
	}
	SDL_RenderPresent(renderer);
}


void afficherAideParametres(){
	printf("Paramètres possibles et cumulables :\n");
	printf("\'n\' suivi du nombre associé pour changer le nombre de point (4 par defaut).\n");
	printf("\'p\' suivi du nombre associé pour changer la précision de la courbe (8 par defaut).\n");
	printf("\'e\' suivi du nombre associé pour changer l'épaisseur en pixel du trait de la courbe (3 par defaut).\n");
	printf("(Exemple: $./casteljau.out n5 p8 e2)\n");
	printf("Utilisez --help pour avoir plus de renseignement sur le programme.\n\n");
}
void afficherAideProgramme(){
	printf("Contrôles du programme :\n");
	printf("- Molette du haut/bas pour changer de point sélectionné.\n");
	printf("- Clique gauche pour déplacer le point sélectionné.\n\n");
}



int main(int argc, char const *argv[])
{
	int quit = 0;
	SDL_Event event;
	int holdClic = 0;
	//int actualiser;
	time_t t;
	struct timespec start, end;

	for (size_t i=1; i<argc; i++) {
		if (strlen(argv[i]) > 1) {
			if (argv[i][0] == 'n') {
				nbPoint = (unsigned)atoi(argv[i]+1);
			}
			else if (argv[i][0] == 'p') {
				precision = (unsigned)atoi(argv[i]+1);
			}
			else if (argv[i][0] == 'e') {
				epaisseur = (unsigned)atoi(argv[i]+1);
			}
			else if (strcmp(argv[i], "--help") == 0) {
				printf("\n");
				afficherAideParametres();
				afficherAideProgramme();
				printf("Page GitHub consacrée au projet : https://github.com/hugo-bec/casteljau\n");
				printf("Programme développé par Hugo BEC.\n\n");
				exit(0);
			}
			else {
				printf("Erreur: \'%c\' argument invalide.\n", argv[i][0]);
				afficherAideParametres();
				return EXIT_FAILURE;
			}
		} else {
			printf("Erreur: l'argument doit être suivi d'une valeur entière.\n");
			afficherAideParametres();
			return EXIT_FAILURE;
		}
	}

	if (nbPoint == 0) {
		nbPoint = 4;
	}
	if (precision == 0) {
		precision = 8;
	}
	if (epaisseur == 0) {
		epaisseur = 3;
	}

	indexPoint = 0;
	tabp = malloc(sizeof(Point) * nbPoint);
	pointActuel = tabp+indexPoint;

	if (SDL_Init(SDL_INIT_VIDEO) != 0 ) {
    	fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
		return EXIT_FAILURE;
    }

	pWindow = SDL_CreateWindow("Casteljau",
	SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    LARGEUR_FENETRE,
	HAUTEUR_FENETRE,
	SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if(pWindow) {
		renderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Création du renderer
		if(renderer == NULL) {
		   printf("Erreur lors de la creation d'un renderer : %s",SDL_GetError());
		   return EXIT_FAILURE;
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255,   255);

		srand((unsigned) time(&t));	// Intializes random number generator
		for(size_t i=0 ; i<nbPoint ; i++) {
			tabp[i].x = rand() % LARGEUR_FENETRE;
			tabp[i].y = rand() % HAUTEUR_FENETRE;
		}
		actualiserCourbe();


		while (!quit) {
			//actualiser = 0;
			while (SDL_PollEvent(&event)){
				switch(event.type) {
			        case SDL_QUIT: // Clic sur la croix
			            quit=1;
			            break;

					case SDL_MOUSEBUTTONDOWN:
						if (SDL_BUTTON_LEFT) {
							holdClic = 1;
						} break;

					case SDL_MOUSEBUTTONUP:
						if (SDL_BUTTON_LEFT) {
							holdClic = 0;
						} break;

					case SDL_MOUSEWHEEL:
						indexPoint = getNouvelIndex(indexPoint, event.wheel.y, nbPoint);
						pointActuel = tabp+indexPoint;
						//actualiser = 1;
						break;

				}
				if (holdClic) {
					pointActuel->x = event.button.x;
					pointActuel->y = event.button.y;
					//actualiser = 1;
				}
			}

			//if (actualiser) {
				actualiserCourbe();
			//}
		}

		SDL_DestroyWindow(pWindow);
	}
	else {
		fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Quit();

	return 0;

}
