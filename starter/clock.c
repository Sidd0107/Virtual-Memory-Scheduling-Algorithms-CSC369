#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

struct  List{
    unsigned int entry;
    int refBit;
    struct List *next;
};


//Will be used as the head
struct List *head;
//Stores the size of the linked list
int size;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
    struct List *iter=head;
    struct List *previous = NULL;
    unsigned int evictedFrame=-100;
    struct List *last = head;
    //sets pointer to last element.
    while(last->next!=NULL){
        last=last->next;
    }
    //iterating through items till 1st refbit=0 is found
    while(iter->refBit==1){
        //There is only 1 element
        if(iter->next==NULL){
            iter->refBit=0;
            break;
        }else{
            //1st element but not last(there are more elements)
            //set ref bit=0
            iter->refBit=0;
            //set head to 2nd item
            head=iter->next;
            //points last item's next to iter
            last->next=iter;
            //sets iter next to null
            iter->next=NULL;
            // sets iter as last element
            last=iter;
            // Sets iter to point back to the 1st element.
            iter=head;
        }
    }
    //Reaches here when iter element's ref bit is 0.
    //Stores evicted frame
    evictedFrame=iter->entry;
    //Sets head=next item and free's iter
    head=iter->next;
    free(iter);
    //reduces size and returns
    size--;
    return evictedFrame;
	
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
    if(size==0){
        head->entry=(p->frame >> PAGE_SHIFT);
        size++;
        return;
    }else{
        //size is not 0
        struct List * iterator=head;
        //checks if p exists in the current list.
        while(iterator!=NULL){
            //exists in list
            if(iterator->entry==(p->frame >> PAGE_SHIFT)){
                //Sets ref bit and returns
                iterator->refBit=1;
                return;
            }else{
                iterator=iterator->next;
            }
        }
        //does not exist in list-that is why the code reaches here.
        if(size==memsize){
            //List is full eviction needed.
            clock_evict();
        }
        // Now list size is fine.
        struct List * last;
        last=head;
        //set pointer to last element
        while(last->next!=NULL){
            last=last->next;
        }
         //Adds new element at the back of the linked list.
        struct List *newPageEntry;
        newPageEntry = (struct List *) malloc( sizeof(struct List));
        newPageEntry->entry=(p->frame >> PAGE_SHIFT);
        newPageEntry->refBit=0;
        newPageEntry->next=NULL;
        last->next=newPageEntry;
        //increments size
        size++;

    }
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
    head = (struct List *) malloc( sizeof(struct List));
    head->next=NULL;
    size=0;
    
    
    
    
}
