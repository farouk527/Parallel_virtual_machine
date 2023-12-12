#include <stdio.h>
#include <stdlib.h>
#include "point.h"
#include "pvm3.h"






// fonction pour copier les x dans un tableau 
 void copie_abs(pts,tab)
      point *pts;
     int tab[point_nb(pts)];
{
	point *pt1;
         int i=0;
for (pt1=pts; pt1!=NULL; pt1=pt1->next) {
                 tab[i]=pt1->x;
                 i++;
}
}





// fonction pour copier les y dans un tableau 
void copie_ord(pts,tab)
     point *pts;
     int tab[point_nb(pts)];
{
point *pt1;
     int i=0;

for (pt1=pts; pt1!=NULL; pt1=pt1->next) {
             tab[i]=pt1->y;
              i++;
}
}



	
static  int compareX(i,j)
point **i, **j;
{ return (*i)->x - (*j)->x; 
}








point *creer_ensemble( nbPts, tabX, tabY)
       int nbPts;
       int tabX[nbPts];
       int tabY[nbPts];

 {int i;
		point **pts;
    	pts = (point **)malloc(nbPts*sizeof(point *));
	for(i=0; i < nbPts; i++) {
		pts[i] = point_alloc();
		pts[i]->x = tabX[i];;
		pts[i]->y = tabY[i];
}
	qsort(pts, nbPts, sizeof(point *),compareX);
	for (i=0; i<nbPts-1; i++)
	pts[i]->next = pts[i+1];
	return  (point *) *pts;
}









main(int argc, char **argv)
{
        
	int mytid;		/* tid tache */
	int parent;		/* tid du pere */
	int nbPoints;	        /* nombre d'elements de l'ensembles */
	int tids[2];		/* tids fils */
       point *pts, *pts2 ;        /*liste de points */
       


	
	parent = pvm_parent();
	if (parent == PvmNoParent) {  //initialisation 

              // initialisation d'un ensemble de point avec l'entier passé en parammetre  
                 pts = point_random(atoi(argv[1]));
               //affichage l'ensemble des points 
               point_print_gnuplot(pts, 0);
				//insialisation de nombre de points dans l'ensemble 
                 nbPoints = point_nb(pts);
    }
	else { // (fils) reception des données de pére 
                
                pvm_recv(parent, MSG_DATA); //recevoir un message de pére (MSG_DATA)
		        pvm_upkint(&nbPoints, 1, 1); // Recevoir  la taille de l'ensemble de points
                int tabX [nbPoints]; //reservation d'un tableau pour les abscisses 
                int tabY [nbPoints];//reservation d'un tableau pour les ordonées 
                pvm_upkint(tabX, nbPoints, 1); // Recevoir l'ensemble de point x
                pvm_upkint(tabY, nbPoints, 1); // Recevoir l'ensemble de point y 
                pts = creer_ensemble(nbPoints,tabX,tabY); //creation d'un ensemble de point a partir 
													 //de tabx et taby 
       }

	

	if (nbPoints > 4) {
		// Création de 2 fils
                pvm_spawn(BPWD "/upper", (char**)0, 0, "", 2, tids);
				//  partitionner l'ensemble de points 
				//pst2 : deuxieme ensemble 
           		 pts2 = point_part(pts); 
			    int taille1=point_nb(pts); //taille 1 contient la taille de la prémiere ensemble 
            	int taille2=point_nb(pts2); //taille 2 contient la taille de la deuxiéme ensemble 
				// reservation de tableau 1
           		int tabPointsX1 [taille1] ; //tableau qui va contenir les x de l'ensemble 1
            	int tabPointsY1 [taille1] ;	// tableau qui va contenir les y de l'ensebmle 1
				//reservation e tableau 2
 			    int tabPointsX2 [taille2] ; //tableau qui va contenir les x de l'ensemble 2
 				int tabPointsY2 [taille2] ; // tableau qui va contenir les y de l'ensebmle 2
				// stockage des x et y dans des tableaux d'entiers, 
                // ensemble de points gauche :
                copie_abs(pts , tabPointsX1); //copie_abs: permettre au tabPointsX1 de conserver les x
            	copie_ord(pts , tabPointsY1); //copie_ord: permettre au tabPointsY1 de conserver les y
				// ensemble de points droite:
                copie_abs(pts2 , tabPointsX2);//copie_abs: permettre au tabPointsX2 de conserver les x
                copie_ord(pts2 , tabPointsY2);//copie_abs: permettre au tabPointsY2 de conserver les y

                       






		// Envoyer vers le fils 1
		pvm_initsend(PvmDataDefault);
	    pvm_pkint(&taille1, 1, 1); // envoyer la taille de sous ensemble
		pvm_pkint(tabPointsX1,taille1, 1); // envoyer les x de sous ensemble
        pvm_pkint(tabPointsY1,taille1, 1); // envoyer les y de sous ensemble
		pvm_send(tids[0], MSG_DATA); // precision de fils via tids [0]
		// Envoyer vers le fils 2
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&taille2, 1, 1); // envoyer la taille de sous ensemble
		pvm_pkint(tabPointsX2,taille2, 1); // envoyer les x de sous ensemble
        pvm_pkint(tabPointsY2,taille2, 1); // envoyer les y de sous ensemble
		pvm_send(tids[1], MSG_DATA); // precision de fils via tids [1]







				// reception deux enveloppes       
                // reception de la part de Fils 1
                pvm_recv(-1, MSG_SORT);
		   		pvm_upkint(&nbPoints, 1, 1); // recevoir la taille de sous ensemble
                int tabX1 [nbPoints];
                int tabY1 [nbPoints];
                pvm_upkint(tabX1, nbPoints, 1); // recevoir les x de sous ensemble
                pvm_upkint(tabY1, nbPoints, 1); // recevoir les y de sous ensemble
                pts = creer_ensemble(nbPoints,tabX1,tabY1); // construire l'ensemble de points a partir les X et les Y
                
                // reception de la part de Fils 2
		        pvm_recv(-1, MSG_SORT);
		        pvm_upkint(&nbPoints, 1, 1); // recevoir la taille de sous ensemble
                int tabX2 [nbPoints];
                int tabY2 [nbPoints];
                pvm_upkint(tabX2, nbPoints, 1); // recevoir les x de sous ensemble
                pvm_upkint(tabY2, nbPoints, 1);// recevoir les y de sous ensemble
                pts2 = creer_ensemble(nbPoints,tabX2,tabY2);// construire l'ensemble de points a partir les X et les Y
				// fusionner les deux sous ensebmle
			    point_merge_UH(pts,pts2); 
             
	}
         


		 else // Calculer l'enveloppe haute directement
            	 pts = point_UH(pts);






	if (parent == PvmNoParent) { // Affichage Resultat
                 
                point_print_gnuplot(pts, 1);
                //point_print(pts, 1); 
	}


	
	else { // renvoi les enveloppe au parents
		        pvm_initsend(PvmDataDefault);
                int taille= point_nb(pts);//declarrer variable qui contient la taille de tab de x et y 
                int tabPointsX1 [taille] ;
                int tabPointsY1 [taille] ;
	            copie_abs(pts , tabPointsX1); //copie_abs: permettre au tabPointsX1 de conserver les x
                copie_ord(pts , tabPointsY1); //copie_ord: permettre au tabPointsY1 de conserver les y

                pvm_pkint(&taille, 1, 1); // envoyer la taille de l'ensemble qui construit l'enveloppe haute
                pvm_pkint(tabPointsX1,taille, 1);  // envoyer les X de l'ensemble qui construit l'enveloppe haute
                pvm_pkint(tabPointsY1,taille, 1);// envoyer les Y de l'ensemble qui construit l'enveloppe haute
		        pvm_send(parent, MSG_SORT);//envoi le MSG_SORT au pére 

	}






	pvm_exit();
	exit(0);
}

