#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"

#define DEFAULT_QUANTA 2

struct CPU { 
    int IP; 
    char IR[1000]; 
    int quanta; 
    int offset;
} CPU;

void initCPU() {
	CPU.quanta = 2;
	CPU.IP     = 0;
	CPU.IR[0]  = '\0';
	CPU.offset = 0;
}

void setCPU(int PC, int offset) {
	CPU.IP    = PC;
	CPU.IR[0] = '\0';
	CPU.offset = offset;
}

int getCPUoffset(){
	return CPU.offset;
}

/*
This method passes a quanta.
It will then executes quanta number of lines in RAM as from the address
pointed by the CPU IP.
Returns an errorCode.
*/
int run(int quanta){
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    if (quanta > DEFAULT_QUANTA ){
        quanta = DEFAULT_QUANTA;
    }

    for (int i = 0; i < quanta; i++)
    {
        strcpy(CPU.IR,ram[CPU.IP]);
        int errorCode = parse(CPU.IR);
        // Do error checking and if error, return error
        if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                ram[CPU.IP][strlen(ram[CPU.IP])-2]='\0';
                displayCode(errorCode,ram[CPU.IP]);
            }
            return errorCode;
        }
        CPU.IP ++;
    }
    return 0;
}
