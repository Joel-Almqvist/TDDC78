#include "linked_list.h"


void init(particle_list* list){
  list->first = NULL;
  list->last = NULL;
}


plist_elem* create_particle(float x, float y, float vx, float vy){
  plist_elem* this = malloc(sizeof(plist_elem));
  this->this.ptype = 0;

  this->this.pcord.x = x;
  this->this.pcord.y = y;
  this->this.pcord.vx = vx;
  this->this.pcord.vy = vy;

  this->next = NULL;

  return this;
}


void append(particle_list* list, plist_elem* elem){
  plist_elem* empty = list->last;

  if(empty == NULL){
    list->first = elem;
    list->last = elem;
  }
  else{
    list->last->next = elem;
    list->last = elem;
  }
}
