/*Shiying Zheng and Ben Stern hw10*/

#include "shell.h"

char** parse(){
	char* line=malloc(sizeof(char)*(LINE_MAX+1));
	char* original_line=line;
	line=fgets(line, LINE_MAX+1, stdin);
	char** parsed=malloc(sizeof(char*)*(TOK_MAX+1));
	char* cur=malloc(sizeof(char)*(LINE_MAX+1));
	int counter=0;
	cur[0]=0;
	line[0]=0;
	parsed[0]=0;

	if (!(line&&parsed&&cur)) perror("Error when reading line");

	while (line){
		char c=line[0];
		if (c=='\n'){
			break;
		}
		else if ( c==' ' ){
			parsed[counter]=cur;
			counter++;
			cur[0]=0;
		}
		else if( c=='<' || c=='>' || c=='|' || c=='&'){
			parsed[counter]=cur;
			cur[0]=0;
			counter++;
			parsed[counter]=&c;
			counter++;
		}
		else {
			int i=strlen(cur);
			cur[i]=c;
			cur[i+1]=0;		
		}
		line++;
	}

	free(cur);
	printf("%s\n",line);
	free(original_line);

	return parsed;
}

int main() {
	char** string=parse();
	printf("%s",*string);
	free(string);
/*
    pid_t child;
    child = fork();
    int c;
	
	if ( -1 == child ) {
    	    perror("Fork failed");
       		return(1);
    }

	while ((c=getchar())!=EOF){

		if (child){//if I have a child then I'm the parent
			int status;
        	pid_t deadchild = wait(&status);
        	exit(0);
		}
		else{//I'm the child
			char *argv[3] = {"ls", ".", NULL};
			execvp("ls",argv);
		}
	}
	*/
}
