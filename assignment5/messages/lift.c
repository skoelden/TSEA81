#include "lift.h"

/* Simple_OS include */ 
#include <pthread.h>

/* standard includes */ 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* panic function, to be called when fatal errors occur */ 
void lift_panic(const char message[])
{
    printf("LIFT_PANIC!!! "); 
    printf("%s", message); 
    printf("\n"); 
    exit(0); 
}

/* --- monitor data type for lift and operations for create and delete START --- */

/* lift_create: creates and initialises a variable of type lift_type */
lift_type lift_create(void) 
{
    /* the lift to be initialised */
    lift_type lift;

    /* floor counter */ 
    int floor; 

    /* loop counter */
    int i;

    /* allocate memory */
    lift = (lift_type) malloc(sizeof(lift_data_type));

    /* initialise variables */

    /* initialise floor */
    lift->floor = 0; 
    
    /* set direction of lift travel to up */
    lift->up = 1;

    /* the lift is not moving */ 
    lift->moving = 0; 

    /* initialise person information */
    for (floor = 0; floor < N_FLOORS; floor++)
    {
        for (i = 0; i < MAX_N_PERSONS; i++)
        {
            lift->persons_to_enter[floor][i].id = NO_ID; 
            lift->persons_to_enter[floor][i].to_floor = NO_FLOOR; 
        }
    }

    /* initialise passenger information */
    for (i = 0; i < MAX_N_PASSENGERS; i++) 
    {
        lift->passengers_in_lift[i].id = NO_ID; 
        lift->passengers_in_lift[i].to_floor = NO_FLOOR; 
    }

    return lift;
}

/* lift_delete: deallocates memory for lift */
void lift_delete(lift_type lift)
{
    free(lift);
}


/* --- monitor data type for lift and operations for create and delete END --- */


/* --- functions related to lift task START --- */

/* MONITOR function lift_next_floor: computes the floor to which the lift 
   shall travel. The parameter *change_direction indicates if the direction 
   shall be changed */
void lift_next_floor(lift_type lift, int *next_floor, int *change_direction)
{
  int floor;
  int up;
  
  floor = lift->floor;
  up = lift->up;
  
  if(floor == N_FLOORS-1 && !up){
    *next_floor = floor - 1;
    *change_direction = 1;
  } else if (floor == 0 && up){
    *next_floor = floor + 1;
    *change_direction = 1;
  } else {
    *next_floor = floor - 2*up + 1;
    *change_direction = 0;
  }
}

void lift_move(lift_type lift, int next_floor, int change_direction)
{
    /* the lift is moving */ 
    lift->moving = 1; 
        
    /* it takes two seconds to move to the next floor */ 
    // usleep(2000000);
        
    /* the lift is not moving */ 
    lift->moving = 0; 

    /* the lift has arrived at next_floor */ 
    lift->floor = next_floor; 

    /* check if direction shall be changed */ 
    if (change_direction)
    {
        lift->up = !lift->up; 
    }
}

/* this function is used also by the person tasks */ 
int n_passengers_in_lift(lift_type lift)
{
    int n_passengers = 0; 
    int i; 
        
    for (i = 0; i < MAX_N_PASSENGERS; i++)
    {
        if (lift->passengers_in_lift[i].id != NO_ID)
        {
            n_passengers++; 
        }
    }
    return n_passengers; 
}

int lift_should_stop(lift_type lift)
{
  int should_stop = 0;
  int i;

  for (i = 0; i < MAX_N_PASSENGERS; i ++){
    if (lift->passengers_in_lift[i].id != NO_ID)
      if(lift->passengers_in_lift[i].to_floor == lift->floor)
	should_stop = 1;
  }
  for (i = 0; i < MAX_N_PASSENGERS; i ++){
    if ((lift->persons_to_enter[lift->floor][i].id != NO_ID) && n_passengers_in_lift(lift) < MAX_N_PASSENGERS){
      should_stop = 1;
    }
  }

  return should_stop;
}

/* --- functions related to lift task END --- */


/* --- functions related to person task START --- */

/* enter_floor: makes a person with id id stand at floor floor */ 
void enter_floor(lift_type lift, int id, int from_floor, int to_floor)
{
    int i; 
    int floor_index; 
    int found; 

    /* stand at floor */ 
    found = 0; 
    for (i = 0; i < MAX_N_PERSONS && !found; i++)
    {
        if (lift->persons_to_enter[from_floor][i].id == NO_ID)
        {
            found = 1; 
            floor_index = i; 
        }
    }
        
    if (!found)
    {
        lift_panic("cannot enter floor"); 
    }

    /* enter floor at index floor_index */ 
    lift->persons_to_enter[from_floor][floor_index].id = id; 
    lift->persons_to_enter[from_floor][floor_index].to_floor = to_floor; 
}

/* leave_floor: makes a person with id id at enter_floor leave 
   enter_floor */ 
void leave_floor(lift_type lift, int id, int enter_floor)

/* fig_end lift_c_prot */ 
{
    int i; 
    int floor_index; 
    int found; 

    /* leave the floor */
    found = 0; 
    for (i = 0; i < MAX_N_PERSONS && !found; i++)
    {
        if (lift->persons_to_enter[enter_floor][i].id == id)
        {
            found = 1; 
            floor_index = i; 
        }
    }
        
    if (!found)
    {
        lift_panic("cannot leave floor"); 
    }

    /* leave floor at index floor_index */ 
    lift->persons_to_enter[enter_floor][floor_index].id = NO_ID; 
    lift->persons_to_enter[enter_floor][floor_index].to_floor = NO_FLOOR; 
}

void enter_lift(lift_type lift, int id, int destination_floor)
{
  int i;
  int sucess = 0;
  for(i = 0; i < MAX_N_PASSENGERS; i++){
    if(lift->passengers_in_lift[i].id == NO_ID){
      lift->passengers_in_lift[i].id = id;
      lift->passengers_in_lift[i].to_floor = destination_floor;
      sucess = 1;
      break;
    }
  }
  
  if(!sucess){lift_panic("Could not enter lift!");}
}

/* --- functions related to person task END --- */
