#define DEFAULT_QUANTA 2

struct CPU { 
    int IP; 
    char IR[1000]; 
    int quanta; 
    int offset;
} CPU;

int run(int);

void initCPU();
void setCPU(FILE *PC, int offset);
int getCPUoffset();