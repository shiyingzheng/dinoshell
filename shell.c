/*Shiying Zheng and Ben Stern hw10*/

#include "shell.h"

void free_list_of_strings(char** list){
	for(int i=0;list[i];++i){
		free(list[i]);
	}
	free(list);
}
char** parse(){
	char* line=malloc(sizeof(char)*(LINE_MAX+1));
	char* original_line=line;
	line[0]=0;
	line=fgets(line, LINE_MAX+1, stdin);
	char** parsed=malloc(sizeof(char*)*(TOK_MAX+1));
	char* cur=malloc(sizeof(char)*(LINE_MAX+1));
	int counter=0;
	cur[0]=0;
	parsed[0]=0;

	if (!(line&&parsed&&cur)) perror("Error when reading line");
	//printf("%s\n",line);
	while (*line){
		char c=line[0];
		if (c=='\n'){
			if(cur[0]){
				parsed[counter]=cur;
				counter++;
			}
			break;
		}
		else if ( c==' ' ){
			parsed[counter]=cur;
			//printf("%s\n",cur);
			cur=malloc(sizeof(char)*(LINE_MAX+1));
			if(!cur) perror("out of memory");
			counter++;
			cur[0]=0;
		}
		else if( c=='<' || c=='>' || c=='|' || c=='&'){
			parsed[counter]=cur;
			cur=malloc(sizeof(char)*(LINE_MAX+1));
			if(!cur) perror("out of memory");
			cur[0]=0;
			counter++;
			char* shortstring=malloc(sizeof(char)+1);
			shortstring[0]=c;
			shortstring[1]=0;
			parsed[counter]=shortstring;
			counter++;
		}
		else {
			int i=strlen(cur);
			cur[i]=c;
			cur[i+1]=0;		
		}
		line++;
	}
	parsed[counter]=NULL;
	//printf("%s\n",original_line);
	free(original_line);
	return parsed;
}

int main() {
	int done=0;
	while(!done){
		char** strings=parse();
		if(!strcmp(strings[0],"exit")) done=1;
		free_list_of_strings(strings);
	}
	/*for(int i=0;strings[i];i++){
		printf("%s\n",strings[i]);
	}*/
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
