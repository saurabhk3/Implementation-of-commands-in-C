#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>

void readFromStdin();


unsigned long int word = 0, ch = 0, line = 0, maxLenLine = 0; 
int main(int argc, char*argv[]){
    // 1. read from stdin
    if(argc == 1){
        readFromStdin();
    }
    printf("\t%u %u %u\n",line, word,ch);
}

void readFromStdin(){
    int c, flag = 0,nFlag=0;
    int currentLenLine = 0;
    while((c = fgetc(stdin)) != EOF){
        if(!isspace(c) && flag ==0){
            word++;
            flag = 1;
        }else if(isspace(c) || isblank(c)){
            flag = 0;
        }
        ch++;
        currentLenLine++;
        if(c=='\n'){
            nFlag = 1;  // if we encounter EOF without getting \n, then the last word has to be incremented
            line++;
            if(currentLenLine > maxLenLine){
                maxLenLine = currentLenLine;
            }
             currentLenLine = 0;  // set it 0 for next line's count
        }
    }
    if(nFlag == 0){
        maxLenLine = currentLenLine;
    }
    return;
}