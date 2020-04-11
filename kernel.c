#include<stdio.h>
#include<stdlib.h>
#include"shell.h"
#include"pcb.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include "memorymanager.h"
/*
This is a node in the Ready Queue implemented as 
a linked list.
A node holds a PCB pointer and a pointer to the next node.
PCB: PCB
next: next node
*/
typedef struct ReadyQueueNode {
    PCB*  PCB;
    struct ReadyQueueNode* next;
} ReadyQueueNode;

ReadyQueueNode* head = NULL;
ReadyQueueNode* tail = NULL;
int sizeOfQueue = 0;


void boot(){
	initRAM(); // there are no pages of code in RAM
	system("rm -r ./BackingStore ; mkdir BackingStore");
}

int main(int argc, char const *argv[])
{
    boot();
    shellUI();
}
/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB* pcb) {
    ReadyQueueNode* newNode = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    newNode->PCB = pcb;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    sizeOfQueue++;
}

/*
Returns the size of the queue
*/
int size(){
    return sizeOfQueue;
}

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop(){
    PCB* topNode = head->PCB;
    ReadyQueueNode * temp = head;
    if (head == tail){
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }
    free(temp);
    sizeOfQueue--;
    return topNode;
}

/*
Passes a filename
Opens the file, copies the content in the RAM.
Creates a PCB for that program.
Adds the PCB on the ready queue.
Return an errorCode:
ERRORCODE 0 : NO ERROR
ERRORCODE -3 : SCRIPT NOT FOUND
ERRORCODE -5 : NOT ENOUGH RAM (EXEC)
*/

int myinit(FILE *p, int currentPage, int offset, int maxPage){
    // Open the filename to get FILE *
    if (p == NULL) return -3;
    int start;
    int end;
    addToRAM(p,&start,&end);// call addToRam on that File *
    fclose(p);
    if (start == -1) return -5;   // If error (check via start/end variable), return that error
    PCB* pcb = makePCB(p, currentPage, offset, maxPage);     // Else create pcb using MakePCB
	if (pcb != NULL) {
		addToReady(pcb); // Then add it to the ReadyQueue
		return 1;
	}
    return 0;
}


void terminate(PCB *p) {
	
	resetRAM();
	free(p);
}


int scheduler() {
	PCB *pcb;
	int result;

	// initialize CPU
	initCPU();

	int i=0;
	// execute the processes
	while(getHead() != NULL) {
		pcb = getFromReady();

		if (pcb != NULL) {
			//setCPU(ram[pcb->pageTable[(pcb->PC_page)]], pcb->PC_offset);			//take care of this. do CPU.PC = pcb->PC_offset;
			
			result = run(2);
		
			if (result != 0){
				removeFromRam(pcb->start,pcb->end);
            			free(pcb);
				
			} 
			else{
				if(getCPUoffset() > 3){		// or: == 4
					
					//PAGEFAULT
					pcb->PC_page++;
					if(pcb->PC_page > pcb->pages_max-1){
						//Terminate program
						terminate(pcb);
					}else{

						int frame = pcb->pageTable[(pcb->PC_page)]; // to be used as index for ram[index]
						//printf("Pagefault checking: %d\n", frame);
						if(frame != -1){
							pcb->PCf = ram[frame];
							pcb->PC_offset = 0;
							addToReady(pcb);
						}
						else{
							fseek(pcb->PCf, 0, SEEK_SET);
							int frameNumber = -1;
        					int victimFrame = -1;
        
        					frameNumber = findFrame();
        					if(frameNumber == -1) victimFrame = findVictim(pcb);

							updatePageTable(pcb, pcb->PC_page, frameNumber, victimFrame);
							loadPage(pcb->PC_page, findPage(pcb->PC_page, pcb->PCf), frameNumber);
							pcb->PC = ram[frame];
							pcb->PC_offset = 0;

							addToReady(pcb);



						}
					}

				}else{
					pcb->PC_offset = getCPUoffset();
					addToReady(pcb);
					
				}
			}
		} 	
	}
	return result;
}



/*
int scheduler(){
    // set CPU quanta to default, IP to -1, IR = NULL
    CPU.quanta = DEFAULT_QUANTA;
    CPU.IP = -1;
    while (size() != 0){
        //pop head of queue
        PCB* pcb = pop();
        //copy PC of PCB to IP of CPU
        CPU.IP = pcb->PC;

        int isOver = FALSE;
        int remaining = (pcb->end) - (pcb->PC) + 1;
        int quanta = DEFAULT_QUANTA;

        if (DEFAULT_QUANTA >= remaining) {
            isOver = TRUE;
            quanta = remaining;
        }

        int errorCode = run(quanta);

        if ( errorCode!=0 || isOver ){
            removeFromRam(pcb->start,pcb->end);
            free(pcb);
        } else {
            pcb->PC += DEFAULT_QUANTA;
            addToReady(pcb);
        }
    }
    // reset RAM
    resetRAM();
    return 0;
}
*/
/*
Flushes every pcb off the ready queue in the case of a load error
*/
void emptyReadyQueue(){
    while (head!=NULL){
        ReadyQueueNode * temp = head;
        head = head->next;
        free(temp->PCB);
        free(temp);
    }
    sizeOfQueue =0;
}

