int launcher(FILE* p);
int findFrame();
int findVictim(PCB *p);
FILE *findPage(int pageNumber, FILE *f);
void loadPage(int pageNumber, FILE *f, int frameNumber);
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);