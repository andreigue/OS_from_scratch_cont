#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

#include "kernel.h"

int countTotalPages(FILE *f){
    // Assume FILE* f != NULL, error case managed before calling this method.
    char p[999];
    int lineCount = 0;

    while(fgets(p,999,f) != NULL){
        lineCount++;
    }

   int numberOfPage = (int)(lineCount/4.0);		//ceil wasn't working so remove it :    (int)ceil(lineCount/4.0);

   //Debug Code: printf("Number of lines in file: %d\n", (int)lineCount);
   //Debug Code: printf("Number of pages needed: %d\n", numberOfPage);
   //fseek( f, 0, SEEK_SET ); // put f back to beginning 
    fclose(f);

    return numberOfPage;
}


// helper to name temp files for copyFileToBackingStore
int nextFileIndex(){
        
   // i initialized at -1 because if the dir exists and successfully points to it, i+2 which makes it start at 1
    int i = -1;

    DIR *d;
    struct dirent *dir;
    d = opendir("./BackingStore");
    if(d){
        while((dir = readdir(d)) != NULL){
            i++;
        }
    }
    return i;
}

char *copyFileToBackingStore(FILE* p){

    char *filePath = malloc (sizeof (char) * 25);
    char cpyFileName[8];
    strcpy(filePath, "./BackingStore");

    int index = nextFileIndex();

    cpyFileName[0] = '/';
    cpyFileName[1] = 'P';
    cpyFileName[2] = index + 48;
    cpyFileName[3] = '.';
    cpyFileName[4] = 't';
    cpyFileName[5] = 'x';
    cpyFileName[6] = 't';
    cpyFileName[7] = '\0';

    strcat(filePath, cpyFileName);

    FILE* cpyPtr = fopen(filePath, "wt");
    if(cpyPtr == NULL){ // probably unreachable 
        printf("Cannot write %s in BackingStore\n", cpyFileName);
        exit(99); 
    } 

    char buffer[1000];
    while(fgets(buffer, 999, p) != NULL)
        if(buffer[0] != '\n')
            fputs(buffer, cpyPtr);

    fclose(p);
    fclose(cpyPtr);
    
    return filePath;
}








int launcher (FILE* p){
	int result = 0;

int defaultLoadingPages = 2; // defaults to loading two pages of program into RAM when launched

    FILE *bsFilePtr, *bsFilePtrCpy;
    
    char *bsFileNameFull = copyFileToBackingStore(p);
    bsFilePtr = fopen(bsFileNameFull, "rt");
    if(bsFilePtr == NULL){
        printf("Cannot open %s\n", bsFileNameFull);
        return 0;
    }

    int pageCount = countTotalPages(bsFilePtr);
	
	
    bsFilePtr = fopen(bsFileNameFull, "rt");
    if(bsFilePtr == NULL){
        printf("Cannot open %s\n", bsFileNameFull);
        return 0;
    }
    result = myinit(bsFilePtr, 0, 0, pageCount);

	



	
	return result;
}

