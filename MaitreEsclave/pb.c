#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h" 
#include "point.h"



pb_t *pb_alloc()
{
	pb_t *pb;

	pb = (pb_t *)malloc(sizeof(pb_t));
	pb->taille1 = pb->taille2 = pb->type = 0;
	pb->pts1 = pb->pts2 = NULL;

	return pb;
}

void pb_free(pb)
pb_t *pb;
{
	if (pb->pts1) free(pb->pts1);
	if (pb->pts2) free(pb->pts2);
	free(pb);
}



void send_pb(int tid, pb_t *pb)
{
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&(pb->taille1), 1, 1);
	pvm_pkint(&(pb->taille2), 1, 1);
	pvm_pkint(&(pb->type), 1, 1);
	pvm_pkint(&(pb->pts1->x), pb->taille1, 1);
	pvm_pkint(&(pb->pts1->y), pb->taille1, 1);
	if (pb->taille2 > 0){
		pvm_pkint(&(pb->pts2->x), pb->taille2, 1);
		pvm_pkint(&(pb->pts2->y), pb->taille2, 1);
		}
	pvm_send(tid, MSG_PB);

	pb_free(pb);
}


	

/*
 * reception d'un probleme venant d'un processus tid
 * allocation locale pour le probleme
 * met a jour le le tid de l'envoyeur dans sender 
 * (utile dans le cas ou la reception venait 
 * d'un processus indifferent (tid == -1))
 * retourne NULL si le message n'est pas de type MSG_PB
 */
pb_t *receive_pb(int tid, int *sender)
{
	
	int tag, taille, bufid;

	bufid = pvm_recv(tid, -1);
	pvm_bufinfo(bufid, &taille, &tag, sender);
	if (tag != MSG_PB) return NULL;
	pb_t *pb = pb_alloc();
	pvm_upkint(&(pb->taille1), 1, 1);
	pvm_upkint(&(pb->taille2), 1, 1);
	pvm_upkint(&(pb->type), 1, 1);
	pb->pts1 = point_alloc();
	pvm_upkint(&(pb->pts1->x), pb->taille1, 1);
	pvm_upkint(&(pb->pts1->y), pb->taille1, 1);
	if (pb->taille2 > 0) {
		pb->pts2 = point_alloc();
		pvm_upkint(&(pb->pts2->x), pb->taille2, 1);
		pvm_upkint(&(pb->pts2->y), pb->taille2, 1);
	}

	return pb;
}


