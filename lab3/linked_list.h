#ifndef _linked_list_h
#define _linked_list_h

#include "coordinate.h"
#include "definitions.h"

/*
struct particle {
  pcord_t  pcord;
  int ptype;
};

typedef struct particle particle_t;
*/

/*
struct part_cord {
    float x ;
    float y ;
    float vx ;
    float vy ;
} ;

typedef struct part_cord pcord_t ;
*/

typedef struct _plist_elem {
  particle_t this;
  struct _plist_elem* next;
  struct _plist_elem* prev;
} plist_elem;


typedef struct _particle_list {
  plist_elem* first;
  plist_elem* last;
} particle_list;


void append(particle_list* list, plist_elem* elem);
void remove_particle(particle_list* list, plist_elem* elem);
void psize(particle_list* list, int rank);
void init(particle_list* list);
void merge_plists(particle_list* from, particle_list* to);
plist_elem* create_particle(float x, float y, float vx, float vy);
int psize_get(particle_list* list);
#endif
