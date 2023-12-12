

#define N			4		/* taille maximale d'un probleme de calcul d'eveloppe */
#define PB			11		/* nombre initial de pbs de tri (= DATA / N) */
#define DATA		44		/* taille des points initiales */

#define P			5			/* nombre d'esclaves */
#define SLAVE 		"sorts"	/* nom du programme esclave */



#define MSG_PB	 0
#define MSG_END	 1 


#define PB_UH 0
#define PB_FUS 1

#define XMAX (1024)
#define YMAX (1024)


/* 
 * structure associee a chaque point 
 */
typedef struct st_point point;
struct st_point {
	int x, y;
	point *next;	/* liste chainee des points de l'enveloppe */
};

/* structure de probleme */
struct st_pb {			
	int taille1, taille2;
	int type;
	point *pts1;
	point *pts2;
};

typedef struct st_pb pb_t;

/*
 * dans point.c
 * utilitaire de calcul pour le TAD point
 */
extern point *point_alloc();
extern void point_free();
extern void point_print();
extern point *point_random();
extern point *point_UH();
extern int point_nb();
extern point *point_part();
extern point *point_merge_UH();
extern void upper_hull();

/* A définir dans point.c pour le probleme */
extern pb_t *pb_alloc();
extern void pb_free();
extern void send_pb();
extern pb_t *receive_pb();
extern point *copy_point();


