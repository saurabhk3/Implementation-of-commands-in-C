#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>

// TODO include --files option
// TODO 2 : in w '*' , list the files in lexicographically
void readFromStdin();
void  printOptionStdin(char*,char[]);
void readFile(char*);
void operateDir(char*);
void printTotal(char*);

unsigned long int word = 0, ch = 0, line = 0, maxLenLine = 0; 
unsigned long int total_word = 0, total_ch = 0, total_line = 0;  // incase of multiple files :)
int fileNameFlag = 0; // this tells whether file name is set to be printed or not.
unsigned long int totalMaximumLineLen = 0; 
int main(int argc, char*argv[]){
    char *options;
    if(argc == 1){  // read from stdin with no option provided
        readFromStdin();
        printf("\t%u\t%u\t%u\n",line, word,ch);
    }else if(argc==2){ // possibilities: 1. stdin with -,2. stdin with -[option] 3. '*' :read all files in dir, 4.read from a file
        
		////////////////////////////////////////////////////WORK ON --files0-from=//////////////////////////////////
        options = argv[1];
		if(strstr(options,"--files0-from=")){  
			printOptionStdin(options,"");
			exit(1);
		}
        if(options[0] == '-' && options[1] == '\0'){
            // 1: stdin with -
            readFromStdin();
            printf("\t%u\t%u\t%u -\n",line, word,ch);
        }else if(options[0] == '-' && strstr("l",options) || strstr("c",options) || strstr("w",options)
            ||(strstr("m",options) || strstr("L",options)) ){
            // 2: stdin with -[option]
            readFromStdin();
            printOptionStdin(options,"");
        }else if((strstr("*",options))){
            // 3 . *
            // input=> ./wc '*' so that it can get it, my OS interprets wild card char in this way
            operateDir("");
			// printf(" %u\t%u\t%u\ttotal\n",total_line,total_word,total_ch);
        }else{
            // 4. file
            //TODO : first check that it's not a directory.
			readFile(options);
			fileNameFlag = 1;
            printf(" %u\t%u\t%u\t%s\n",line,word,ch,options);
        }
    }else if(argc == 3){  // 1. wc - -opt , 2. wc abc.txt -opt 3. wc abc.txt cde.txt 4. wc * -opt
        if(argv[1][0] == '-'){ // 1
            fileNameFlag = 1;
            readFromStdin();
            printOptionStdin(argv[2],"-");
        }else if(((strstr(argv[1],"-") == NULL) && (strstr(argv[2],"-")) != NULL) && (strstr(argv[1],"*") == NULL)){  // 2
			readFile(argv[1]);
			printOptionStdin(argv[2],argv[1]);
			fileNameFlag = 1;
		}else if(((strstr(argv[1],"-") == NULL) && (strstr(argv[2],"-")) == NULL)&& (strstr(argv[1],"*") == NULL)){  // 3
			readFile(argv[1]);
			total_ch += ch;
			total_word += word;
			total_line += line;
			printf(" %u %u %u %s\n",line,word,ch,argv[1]);

			ch = word = line = 0;
			readFile(argv[2]);
			total_ch += ch;
			total_word += word;
			total_line += line;
			printf(" %u %u %u %s\n",line,word,ch,argv[2]);
			printf(" %u %u %u total\n",total_line,total_word,total_ch);
		}
		else if((strstr("*",argv[1])) != NULL){ // 4
			operateDir(argv[2]);
		}
    }else{ // wc a.txt b.txt c.txt ... -[opt]
            char mode[30];
            int j = 1, f = 0;
            mode[0] = '-';
            while(j<argc){
                if(strstr(argv[j],"-") != NULL){
                    f = 1;
                    char * tok = strtok(argv[j],"-");
                    strcat(mode,tok);
                }
                j++;
            }
            if(f == 0){
                strcpy(mode,"");
            }
            int i = 1;
            while(i<argc){
                if(strstr(argv[i],"-") == NULL){
                    // it's a file
                    readFile(argv[i]);
                    total_ch += ch;
                    total_line += line;
                    total_word += word;
                    if(maxLenLine > totalMaximumLineLen){
                        totalMaximumLineLen = maxLenLine;
                    }
                    printOptionStdin(mode,argv[i]);
                    ch = 0;
                    line = 0;
                    word = 0;
                    maxLenLine = 0;
                    
                }
                i++;
            }
            printTotal(mode);
            
            
            
    }
    
}
void operateDir(char* mode){
    int status;
    struct stat st_buf;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(d){
        while((dir = readdir(d)) != NULL){
            status = stat(dir->d_name,&st_buf);
            if(status !=0){
                printf("Error %d\n",errno);
                exit(1);
            }
            // now let's check whether the current file is a regular file or a dircetory
            if(S_ISREG(st_buf.st_mode)){
                // regualr file
               readFile(dir->d_name);
            }if(S_ISDIR(st_buf.st_mode)){
                // directory
                if(dir->d_name[0] == '.'){
                    continue;  // we need not to consider them, wc also ignores it.
                }
                printf("wc: %s :Is a directory\n",dir->d_name);
            }
			printOptionStdin(mode,dir->d_name);
            total_ch += ch;
            total_line += line;
            total_word += word;
			if(totalMaximumLineLen < maxLenLine){
				totalMaximumLineLen = maxLenLine;
			}
            line = 0;
            word = 0;
            ch = 0;
			maxLenLine = 0;
        }
		printTotal(mode);
        closedir(d);
    }else{
        printf("error opening dir\n");
    }
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
            currentLenLine--;  // \n is not counted 
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
void readFile(char * s){
	FILE * fp = fopen(s,"r");
	if(fp == NULL){
		printf("Error opening  file %s: No such file or directory\n",s);
		exit(1);
	}
    int c, flag = 0,nFlag=0;
    int currentLenLine = 0;
    while((c = fgetc(fp)) != EOF){
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
			currentLenLine--;
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
void printOptionStdin(char*s,char *file){ // TODO add combiantions of lwmLc
	if(strcmp(s,"")==0){
		printf(" %u\t%u\t%u\t%s\n",line,word,ch,file);
		return;
	}
    if(s[0]=='-'){
        if(strcmp(s,"-c")==0 || strcmp(s,"-m")==0){
            printf(" %u",ch);
        }else if(strcmp(s,"-l")==0){
            printf(" %u",line);
        }else if(strcmp(s,"-L")==0){
            printf(" %u",maxLenLine);
        }else if(strcmp(s,"-w")==0){
            printf(" %u",word);
        }else if(strcmp("-cwlL",s)==0 || strcmp("-cwLl",s)==0 || strcmp("-clwL",s)==0 || strcmp("-clLw",s)==0 || 
            strcmp("-cLwl",s)==0 || strcmp("-cLlw",s)==0 || strcmp("-Lcwl",s)==0 || strcmp("-Lclw",s)==0 ||
            strcmp("-Llcw",s)==0 || strcmp("-Llwc",s)==0 || strcmp("-Lwcl",s)==0 || strcmp("-Lwlc",s)==0 ||
            strcmp("-lcwL",s)==0 || strcmp("-lcLw",s)==0 || strcmp("-lLcw",s)==0 || strcmp("-lLwc",s)==0 ||
            strcmp("-lwcL",s)==0 || strcmp("-lwLc",s)==0 || strcmp("-wlLc",s)==0 || strcmp("-wlcL",s)==0 ||
            strcmp("-wLcl",s)==0 || strcmp("-wLlc",s)==0 || strcmp("-wcLl",s)==0 || strcmp("-wclL",s)==0 ||
            strcmp("-cmlL",s)==0 || strcmp("-cmLl",s)==0 || strcmp("-clmL",s)==0 || strcmp("-clLm",s)==0 || 
            strcmp("-cLml",s)==0 || strcmp("-cLlm",s)==0 || strcmp("-Lcml",s)==0 || strcmp("-Lclm",s)==0 ||
            strcmp("-Llcm",s)==0 || strcmp("-Llmc",s)==0 || strcmp("-Lmcl",s)==0 || strcmp("-Lmlc",s)==0 ||
            strcmp("-lcmL",s)==0 || strcmp("-lcLm",s)==0 || strcmp("-lLcm",s)==0 || strcmp("-lLmc",s)==0 ||
            strcmp("-lmcL",s)==0 || strcmp("-lmLc",s)==0 || strcmp("-mlLc",s)==0 || strcmp("-mlcL",s)==0 ||
            strcmp("-mLcl",s)==0 || strcmp("-mLlc",s)==0 || strcmp("-mcLl",s)==0 || strcmp("-mclL",s)==0){

            printf(" %u %u %u %u",line,word,ch,maxLenLine);
        }else if(strcmp("-clw",s)==0 || strcmp("-cwl",s)==0 || strcmp("-lcw",s)==0 || strcmp("-lwc",s)==0 || 
            strcmp("-wcl",s)==0 || strcmp("-wlc",s)==0){

            printf(" %u %u %u",line,word,ch);
        }else if(strcmp("-cL",s)==0 || strcmp("-Lc",s)==0){
            printf(" %u %u",ch,maxLenLine);
        }else if(strcmp("-cl",s)==0 || strcmp("-lc",s)==0){
            printf(" %u %u",line,ch);
        }else if(strcmp("-cw",s)==0 || strcmp("-wc",s)==0){
            printf(" %u %u",word,ch);
        }else if(strcmp("-cm",s)==0 || strcmp("-mc",s)==0){
            printf(" %u %u",ch,ch);
        }else if(strcmp("-cc",s)==0){
            printf(" %u %u",ch);
        }else if(strcmp("-ml",s)==0 || strcmp("-lm",s)==0){
            printf(" %u %u",line,ch);
        }else if(strcmp("-mL",s)==0 || strcmp("-Lm",s)==0){
            printf(" %u %u",ch,maxLenLine);
        }else if(strcmp("-mw",s)==0 || strcmp("-wm",s)==0){
            printf(" %u %u",word,ch);
        }else if(strcmp("-lL",s)==0 || strcmp("-Ll",s)==0){
            printf(" %u %u",line,maxLenLine);
        }else if(strcmp("-lw",s)==0 || strcmp("-wl",s)==0){
            printf(" %u %u",line,word);
        }else if(strcmp("-Lw",s)==0 || strcmp("-wL",s)==0){
            printf(" %u %u",word,maxLenLine);
        }
        else if(s[1]=='-'){
            if(strcmp("--chars",s)==0 ||strcmp("--bytes",s)==0 ){
                printf(" %u",ch);
            }else if(strcmp("--lines",s)==0){
                printf(" %u",line);
            }else if(strcmp("--max-line-length",s)==0){
                printf(" %u",maxLenLine);
            }else if(strcmp("--words",s)==0){
                printf(" %u",word);
            }else if(strcmp(s,"--files0-from=")>0){  // s should also contain the filename, so it's size should be greater
				FILE *fp;
				char * source;
				source = strtok(s,"--files0-from=");
				if(source == NULL){
					printf("Unrecognized option '%s'",s);
					exit(1);
				}
				if((fp=fopen(source,"r")) != NULL){
					// read each file and perform operations.
					char names[100];
					char c;
					int i =0;
					while((c = fgetc(fp)) != EOF){
						if(c != '\0'){
							names[i] = c;
							i++;
						}else if(c == '\0'){
							names[i] = c;
							readFile(names);
							printOptionStdin("",names);
							strcpy(names,0);
							i = 0;
						}
					}
				}
			}else{
                printf("Invalid input\n");
				exit(1);
            }
        }
    }else{
        printf("Invalid input\n");
		exit(1);
    }
    if(strlen(file)>0){
		printf(" %s",file);
	}
    printf("\n");
}
void printTotal(char *s){
	if(strcmp("",s)==0){
			printf(" %u\t%u\t%u\ttotal\n",total_line,total_word,total_ch);
			return;
	}
	if(s[0]=='-'){
        if(strcmp(s,"-c")==0 || strcmp(s,"-m")==0){
            printf(" %u",total_ch);
        }else if(strcmp(s,"-l")==0){
            printf(" %u",total_line);
        }else if(strcmp(s,"-L")==0){
            printf(" %u",totalMaximumLineLen);
        }else if(strcmp(s,"-w")==0){
            printf(" %u",total_word);
        }else if(strcmp("-cwlL",s)==0 || strcmp("-cwLl",s)==0 || strcmp("-clwL",s)==0 || strcmp("-clLw",s)==0 || 
            strcmp("-cLwl",s)==0 || strcmp("-cLlw",s)==0 || strcmp("-Lcwl",s)==0 || strcmp("-Lclw",s)==0 ||
            strcmp("-Llcw",s)==0 || strcmp("-Llwc",s)==0 || strcmp("-Lwcl",s)==0 || strcmp("-Lwlc",s)==0 ||
            strcmp("-lcwL",s)==0 || strcmp("-lcLw",s)==0 || strcmp("-lLcw",s)==0 || strcmp("-lLwc",s)==0 ||
            strcmp("-lwcL",s)==0 || strcmp("-lwLc",s)==0 || strcmp("-wlLc",s)==0 || strcmp("-wlcL",s)==0 ||
            strcmp("-wLcl",s)==0 || strcmp("-wLlc",s)==0 || strcmp("-wcLl",s)==0 || strcmp("-wclL",s)==0 ||
            strcmp("-cmlL",s)==0 || strcmp("-cmLl",s)==0 || strcmp("-clmL",s)==0 || strcmp("-clLm",s)==0 || 
            strcmp("-cLml",s)==0 || strcmp("-cLlm",s)==0 || strcmp("-Lcml",s)==0 || strcmp("-Lclm",s)==0 ||
            strcmp("-Llcm",s)==0 || strcmp("-Llmc",s)==0 || strcmp("-Lmcl",s)==0 || strcmp("-Lmlc",s)==0 ||
            strcmp("-lcmL",s)==0 || strcmp("-lcLm",s)==0 || strcmp("-lLcm",s)==0 || strcmp("-lLmc",s)==0 ||
            strcmp("-lmcL",s)==0 || strcmp("-lmLc",s)==0 || strcmp("-mlLc",s)==0 || strcmp("-mlcL",s)==0 ||
            strcmp("-mLcl",s)==0 || strcmp("-mLlc",s)==0 || strcmp("-mcLl",s)==0 || strcmp("-mclL",s)==0){

            printf(" %u %u %u %u",total_line,total_word,total_ch,totalMaximumLineLen);
        }else if(strcmp("-clw",s)==0 || strcmp("-cwl",s)==0 || strcmp("-lcw",s)==0 || strcmp("-lwc",s)==0 || 
            strcmp("-wcl",s)==0 || strcmp("-wlc",s)==0){

            printf(" %u %u %u",total_line,total_word,total_ch);
        }else if(strcmp("-cL",s)==0 || strcmp("-Lc",s)==0){
            printf(" %u %u",total_ch,totalMaximumLineLen);
        }else if(strcmp("-cl",s)==0 || strcmp("-lc",s)==0){
            printf(" %u %u",total_line,total_ch);
        }else if(strcmp("-cw",s)==0 || strcmp("-wc",s)==0){
            printf(" %u %u",total_word,total_ch);
        }else if(strcmp("-cm",s)==0 || strcmp("-mc",s)==0){
            printf(" %u %u",total_ch,total_ch);
        }else if(strcmp("-cc",s)==0){
            printf(" %u %u",total_ch);
        }else if(strcmp("-ml",s)==0 || strcmp("-lm",s)==0){
            printf(" %u %u",total_line,total_ch);
        }else if(strcmp("-mL",s)==0 || strcmp("-Lm",s)==0){
            printf(" %u %u",total_ch,totalMaximumLineLen);
        }else if(strcmp("-mw",s)==0 || strcmp("-wm",s)==0){
            printf(" %u %u",total_word,total_ch);
        }else if(strcmp("-lL",s)==0 || strcmp("-Ll",s)==0){
            printf(" %u %u",total_line,totalMaximumLineLen);
        }else if(strcmp("-lw",s)==0 || strcmp("-wl",s)==0){
            printf(" %u %u",total_line,total_word);
        }else if(strcmp("-Lw",s)==0 || strcmp("-wL",s)==0){
            printf(" %u %u",total_word,totalMaximumLineLen);
        }
        else if(s[1]=='-'){
            if(strcmp("--total_chars",s)==0 ||strcmp("--bytes",s)==0 ){
                printf(" %u",total_ch);
            }else if(strcmp("--total_lines",s)==0){
                printf(" %u",total_line);
            }else if(strcmp("--max-total_line-length",s)==0){
                printf(" %u",totalMaximumLineLen);
            }else if(strcmp("--total_words",s)==0){
                printf(" %u",total_word);
            }else{
                printf("Invalid input\n");
				exit(1);
            }
        }
    }else{
        printf("Invalid input\n");
		exit(1);
    }
    printf(" total\n");
}