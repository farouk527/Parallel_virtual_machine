#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h"
#include "point.h"


/*
 * programme esclave
 */


void calcul_pointUH(pb_t *pb)
{
	pb->pts1 = point_UH(pb->pts1);
	pb->type = PB_FUS;
}


/*
 * fusion
 */

void merge_data(pb)
	pb_t *pb;
{
	pb->taille1 = pb->taille1 + pb->taille2; 	
	pb->pts1 = point_merge_UH(pb->pts1,pb->pts2);	
    point_free(pb->pts2);
	pb->pts2 = NULL;
	pb->taille2 = 0;	


}


void main()
{
	extern pb_t *receive_pb();
	int parent, sender[1];	/* pere et envoyeur (non utilise) */
	pb_t *pb;					/* probleme courant */

	parent = pvm_parent();

	/* tant que l'on recoit un probleme a traiter ... */
	while ((pb = receive_pb(parent, sender)) != NULL) {
		if (pb->type == PB_UH) /* on traite suivant le cas */
			calcul_pointUH(pb);
		else 
			merge_data(pb);
		/* et on revoie la solution */
		send_pb(parent, pb);
	}

	pvm_exit();
	exit(0);
}

