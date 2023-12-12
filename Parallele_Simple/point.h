

#define MSG_DATA	0	
#define MSG_SORT	1

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



