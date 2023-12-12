
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <strings.h>
#include "pvm3.h"
#include "point.h"

static pb_t *Q[PB];		/* la pile de problemes */
static int Q_nb;			/* l'index de tete de pile */

/*
 * remplit un tableau d'entiers aleatoirement
 * dans l'intervalle [0..99]
 * la taille du tableau est donnee
 * par DATA (cf sort.h)
 */


/*
 * initialise la file de problemes
 * chacun des PB problemes est un
 * probleme UH de taille N
 * stocke dans le premier champ de
 * donnees de la structure.
 */

void init_queue(pts)
point *pts;
{
	int i;

	for (i=0; i<PB; i++) {
		Q[i] 		= pb_alloc();
		Q[i]->taille1 = N;
		Q[i]->taille2 = 0;
		Q[i]->pts1 = pb_alloc();
		bcopy((char *)(pts + i*N), (char *)(Q[i]->pts1), N*sizeof(point));
		Q[i]->pts2 = NULL;
		Q[i]->type = PB_UH;
	}
	Q_nb = PB;	
}

/*
 * empile ou depile dans la 
 * pile globale Q un probleme
 */

pb_t *depile()
{
	return Q_nb>0 ? Q[--Q_nb] : NULL;
}

void empile(pb)
pb_t *pb;
{
	if (Q_nb >= PB) {
		fprintf(stderr, "error -- stack overflow\n");
		pvm_exit();
		exit(-1);
	}
	Q[Q_nb++] = pb;
}

/*
 * programme maitre
 */

int main(argc, argv)
int argc; 
char **argv;
{
	int i;
	int tids[P];	/* tids fils */
	point *pts;	/* donnees */
	pb_t *pb;	/* probleme courant */
	int sender[1];

	pts = point_random(DATA);	/* initialisation aleatoire */
	init_queue(pts);	/* initialisation de la pile */
	
	/* Creation de P esclave */
	pvm_spawn(EPATH "/calculs", (char**)0, 0, "", P, tids);

	/* envoi d'un probleme (UH) a chaque esclave */
	for (i=0; Q_nb>0 && i<P; i++)	send_pb(tids[i], depile());

	while (1) {
	pb_t *pb2;

		/* reception d'une solution (type fusion) */
		pb = receive_pb(-1, sender);
		empile(pb);

		/* dernier probleme : si la taille reçu egale au taille initial de données  */
		if (pb->taille1 == DATA)
			break;

		pb = depile(); // depiler un probléme 
		//si le probléme de type calcul d'enveloppe 
		if (pb->type == PB_UH) 
		//on envoie le probléme directement au esclave 
			send_pb(*sender, pb);



		else { // sionn un probléme PB_FUS (fusion) 
			pb2 = depile(); /* dépiler un 2eme pb pour fusion ... */
			if (pb2==NULL) //si le probléme est null , pile vide
				empile(pb); // rien a faire
			else {
				if (pb2->type == PB_UH) { /* sinon si pb2 est un probléme de type calcul d'enveloppe on l'envoie 
											vers l'esclave est en empile le probleme pb */
					empile(pb);
					send_pb(*sender, pb2); // evoi vers l'esclave
				}
				else { //sionon si le probléme est de type fuison
				//on met les deux tableaux dans un seul probléme 
                     pb->taille2 = pb2->taille1;
					pb->pts2 = pb2->pts1;
					send_pb(*sender, pb); /* envoi du probleme a l'esclave */
                     pb_free(pb2);// liberation de probléme pb2
				}
			}
		}
	}


	pvm_initsend(PvmDataDefault);
	pvm_mcast(tids, P, MSG_END); /* fin esclaves */
	point_print_gnuplot(pb->pts1, 0); /* affiche l'ensemble des points restant, i.e*/
    point_print_gnuplot(pb->pts1, 1); 
    pb_free(pb); //liberation de probléme pb
	pvm_exit();
	exit(0);




}
