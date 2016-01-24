#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#define MAXLINE 256

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

char buf[10000000];



int vaddrslen=0;
addr_t *vaddrs;
struct  List{
    unsigned int entry;
    int nextAccessIndex;
    struct List *next;
};
int counter;
struct List* list;

FILE* tfp;
FILE* tfp2;
int indexno;


//Will be used as the head
struct List *head;
//Stores the size of the linked list
int size;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	//Go over list and compare next call value and remove based on that
    struct List *iterator = list;
    struct List *previous = NULL;
    //1st element and memsize is also 1
    if(iterator->next==NULL){
        list->entry=-1;
        size--;
        //wont free as it is already initialized in init
    }else{
        int lastIndex=-1;
        struct List *farthest = NULL;//Pointer to element that will be called last
        struct List * prevFarthest = NULL;
        //Not 1 element
        while(iterator->next!=NULL){
            //Check if index is -1. If so remove.
            if(iterator->nextAccessIndex==-1){
                //remove and return frame
                //1st(oldest element
                if(previous==NULL){
                    list=iterator->next;
                    unsigned int evictedFrame = iterator->entry;
                    free(iterator);
                    size--;
                    return evictedFrame;
                }else{
                    //not 1st element
                    //List preserved
                    previous->next=iterator->next;
                    //store's frame, frees and returns
                    unsigned int evictedFrame = iterator->entry;
                    free(iterator);
                    size--;
                    return evictedFrame;
                }
            }else if(iterator->nextAccessIndex<indexno){
                //Index is not -1
                //Check if index is before current idex implying it will have to be updated
                //next access index is less than current index
                int count=indexno+1;
                int currElemind=iterator->nextAccessIndex;
                while(count<counter){
                    if(vaddrs[currElemind]==vaddrs[count]){
                        iterator->nextAccessIndex=count;
                        break;
                    }
                    count++;
                }
                //Now that it is updated if -1 remove else if max make farthest point
                // to this
                //Check if index is -1. If so remove.
                if(iterator->nextAccessIndex==-1){
                    //remove and return frame
                    //1st(oldest element
                    if(previous==NULL){
                        list=iterator->next;
                        unsigned int evictedFrame = iterator->entry;
                        free(iterator);
                        size--;
                        return evictedFrame;
                    }else{
                        //not 1st element
                        //List preserved
                        previous->next=iterator->next;
                        //store's frame, frees and returns
                        unsigned int evictedFrame = iterator->entry;
                        free(iterator);
                        size--;
                        return evictedFrame;
                    }
                }else{
                    // Updated element is referenced again in the future.
                    // Compare with farthest element
                    if(iterator->nextAccessIndex>lastIndex){
                        lastIndex=iterator->nextAccessIndex;
                        farthest=iterator;
                        prevFarthest=previous;
                        previous=iterator;
                        iterator=iterator->next;
                    }else{
                        previous=iterator;
                        iterator=iterator->next;
                    }
                
                }
            }else{
                //Next access not -1 or invalid so valid in coming later
                //Compare with last index
                if(iterator->nextAccessIndex>lastIndex){
                    lastIndex=iterator->nextAccessIndex;
                    farthest=iterator;
                    prevFarthest=previous;
                    previous=iterator;
                    iterator=iterator->next;
                }else{
                    //simply move on
                    previous=iterator;
                    iterator=iterator->next;
                }
            }
        }
        //Program reaches here implies that none of the frames had next reference index = -1.
        //Must remove farthest and set previous to next
        if(prevFarthest!=NULL){
            prevFarthest->next=farthest->next;
        }
        unsigned int evictedFrame = farthest->entry;
        free(farthest);
        size--;
        return evictedFrame;
    }
	return 0;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
    //Empty list therefore set values of list.
    if(size==0){
        //Initialized in init just setting the values.
        list->entry=(p->frame >> PAGE_SHIFT);
        size++;
        indexno=0;
        //Loop through array to find when this will be accessed next.
        int count=indexno+1;
        list->nextAccessIndex=-1;
        while(count<counter){
            if(vaddrs[indexno]==vaddrs[count]){
                list->nextAccessIndex=count;
                break;
            }
            count++;
        }
        
        return;
    }else{
        //increment index no to current index
        indexno++;
        //List not empty
        // All blocks are not filled
        if(size<memsize){
            struct List *iterator = list;
            while(iterator->next!=NULL){
                if(iterator->entry==(p->frame >> PAGE_SHIFT)){
                    //Already in list change next access attr and return
                    int count=indexno+1;
                    iterator->nextAccessIndex=-1;//will be reset below if there is another access
                    //As long as we iterate through every element after index in vaddrs array
                    while(count<counter){
                        //found next access and break
                        if(vaddrs[indexno]==vaddrs[count]){
                            iterator->nextAccessIndex=count;
                            return;
                        }
                        count++;
                    }
                    return;
                }else{
                    iterator=iterator->next;
                }
            }
            //Not in list which is why code reaches here
            struct List * newPage;
            newPage = (struct List *) malloc( sizeof(struct List));
            newPage->entry=(p->frame >> PAGE_SHIFT);
            newPage->next=NULL;
            newPage->nextAccessIndex=-1; //Gets overwritten below if it does gets accessed later
            int count=indexno+1;
            while(count<counter){
                if(vaddrs[indexno]==vaddrs[count]){
                    iterator->nextAccessIndex=count;
                    break;
                }
                count++;
            }
            iterator->next=newPage;
            size++;
        }else{
            //Memory blocks are filled
            struct List *iterator2 = list;
            
            //Checks if element already exists in list
            while(iterator2->next!=NULL){
                if(iterator2->entry==(p->frame >> PAGE_SHIFT)){
                    iterator2->nextAccessIndex=-1;//will be reset below if there is another access
                    int count=indexno+1;
                    while(count<counter){
                        if(vaddrs[indexno]==vaddrs[count]){
                            iterator2->nextAccessIndex=count;
                            return;
                        }
                        count++;
                    }
                    return;
                }else{
                    iterator2=iterator2->next;
                }
            }
            //If it does not exist, evict and recursive call.
            opt_evict();
            struct List * newPage;
            newPage = (struct List *) malloc( sizeof(struct List));
            newPage->entry=(p->frame >> PAGE_SHIFT);
            newPage->next=NULL;
            newPage->nextAccessIndex=-1;//will be reset below if accessed again
            //iterator2->next=newPage;
            //Updates next access value.
            int count=indexno+1;
            while(count<counter){
                if(vaddrs[indexno]==vaddrs[count]){
                    newPage->nextAccessIndex=count;
                    break;
                }
                count++;
            }
            //Need to get pointer to end again cause evict could have removed the last item.
            struct List *iterator3 = list;
            while(iterator3->next!=NULL){
                iterator3=iterator3->next;
            }
            //now iterator 3 points to new last element after eviction
            iterator3->next=newPage;
            size++;
        }
        
    }
    return;

    }


/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
    list = (struct List *) malloc( sizeof(struct List));
    list->entry=-1;
    list->next=NULL;
    size=0;
    //Opens file for reading
    if(tracefile != NULL) {
        if((tfp = fopen(tracefile, "r")) == NULL) {
            perror("Error opening tracefile:");
            exit(1);
        }
        if((tfp2 = fopen(tracefile, "r")) == NULL) {
            perror("Error opening tracefile:");
            exit(1);
        }
    }
    //Gets no of elements in tracefile.
    char type;
    addr_t vaddr;
    counter=0;
    while(fgets(buf, MAXLINE, tfp) != NULL) {
        if(buf[0] != '=') {
            sscanf(buf, "%c %lx", &type, &vaddr);
            //vaddrs[counter]=vaddr;
            counter++;
            
        }
        
    }
    //Sets vaddrlen to be no of vaddresses.
    vaddrslen=counter;
    vaddrs=malloc(counter * sizeof(addr_t));
    int count2=0;
    
    //Reads in every address and stores it.
    while(fgets(buf, MAXLINE, tfp2) != NULL) {
        if(buf[0] != '=') {
            sscanf(buf, "%c %lx", &type, &vaddr);
            vaddrs[count2]=vaddr;
            count2++;
            
        }
        
    }
    
    
    
}

