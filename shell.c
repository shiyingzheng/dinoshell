/*Shiying Zheng and Ben Stern hw10*/

#include "shell.h"
pid_t child=-1;
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
	if (!(parsed&&cur)) perror("Ran out of memory");;

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
			cur=malloc(sizeof(char)*(LINE_MAX+1));
			if(!cur) perror("out of memory");
			counter++;
			curcounter=0;
			cur[0]=0;
		}
		else {
			cur[curcounter]=c;
			curcounter++;
			cur[curcounter]=0;		
		}
		line++;
	}
	parsed[counter]=NULL;
	free(original_line);
	*argcountptr=counter;
	return parsed;
}
void child_handler(int sig){
	printf("kill me");
	pid_t pid=getpid();
	kill(pid, SIGTERM);
}
void parent_handler(int sig){
	signal(SIGINT,SIG_IGN);
}
void sig_handler(int sig){
	if (sig==SIGINT) {
		if(child)
			signal(SIGINT,parent_handler);
		else
			signal(SIGINT,child_handler);
	}//kill child
	signal(sig, sig_handler);
}
char*** grouping(int strcount, char** strings, int* groupcount){
	char*** grouped=malloc(sizeof(char**)*(strcount+1));
	int group_num=0;
	int string_num=0;
	grouped[0]=malloc(sizeof(char*)*(strcount+1));
	grouped[0][0]=malloc(sizeof(char)*(LINE_MAX+1));
	if (!grouped[0] || !grouped[0][0]) perror("No memory for grouping");
	grouped[0][0][0]=0;
	for (int i=0;i<strcount;i++){
		char* cur=strings[i];
		if ( !strcmp(cur, "<") || !strcmp(cur, ">") || !strcmp(cur, "|") ){
			grouped[group_num][string_num]=0;
			group_num++;
			string_num=0;
			grouped[group_num]=malloc(sizeof(char*)*(strcount+1));
			grouped[group_num][string_num]=malloc(sizeof(char)*(LINE_MAX+1));
			if (!grouped[group_num] || !grouped[group_num][string_num]) perror("No memory for grouping");
			grouped[group_num][string_num][0]=0;
			strcpy(grouped[group_num][string_num],cur);
			grouped[group_num][string_num+1]=0;
			group_num++;
			grouped[group_num]=malloc(sizeof(char*)*(strcount+1));
			if (!grouped[group_num]) perror("No memory for grouping");
			grouped[group_num][string_num][0]=0;
		}
		else{
			grouped[group_num][string_num]=malloc(sizeof(char)*(LINE_MAX+1));
			if (!grouped[group_num][string_num]) perror("No memory for grouping");
			grouped[group_num][string_num][0]=0;
			strcpy(grouped[group_num][string_num],cur);
			string_num++;
		}
	}
	*groupcount=group_num+1;
	return grouped;
}
pid_t execprocess(int strcount,char** strings){
    if(execvp(*strings,strings)){
        perror("");
        return(2);
	}
	pause();
	return getpid();
	/*pid_t pid=getpid();
	arraylist_addEnd(stack,&pid);
	int groupcount;
	char*** grouped=grouping(strcount, strings, &groupcount);
	char** command=malloc(sizeof(char*)*strcount);
	char* file=malloc(sizeof(char)*(LINE_MAX+1));
	for (int i=0; i<groupcount; i++){
		if ( !strcmp(grouped[0], "<") ){
			//open the file using open() (please man it)
			//dup the thing on the right of < to stdin like dup2("meow", STDIN_FILENO)
			//and then execute the command
			//and clean the command
			//check this out http://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
		}
		else if ( !strcmp(grouped[0], ">")){
			//open the file
			//dup the thing on the right of > to stdout like dup2("meow", STDOUT_FILENO)
			//and then execute the command
			//and clean the command
		}
		else if ( !strcmp(grouped[0], "|") ){
			//From kuperman: should have STDIN_FILENO of bar be reading from the STDOUT_FILENO of bar. 
			//You can do this by using pipe(2) to create connected pairs of file descriptors and then use dup2(2) to set them up appropriately in the children. 
			//Close the unused ends 
			//(e.g., foo should close the descriptor that bar is using to read). Your shell will need to exec both processes and wait for both of them to return.
			//there's a very nice example here http://www6.uniovi.es/cscene/CS4/CS4-06.html
		}
		else{
			//put it onto the command buffer
		}
	}
	//if command is not empty at the end, execute it
	//also, every time when executing stuff, check if there's any error, i.e. if exec returns -1, then we should perror.
	pause();
	pid_t* currentprocess=arraylist_removeEnd(stack);
	free(currentprocess);
	return pid;*/
}
int main() {
	//Test the grouping method again before using it! Sorry I was too tired to do that. We also need a method to free memory from char***
	
	signal(SIGINT, sig_handler);
	char* prompt="dinoshell: ";
	int done=0;
	int strcount;
	while(!done){
		//printf("\n%d\n",getpid());
		printf("%s",prompt);
		char** strings=parse(&strcount);
		if(!strings||!strcmp(strings[0],"exit")) done=1;
		else if(!strings[0]) printf("\n");
		else{
			child=fork();
			if ( -1 == child ) {
	    		perror("fork failed, I am sad");
	    		return(2);

			}
			if(child){
				int status;
				//If the last thing on the command line is &, use waitpid(child, &status, WNOHANG) instead
				//if (!strcmp(strings[strcount-1], "&")){
				//	pid_t deadchild = waitpid(child, &status, WNOHANG);
				//}
				//else{
				while(child!=wait(&status)){
					;
				}
				//}
				child=0;
			}
			else {
				int ex=execprocess(strcount,strings);
				if (-1==ex) 
					perror("Child process not successfully executed");
				return(2);
			}
		}
		if(strings)
			free_list_of_strings(strings);
	}
}
