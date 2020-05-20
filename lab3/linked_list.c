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
  this->prev = NULL;

  return this;
}


void append(particle_list* list, plist_elem* elem){
  plist_elem* empty = list->last;

  if(empty == NULL){
    list->first = elem;
    list->last = elem;
  }
  else{
    elem->prev = list->last;
    list->last->next = elem;
    list->last = elem;

  }
}



void remove_particle(particle_list* list, plist_elem* elem){

  if(list->first == NULL){
    return;
  }

  // Removing our first element
  if(elem->prev == NULL){
    list->first = elem->next;

    // Removing our only element
    if(list->last == elem){
      list->last = NULL;
    }
    elem->next = NULL;
    elem-> prev = NULL;
    return;
  }

  // Removing the last element (not our only one)
  else if(elem->next == NULL){
    elem->prev->next = NULL;
    list->last = elem->prev;
  }

  // Removing middle element
  else{
    elem->prev->next = elem->next;
  }
  
  elem->next = NULL;
  elem-> prev = NULL;
}
