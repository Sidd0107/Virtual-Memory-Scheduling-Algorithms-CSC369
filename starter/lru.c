#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

struct  lruList{
    unsigned int entry;
    struct lruList *next;
};
//Will be used as the head
struct lruList *head;
//Stores the size of the linked list
int size;


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
    //Memory blocks are filled
    struct lruList *lruPage = head;
    struct lruList *previous = NULL;
    //Checks if element does not exist in list
    while(lruPage->next!=NULL){
        previous=lruPage;
        lruPage=lruPage->next;
    }
    unsigned int evictedFrame=lruPage->entry;
    previous->next=NULL;
    //free's last element
    free(lruPage);
    size--;
	return evictedFrame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
    //Empty list
    if(size==0){
        //Add to the head of the linked list
        head->entry=(p->frame >> PAGE_SHIFT);
        size++;
        return;
    }else{
        //List not empty
        struct lruList *iterator2 = head;
        //Checks if first element(MRU) is being referenced and return cause no change needed
        if(iterator2->entry==(p->frame >> PAGE_SHIFT)){
            return;
        }
        //Checks if element exists in rest of the linked list
        while(iterator2->next!=NULL){
            //If entry exists in linked list
            if(iterator2->next->entry==(p->frame >> PAGE_SHIFT)){
                // Move entry to begining(MRU position) and connects the previous
                // one to the next entry and returns.
                struct lruList *tempValue = iterator2;
                struct lruList *tempNext = iterator2->next;
                tempValue->next=tempNext->next;
                tempNext->next=head;
                head=tempNext;
                return;
            }else{
                iterator2=iterator2->next;
            }
        }
        //Reaches here implying entry does not exist in Linked list
        //Sets new element to be added.
        struct lruList *newPageEntry;
        //allocates memory for newPage to store a Linked list entry.
        newPageEntry = (struct lruList *) malloc( sizeof(struct lruList));
        // Sets entry to frame number and
        newPageEntry->entry=(p->frame >> PAGE_SHIFT);
        //Checks if linked list is not full
        if(size<memsize){
            // Adds entry in front
            newPageEntry->next=head;
            head=newPageEntry;
            size++;
        }else if(size==memsize){
            //List is full
            //Evict and add in front
            lru_evict();
            newPageEntry->next=head;
            head=newPageEntry;
            size++;
        }
    }
	return;
}


/* Initialize any data structures needed for this
 * replacement algorithm 
 */
void lru_init() {
    //Intialization of the primary linked list element.
    head = (struct lruList *) malloc( sizeof(struct lruList));
    head->next=NULL;
    size=0;

}
