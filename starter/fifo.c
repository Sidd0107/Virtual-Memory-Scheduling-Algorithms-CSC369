#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;
// Linked list will store frame numbers.
struct  fifoLink{
    unsigned int entry;
    struct fifoLink *next;
};
//Will be used as the head
struct fifoLink *list;
//Stores the size of the linked list
int size;

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
    // Stores frame value
    unsigned int evictedFrame = list->entry;
    //Creates pointer to 1st element
	struct fifoLink *oldest=list;
    // sets list(head of the list) to second element
    list = oldest->next;
    oldest->next=NULL;
    // frees the memory allocated for the first/oldest element
    free(oldest);
    // Reduces size
    size--;
    //Returns evicted frame
	return evictedFrame;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
    //Empty list therefore set values of list.
    if(size==0){
        //Initialized in init just setting the values.
        list->entry=(p->frame >> PAGE_SHIFT);
        size++;
    }else{
        //List not empty
        // All blocks are not filled
        if(size<memsize){
            struct fifoLink *iterator = list;
            while(iterator->next!=NULL){
                if(iterator->entry==(p->frame >> PAGE_SHIFT)){
                    return;
                }else{
                    iterator=iterator->next;
                }
            }
            struct fifoLink * newPage;
            newPage = (struct fifoLink *) malloc( sizeof(struct fifoLink));
            newPage->entry=(p->frame >> PAGE_SHIFT);
            newPage->next=NULL;
            iterator->next=newPage;
            size++;
        }else{
            //Memory blocks are filled
            struct fifoLink *iterator2 = list;
            
            //Checks if element does not exist in list
            while(iterator2->next!=NULL){
                if(iterator2->entry==(p->frame >> PAGE_SHIFT)){
                    return;
                }else{
                    iterator2=iterator2->next;
                }
            }
            //If it does not exist, evict and recursive call.
            fifo_evict();
            struct fifoLink * newPage;
            newPage = (struct fifoLink *) malloc( sizeof(struct fifoLink));
            newPage->entry=(p->frame >> PAGE_SHIFT);
            newPage->next=NULL;
            iterator2->next=newPage;
            size++;
        }
             
        
    }
	return;
}

/* Initialize any data structures needed for this
 * replacement algorithm 
 */
void fifo_init() {
    //Intialization of the primary linked list element.
    list = (struct fifoLink *) malloc( sizeof(struct fifoLink));
    list->next=NULL;
    size=0;
    
}
