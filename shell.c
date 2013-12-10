/*Shiying Zheng and Ben Stern hw10*/

#include "shell.h"
void free_list_of_strings(char** list){
	for(int i=0;list[i];++i){
		free(list[i]);
	}
	free(list);
}
char** parse(int * argcountptr){
	char* line=malloc(sizeof(char)*(LINE_MAX+1));
	char* original_line=line;
	line[0]=0;
	if(!fgets(line, LINE_MAX+1, stdin)){
		free(line);
		return NULL;
	}
	char** parsed=malloc(sizeof(char*)*(TOK_MAX+1));
	char* cur=malloc(sizeof(char)*(LINE_MAX+1));
	int counter=0;
	int curcounter=0;
	cur[curcounter]=0;
	parsed[0]=0;
	if(!line){
		free(parsed);
		free(cur);
		free(line);
	}
	if (!(parsed&&cur)) perror("Ran out of memory");
	//printf("%s\n",line);

	while (*line){
		char c=line[0];
		if(c==EOF){
			*argcountptr=EOF;
			free_list_of_strings(parsed);
			free(original_line);
			return NULL;
		}
		if(c=='"'){
			for(line++;(c=line[0])!='"'&&c&&c!=EOF;line++){
				cur[curcounter]=c;
				curcounter++;
				cur[curcounter]=0;
				if(c=='\n'){
					line=original_line;
					fgets(line,LINE_MAX+1,stdin);
					line--;
				}
			}
		}
		else if (c=='\n'){
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
			curcounter=0;
			cur[0]=0;
		}
		else if( c=='<' || c=='>' || c=='|' || c=='&'){
			parsed[counter]=cur;
			cur=malloc(sizeof(char)*(LINE_MAX+1));
			if(!cur) perror("out of memory");
			curcounter=0;
			cur[0]=0;
			counter++;
			char* shortstring=malloc(sizeof(char)+1);
			shortstring[0]=c;
			shortstring[1]=0;
			parsed[counter]=shortstring;
			counter++;
		}
		else {
			cur[curcounter]=c;
			curcounter++;
			cur[curcounter]=0;		
		}
		line++;
	}
	parsed[counter]=NULL;
	//printf("%s\n",original_line);
	free(original_line);
	*argcountptr=counter;
	return parsed;
}
/*
int execute(char** strings, int strcount){
	char* argv[strcount];
	int argcounter=0;
	argv[0]=0;
	
	for (int i=0;i<strcount;i++){

		if (!strcmp(strings[i],">")){
			FILE* file=fopen(strings[i+1],"w");
			dup2(STDOUT_FILENO, file);
			fclose(file);
			i++;
		}
		else if (!strcmp(strings[i],"<")){
			FILE* file=fopen(strings[i+1],"r");
			dup2(file, STDIN_FILENO);
			fclose(file);
			i++;
		}
		else if (!strcmp(strings[i],"|")){

		}
		else if (!strcmp(string[i],"&")){
			
		}
		else{
			argv[argcounter]=strings[i];
			argcounter++;
			argv[argcounter]=0;
		}
	}
}
*/
arraylist* stack;
int ischild=0;
void sig_handler(int sig){
	if (sig==SIGINT) {
		kill(*(pid_t*)arraylist_get(stack,arraylist_size(stack)-1),SIGKILL);
	}//kill child
}
pid_t execprocess(int strcount,char** strings){
	pid_t child=fork();
	if ( -1 == child ) {
	    perror("fork failed, I am sad");
	    return(2);
	}
	if(child){
		signal(SIGINT, sig_handler);
		int status;
		pid_t deadchild = wait(&status);
	}
	else{
		pid_t pid=getpid();
		arraylist_addEnd(stack,&pid);
		//execute(strings,strcount);
		if(execvp(*strings,strings)){
			pid_t* currentprocess=arraylist_removeEnd(stack);
			free(currentprocess);
			//printf("Sorry I failed sucks for you\n")
			perror("");
			return(2);
		}
		pause();
		pid_t* currentprocess=arraylist_removeEnd(stack);
		free(currentprocess);
	}
	return child;
}
int main() {
	stack=arraylist_init(sizeof(pid_t),5);
	pid_t pid=getpid();
	arraylist_addEnd(stack,&pid);
	char* prompt="dinoshell: ";
	int done=0;
	pid_t child;
	int strcount;
	while(!done){
		printf("%s",prompt);
		char** strings=parse(&strcount);
		if(!strings) done=1;
		else if(!strings[0]);
		else if(!strcmp(strings[0],"exit")) done=1;
		else{
			child=execprocess(strcount,strings);
		}
		if(strings)
			free_list_of_strings(strings);
	}
	arraylist_free(stack);
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
