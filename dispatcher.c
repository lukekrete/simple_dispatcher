/*
NOTE:  MY QUEUE FUNCTIONS AND QUEUE STRUCTURE WERE TAKEN FROM MY CP264 NOTES

Lucas Krete
kret8350@mylaurier.ca
160 758 350
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define EXIT 3
//Global Time to keep track of elapsed time
//makes it easier for time interval calculations
int globalTime = 0;

//Program structure
struct Program{
   int time_running;
   int time_ready;
   int time_blocked;
   int status;   //status are as follows:  0 = ready, 1 = running; 2 = blocked, 3 = exited;
   int id;
   struct Program *next;
};
//Queue Structure
struct Queue {
   struct Program *front;
   struct Program *last;
   unsigned int size;
};
//returns size of the queue
int getsize(struct Queue *q){
   return q->size;
}
//***CONSTRUCTOR METHOD FOR QUEUE
void init(struct Queue *q) {
   q->front = NULL;
   q->last = NULL;
   q->size = 0;
}
//CONSTRUCTOR METHOD FOR PROGRAM
void init_p(struct Program *p) {
   p->time_running = 0;
   p->time_ready = 0;
   p->time_blocked = 0;
   p->status = 0;
   p->id = 0;
   p->next = NULL;
}
//Determines the difference in time from the last
//entry to the current entry
//used as a private method
int time_interval(int t){
   int time_return = t - globalTime;
   globalTime = t;
   return time_return;
}
//Loops through all programs in the Program queue
//and determines which time to increment based on status
                        //entered time
void update_times(struct Queue *q, int time){
   struct Program *current;
   current = q->front;
   int time2 = time_interval(time);
   while(current != NULL){
      //if current->status = 0{...}......else if current->status = 4{...}
      switch(current->status){
         case 0:
            current->time_ready += time2;
            break;
         case 1:
            current->time_running += time2;
            break;
         case 2:
            current->time_blocked += time2;
            break;
         // case 3: program has been exited, do nothing
      }
      current = current->next;
   }
}
//pushes a program onto the Queue given a id and status
void Enqueue(struct Queue *q, int id, int status) {
   q->size++;
   if (q->front == NULL) {
      q->front = (struct Program *) malloc(sizeof(struct Program));
      init_p(q->front);
      q->front->id = id;
      q->front->status = status;
      q->front->next = NULL;
      q->last = q->front;
   } else {
      q->last->next = (struct Program *) malloc(sizeof(struct Program));
      init_p(q->last->next);
      q->last->next->id = id;
      q->last->next->status = status;
      q->last->next->next = NULL;
      q->last = q->last->next;
   }
}
//determines if the Queue is empty or not
int isEmpty(struct Queue* pQueue) {
    if (pQueue == NULL) {
        return FALSE;
    }
    if (pQueue->size == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}
//Take the front program out of the Queue and return it
struct Program * Dequeue(struct Queue *pQueue) {
    /*the queue is empty or bad param*/
    struct Program *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->front;
    pQueue->front = (pQueue->front)->next;
    pQueue->size--;
   // printf("%d",item->time_running);
    return item;
}



int main(void){
   /*-------TESTING QUEUE FUNCTIONS-----------------------------------
   struct Queue q;
   init(&q);
   Enqueue(&q,0,1);
   Enqueue(&q, 1, 0);
   Enqueue(&q,3,9);
   if(isEmpty(&q)){
      printf("\nEMPTY\n");
   }else{
      printf("\nNOT EMPTY\n");
   }
   struct Program *d;
   d = Dequeue(&q);
   printf("ID: %d,   Status: %d\n",d->id, d->status);
   d = Dequeue(&q);
   printf("ID: %d,   Status: %d\n",d->id, d->status);
   d = Dequeue(&q);
   printf("ID: %d,   Status: %d\n",d->id, d->status);
   if(isEmpty(&q)){
      printf("EMPTY");
   }-----------------------------------------------------------------
   */
   //Create ready Queue
   struct Queue ready_q;
   init(&ready_q);
   //Create Queue to hold all created programs
   struct Queue programs_q;
   init(&programs_q);
   //USER INPUT------------------------------------------------------
   char str[1024];
   fgets(str, sizeof(str),stdin);
      const char s[15] = " \n\0\t";
      char *time_c, *event_c, *id_c, *rsc_c;
      int time, id, rsc, running = 0;
      //Break up line----------
      time_c = strtok(str, s);
      event_c = strtok(NULL,s);
      id_c = strtok(NULL,s);
      rsc_c = strtok(NULL, s);
      //Convert to int---------
      time = atoi(&time_c[0]);
      if (&id_c[0] != NULL){
      id = atoi(&id_c[0]);
      }
      if(rsc_c != NULL){
         rsc = atoi(&rsc_c[0]);
         //If requesting resource, flip id/resource so the values are correct
         int temp;
         temp = rsc;
         rsc = id;
         id = temp;
   }
   //-------------------------------------------------------------------------
   //push system idle process onto the queue and immediately set to running
   Enqueue(&programs_q, 0, 1);
   //while not a null line
   while(strlen(str) != 1){
            update_times(&programs_q, time);   //tested multiple times, works perfectly!
         //switch case for all possible entries of the event
         switch(event_c[0]){
            case 'C':{
               Enqueue(&programs_q, id, READY);
               Enqueue(&ready_q, id, READY);
               if(getsize(&ready_q) == 1 && programs_q.front->status == RUNNING){
                  programs_q.front->status = READY;
                  struct Program *x = Dequeue(&ready_q);
                  struct Program *current;
                  current = programs_q.front;
                  while(current->id != x->id){
                     current = current->next;
                  }
                  current->status = RUNNING;
               }
               break;
            }
            case 'R':{
               struct Program *current = NULL;
               current = programs_q.front->next;
               while(current != NULL && current->id != id){
                  current = current->next;
               }
               if (current != NULL){
                  current->status = BLOCKED;
               }
               if(getsize(&ready_q) >0){
                  struct Program *prodigy;
                  prodigy = Dequeue(&ready_q);
                  current = programs_q.front->next;
                  while(current->id != prodigy->id){
                     current = current->next;
                  }
                  current->status = RUNNING;
               }else{
                  programs_q.front->status = RUNNING;
               }
               break;
            }
            case 'I':{
               //printf("I");
               struct Program *current;
               current = programs_q.front->next;
               while(current != NULL && current->id != id){
                  current = current->next;
               }
               if(current != NULL){
                  current->status = READY;
                  if(getsize(&ready_q)>0 || programs_q.front->status != RUNNING){
                  Enqueue(&ready_q,id,READY);
               }else{
                  programs_q.front->status = READY;
                  current->status = RUNNING;
               }
               }
               
               break;
               }
            case 'T':{
               //printf("T");
            struct Program *current;
               current = programs_q.front->next;
               while(current != NULL && current->status != RUNNING){  //If current status is not RUNNING and the one it's on is also not system idle
                  current = current->next;
               }
               //printf("HERE");
               if(current != NULL){
                  current->status = READY;
                  Enqueue(&ready_q, current->id, current->status);
                  struct Program *prodigy;
                  prodigy = Dequeue(&ready_q);
                  current = programs_q.front;
                  while(current->id != prodigy->id){
                     current = current->next;
                  }
                  current->status = RUNNING;
               }else{
                  programs_q.front->status = RUNNING;
               }

               break;
            }
            case 'E':{
               //printf("E");
               struct Program *current;
               current = programs_q.front->next;
               while(current != NULL && current->id != id){
                  current = current->next;
               }
               if (current != NULL){
                  current->status = EXIT;
               }
               if(getsize(&ready_q) >0){
                  struct Program *prodigy;
                  prodigy = Dequeue(&ready_q);
                  current = programs_q.front->next;
                  while(current->id != prodigy->id){
                     current = current->next;
                  }
                  current->status = RUNNING;
               }else{
                  programs_q.front->status = RUNNING;
               }
               break;
            }
         }
         //must flush stdin or else it holds onto hidden characters
         //Was a weird time figuring this out
         fflush(stdin);
      fgets(str, sizeof(str),stdin);
      //Break up line----------
         time_c = strtok(str, " ");
         event_c = strtok(NULL," ");
         id_c = strtok(NULL," ");
         rsc_c = strtok(NULL, " ");
         //Convert to int---------
         time = atoi(&time_c[0]);
         if(id_c != NULL){
         id = atoi(&id_c[0]);
         if(rsc_c != NULL){
            rsc = atoi(&rsc_c[0]);
            //If requesting resource, flip id/resource so the values are correct
            int temp;
            temp = rsc;
            rsc = id;
            id = temp;
         }
         }
         
      }
      struct Program *test;
      test = Dequeue(&programs_q);
      printf("%d %d\n",test->id, test->time_running);
   while(getsize(&programs_q) != 0){
         test = Dequeue(&programs_q);
         printf("%d %d %d %d\n",test->id, test->time_running, test->time_ready, test->time_blocked);
      }  
      //test = Dequeue(&programs_q);
      //printf("%d %d %d %d",test->id, test->time_running, test->time_ready, test->time_blocked);
         
}