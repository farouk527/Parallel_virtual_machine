
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h" 
#include "point.h"





point *point_alloc()
{
	point *pt;

	pt = (point *)malloc(sizeof(point));
	pt->x = pt->y = 0;
	pt->next = NULL;

	return pt;
}

/*
 * dealloue une liste de points
 */
void point_free(point *pts)
{
	if (!pts) return;
	point_free(pts->next);
	free(pts);
}

/*
   static int compareX(i,j)

   comparaison des abscisses de 2 points
   pour la fonction qsort
 */

static int compareX(point **i, point **j)

{ return (*i)->x - (*j)->x; }

/* 
   Construit un ensemble de points d'abscisse comprise
   entre 0 et XMAX et d'ordonnee entre 0 et YMAX. 
   Tous les points ont des abscisses et des ordonnees 
   differentes. Les points sont trie's par ordre croissant
   des abscisses.
 */

point *point_random(int nbPts)/* nombre de points a` tirer */
{
	int i,j;
	point **pts;

	pts = (point **)malloc(nbPts*sizeof(point *));
	for(i=0; i < nbPts; i++) {
		pts[i] = point_alloc();
		do {
			pts[i]->x = rand() % XMAX;
			for(j=0; j<i && pts[i]->x!=pts[j]->x; j++);
		} while( j != i );
		do {
			pts[i]->y = rand() % YMAX;
			for(j=0; j<i && pts[i]->y!=pts[j]->y; j++);
		} while( j != i );
	}

	qsort(pts, nbPts, sizeof(point *), (__compar_fn_t) compareX);
	for (i=0; i<nbPts-1; i++)
		pts[i]->next = pts[i+1];

	return (point *)*pts;
}

/*
 * genere une liste de points, avec ligne si solid == 1
 * generation au format jgraph
 */

void point_print(point *pts, int solid)
	/* solid: enveloppe ou non */
{
	if (!solid) {
		printf("newgraph\nlegend top\n");
	}

	printf("newcurve pts\n");
	while (pts) {
		printf("%d %d\n",pts->x, pts->y);
		pts=pts->next;
	}
	if (solid)
		printf("linetype solid\ntitle\n");
}

// au format gnuplot, dans 2 fichiers.
// appele ensuite gnuplot 
void point_print_gnuplot(point *pts, int solid)
	/* solid: enveloppe ou non */
{
	FILE *fp;
	if (!solid) 
		fp = fopen("points_gnuplot", "w");
	else
		fp = fopen("uh_gnuplot", "w");
	while (pts) {
		fprintf(fp, "%d %d\n",pts->x, pts->y);
		pts=pts->next;
	}
	fclose(fp);
	if (solid) {
		fp = fopen("cmd_gnuplot", "w");


		fprintf(fp, "set terminal pdf\n");
		fprintf(fp, "set output \"upper_hull.pdf\"\n");
		fprintf(fp, "plot \"points_gnuplot\" ps 2, \"uh_gnuplot\" with lines lw 4\n");

		fclose(fp);
		system("gnuplot cmd_gnuplot");
		sleep(1);
		//system("ps2pdf upper_hull.ps");
		system("rm cmd_gnuplot points_gnuplot uh_gnuplot");
		//system("rm cmd_gnuplot points_gnuplot uh_gnuplot upper_hull.ps");
		printf ("courbe dans upper_hull.pdf\n");
		printf ("evince upper_hull.pdf\n");
	}
}


/*
 * nombre de points dans une liste
 */
int point_nb(point *pts)
{
	int nb;
	point *cur;

	for (cur=pts, nb=0; cur != NULL; cur = cur->next, nb++);

	return nb;
}

/*
 * partitionne une liste en deux.
 * retourne le milieu de la liste 
 */

point *point_part(point *pts)
{
	int nb, next;
	point *cur, *mid, *midpred;

	midpred = NULL;
	for (mid=cur=pts, next=0; cur!=NULL; cur = cur->next, next=1-next)
		if (next) {
			midpred = mid;
			mid = mid->next;
		}
	if (midpred)
		midpred->next = NULL;
	return mid;
}


/* pente definie par 2 points */
#define PENTE(pt1,pt2) ( (float)((pt2)->y - (pt1)->y) / (float)((pt2)->x-(pt1)->x) )


/* 
   Calcule l'enveloppe convexe haute de 4 points au plus.
   Les points de l'enveloppe sont lies par une liste
   Les autres sont elimines de la liste.
 */

point *point_UH(point *pts) /* ensemble des points a` "envelopper" */
{
	int nb;
	point *cur, *tmp;

	nb = point_nb(pts);
	if (nb < 3) return pts;
	if (nb > 4) return NULL;

	if (nb == 3) {
		if (PENTE(pts, pts->next) < PENTE(pts, pts->next->next)) {
			tmp = pts->next;
			pts->next = tmp->next;
			free(tmp);
		}
		return pts;
	}

	cur = pts;
	if ( ( PENTE(cur, cur->next) < PENTE(cur, cur->next->next) ) ||
			( PENTE(cur, cur->next) < PENTE(cur, cur->next->next->next) ) ) {
		/* le deuxieme n'est pas sur l'enveloppe */
		tmp = cur->next;
		cur->next = tmp->next;
		free(tmp);
	}
	else
		cur = cur->next;

	if ( PENTE(cur, cur->next) < PENTE(cur, cur->next->next) ) {
		/* le troisieme point n'est pas sur l'enveloppe */
		tmp = cur->next;
		cur->next = tmp->next;
		free(tmp);
	}

	return pts;
}

/*
 * retourne 1 si la droite (pt1,pt2) est tangente
 * a l'enveloppe pts
 * (pt2 appartient a pts)
 */
int point_is_tangent(point *pt1, point *pt2, point *pts)
{
	point *next, *prev;
	float pente;

	for (prev=pts; prev!=NULL; prev=prev->next)
		if (prev->next == pt2) break;
	next = pt2->next;

	pente = PENTE(pt1, pt2);

	return 	((prev ? PENTE(prev,pt2)>=pente : 1) && 
			(next ? PENTE(pt2,next)<=pente : 1)) ? 1 : 0;
}

point *point_tangent(point *pt, point *pts)
{
	point *pt2;

	for (pt2=pts; pt2!=NULL; pt2=pt2->next)
		if (point_is_tangent(pt, pt2, pts)) return pt2;

	if (pt2==NULL) {
		printf("Warning : no tangent : \n");
		point_print(pt, 0);
		point_print(pts, 1);
		exit(-1);
	}
	return NULL;
}

/*
 * retourne dans ppt1 et ppt2 les points de tangence
 * de la tangeante commune des deux enveloppes pts1 et pts2
 */
void point_common_tangent(point *pts1, point *pts2, point **ppt1, point **ppt2)
{
	point *pt1, *pt2;

	for (pt1=pts1; pt1!=NULL; pt1=pt1->next) {
		pt2 = point_tangent(pt1, pts2);
		if (point_is_tangent(pt2, pt1, pts1))
			break;
	}

	*ppt1 = pt1;
	*ppt2 = pt2;
}

/*
 * calcule l'enveloppe convexe commune de 
 * deux enveloppes convexes
 */
point *point_merge_UH(point *pts1, point *pts2)
{
	point *pt1, *pt2, *cur, *tmp;

	point_common_tangent(pts1, pts2, &pt1, &pt2);
	point_free(pt1->next);
	for (cur=pts2; cur!=pt2; cur=tmp) {
		tmp = cur->next;
		free(cur);
	}
	pt1->next = pt2;
	return pts1;
}




