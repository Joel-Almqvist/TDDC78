#include "linked_list.h"
#include <stdbool.h>
#include <stdio.h>


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

void psize(particle_list* list){
  int i = 0;

  plist_elem* elem = list->first;
  while(true){

    if(elem == NULL){
      break;
    }

    ++i;
    elem = elem->next;
  }
  printf("List size is %i\n", i);


}

void append(particle_list* list, plist_elem* elem){
  elem->next = NULL;

  if(list->last == NULL){
    list->first = elem;
    list->last = elem;
    elem->prev = NULL;
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
    if(elem->next == NULL){
      list->last = NULL;
    }
    else{
      elem->next->prev = NULL;
    }
    elem->next = NULL;
    elem->prev = NULL;
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
    elem->next->prev = elem->prev;
  }

  elem->next = NULL;
  elem-> prev = NULL;
}
