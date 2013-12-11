/*Shiying Zheng and Ben Stern hw10*/

#include "shell.h"
arraylist* stack;
pid_t root;
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
void child_handler(int sig){
	pid_t pid=getpid();
	kill(pid, SIGTERM);
}
void parent_handler(int sig){
	signal(SIGINT,SIG_IGN);
}
void sig_handler(int sig){
	if (sig==SIGINT) {
		pid_t mypid=getpid();
		pid_t curprocess=*(pid_t*)arraylist_get(stack,arraylist_size(stack)-1);
		if(mypid==curprocess && root!=curprocess) signal(SIGINT,child_handler);
		else signal(SIGINT,parent_handler);
	}//kill child
	signal(sig, sig_handler);
}
pid_t execprocess(int strcount,char** strings){
	pid_t pid=getpid();
	arraylist_addEnd(stack,&pid);
	if(execvp(*strings,strings)){
		pid_t* currentprocess=arraylist_removeEnd(stack);
		free(currentprocess);
		perror("");
		return(2);
	}
	pause();
	pid_t* currentprocess=arraylist_removeEnd(stack);
	free(currentprocess);
	return pid;
}
int main() {
	signal(SIGINT, sig_handler);
	stack=arraylist_init(sizeof(pid_t),5);
	pid_t pid=getpid();
	root=pid;
	arraylist_addEnd(stack,&pid);
	char* prompt="dinoshell: ";
	int done=0;
	pid_t child;
	int strcount;
	while(!done){
		printf("%s",prompt);
		char** strings=parse(&strcount);
		if(!strings || !strings[0]) printf("\n");
		else if(!strcmp(strings[0],"exit")) 
			done=1;
		else{
			pid_t child=fork();
			if ( -1 == child ) {
	    		perror("fork failed, I am sad");
	    		return(2);
			}
			if(child){
				int status;
				pid_t deadchild = wait(&status);
			}
			else {
				int ex=execprocess(strcount,strings);
				if (-1==ex) 
					fprintf(stderr, "Child process not successfully executed");
			}
		}
		if(strings)
			free_list_of_strings(strings);
	}
	arraylist_free(stack);
}
