/*
PCB has 3 fields
PC : Points to the the current line reached for that program
start: Points to first line of that program
end: Points to last line of that program
*/
/*
typedef struct PCB
{
    int PC;
    int start;
    int end;
}PCB;
*/

typedef struct PCB {
	int PC;
	FILE *PCf;
	int start; // startAddressRAM
	int end;
	struct PCB_REC *next;
	int pageTable[10]; // each cell contains the frame number, 10 because RAM[10]
	int PC_page; // which page the program is currently at
	int PC_offset; // which offset (line) of the page we are at
	int pages_max; // total number of pages
} PCB;


/*
Passes 2 parameters (start , end)
This method creates a PCB with fields set as this:
PC = start
start = start
end = end
*/
PCB *makePCB(FILE *p, int curPage, int offset, int numberOfPages);


PCB *getFromReady();
PCB *getHead();
