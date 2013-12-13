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
	char* line=malloc(sizeof(char)*(LINE_MAX+1)); //the line buffer
	char* original_line=line; //a pointer to the start of the line buffer so that we can free the buffer later
	line[0]=0; 
	if(!fgets(line, LINE_MAX+1, stdin)){ //the line has EOF, we try to exit the program
		free(line);
		return NULL;
	}
	char** parsed=malloc(sizeof(char*)*(TOK_MAX+1)); //an array of strings to put in the words/symbols from the line
	char* cur=malloc(sizeof(char)*(LINE_MAX+1)); //a buffer for the current word
	int counter=0; //a counter for the location we are pointing to in parsed
	int curcounter=0; //a counter for the location we are pointing to in cur
	cur[curcounter]=0; 
	parsed[0]=0;
	if (!(parsed&&cur)) perror("Ran out of memory");;
	if(!line){ //if nothing is in the line, we just free up everything
		free(parsed);
		free(cur);
		free(line);
	}

	while (*line){ //when there is still another character in the line buffer
		char c=line[0]; //get the first character of line
		if(c==EOF){ //just to make sure EOF is handled
			*argcountptr=EOF;
			free_list_of_strings(parsed);
			free(original_line);
			return NULL;
		}
		if(c=='"' || c=='\'' ){ //when things are in quotes, we want them grouped together into one character array
			char ch=c; 
			for(line++;(c=line[0])!=ch&&c&&c!=EOF;line++){
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
		else if( c=='<' || c=='>' || c=='|' || c=='&'){ //we can do things like "cat <a.txt" or "cat meow.txt|grep ruff"! 
        	parsed[counter]=cur; //put the current word into line buffer
            cur=malloc(sizeof(char)*(LINE_MAX+1)); //make a new word!
            if(!cur) perror("out of memory");
            curcounter=0; //counter for the current word set back to 0
            cur[0]=0; 
            counter++; //counter for the location in parsed
            char* shortstring=malloc(sizeof(char)+1);
            if(!shortstring) perror("out of memory");
            shortstring[0]=c;
            shortstring[1]=0;
            parsed[counter]=shortstring;
            counter++;
        }
		else { //all other normal boring characters aren't left out
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
void print_groups(char*** groups){
	for(int i=0;groups[i];i++){
		printf("[");
		for(int j=0;groups[i][j];j++){
			printf("\"%s\",",groups[i][j]);
		}
		printf("]");
	}
	printf("\n");
}
void print_strings(char** strings){
	for(int i=0;strings[i];i++){
		printf("\"%s\",",strings[i]);
	}
	printf("\n");
}
char*** grouping(int strcount, char** strings, int* groupcount){
	char*** grouped=malloc(sizeof(char**)*(strcount+1));
	//print_strings(strings);
	int group_num=0;
	int string_num=0;
	grouped[group_num]=malloc(sizeof(char*)*(strcount+1));   
	grouped[group_num][string_num]=malloc(sizeof(char)*(LINE_MAX+1)); 
	if (!grouped[group_num] || !grouped[group_num][string_num]) fprintf(stderr,"No memory for grouping");
	for (int i=0;strings[i];i++){
		char* cur=strings[i];
		if(!cur[0]) continue;
		if ( !strcmp(cur, "<") || !strcmp(cur, ">") || !strcmp(cur, "|") ){
			grouped[group_num][string_num]=0;//making the last string in the group null
			group_num++;//incrementing the group
			string_num=0;
			grouped[group_num]=malloc(sizeof(char*)*(strcount+1));
			grouped[group_num][string_num]=malloc(sizeof(char)*(LINE_MAX+1)); //creating the first string in the group
			if (!grouped[group_num] || !grouped[group_num][string_num]) fprintf(stderr,"No memory for grouping");
			grouped[group_num][string_num][0]=0;//making the first string the the group an empty string
			strcpy(grouped[group_num][string_num],cur);//copying the < > | into the string
			grouped[group_num][string_num+1]=0;//making the second string in the group null
			group_num++;//making a new group
			grouped[group_num]=0;//making it null;
		}
		else{
			if (!grouped[group_num]){ 
				grouped[group_num]=malloc(sizeof(char*)*(strcount+1)); 
			}
			grouped[group_num][string_num]=malloc(sizeof(char)*(LINE_MAX+1)); 
			if (!grouped[group_num][string_num]) fprintf(stderr,"No memory for grouping");
			strcpy(grouped[group_num][string_num],cur);//copying the string from strings into grouped
			string_num++;
		}
	}
	grouped[group_num][string_num]=0;
	group_num++;//making a new group which should be null
	grouped[group_num]=0;
	*groupcount=group_num;
	return grouped;
}
pid_t multipipe(int groupcount, char*** grouped){
		char** command;
		int i=0;
		pid_t child2;
		int* p=malloc((groupcount-1)*sizeof(int));
		for(int j=0;j<(groupcount-1)/2;++j){
			if(pipe(p+2*j)<0){
				perror("");
				return(2);
			}
		}
		while((command=grouped[2*i])!=NULL){
			if((child2=fork())==-1){
				perror("");
				return(2);
			}
			if(child2){
				int status;
				while(child2!=wait(&status)){
					;
				}
				i++;
			}
			else{
				if(i!=0){
					if(dup2(p[2*i-2],STDIN_FILENO)<0){
						perror("");
						return(2);
					}
				}
				else if(i<(groupcount-1)/2){
					if(dup2(p[2*i+1],STDOUT_FILENO)<0){
						perror("");
						return(2);
					}
				}
				//print_strings(command);
				execvp(*command,command);
				perror("");
			}
		}
		free(p);
		return getpid();
		//From kuperman: should have STDIN_FILENO of bar be reading from the STDOUT_FILENO of bar. 
		//You can do this by using pipe(2) to create connected pairs of file descriptors and then use dup2(2) to set them up appropriately in the children. 
		//Close the unused ends 
		//(e.g., foo should close the descriptor that bar is using to read). Your shell will need to exec both processes and wait for both of them to return.
		//there's a very nice example here http://www6.uniovi.es/cscene/CS4/CS4-06.htmld
//if command is not empty at the end, execute it
//also, every time when executing stuff, check if there's any error, i.e. if exec returns -1, then we should perror.
}
pid_t execprocess(int strcount,char** strings){
    /*if(execvp(*strings,strings)){
        perror("");
        return(2);
	}
	pause();
	return getpid();*/
	pid_t pid=getpid();
	int groupcount;
	char*** grouped=grouping(strcount, strings, &groupcount);
	//print_groups(grouped);
	char** command;//=malloc(sizeof(char*)*strcount);
	char* file;
	if(groupcount<2){
		//printf("**grouped is %s",**grouped);
		if(execvp(**grouped,*grouped)){
        	perror("");
        	return(2);
		}
		pause();
		return getpid();
	}
	else{
		//printf("%d ", groupcount);
		for (int i=0; i<groupcount; i++){
			if ( !strcmp(grouped[i][0], "<") ){
				if(i<1){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				command=grouped[i-1];
				if(!*command){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				if(!grouped[i+1]){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				int j;
				for(j=0;grouped[i+1][j];j++){
					;
				}
				file=grouped[i+1][j-1];
				//printf("command %s file %s\n",*command,file);
				int f=fileno(fopen(file,"r"));
				//printf("%d\n",f);
				//printf("I'm here hi");
				dup2(f,STDIN_FILENO);
				close(f);
				if(execvp(*command,command)){
        			perror("");
        			return(2);
				}
				pause();
				return getpid();
			//open the file using open() (please man it)
			//dup the thing on the right of < to stdin like dup2("meow", STDIN_FILENO)
			//and then execute the command
			//and clean the command
			//check this out http://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
			}
			else if ( !strcmp(grouped[i][0], ">")){
				if(i<1){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				command=grouped[i-1];
				if(!*command){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				if(!grouped[i+1]){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				int j;
				for(j=0;grouped[i+1][j];j++){
					;
				}
				file=grouped[i+1][j-1];
				//printf("command %s file %s\n",*command,file);
				int f=fileno(fopen(file,"w"));
				//printf("%d\n",f);
				//printf("I'm here hi");
				dup2(f,STDOUT_FILENO);
				close(f);
				if(execvp(*command,command)){
        			perror("");
        			return(2);
				}
				pause();
				return getpid();
			//open the file
			//dup the thing on the right of > to stdout like dup2("meow", STDOUT_FILENO)
			//and then execute the command
			//and clean the command
			}
			else if ( !strcmp(grouped[i][0], "|") ){
				if(i<1){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				char** command2;
				command=grouped[i-1];
				//print_groups(grouped);
				//printf("%d\n",i);
				if(!*command){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				if(!grouped[i+1]){
					fprintf(stderr, "%s\n", "No such file or directory");
					return -1;
				}
				int s=multipipe(groupcount-i,grouped+i-1);
				if(s) perror("");
				return s;
			//From kuperman: should have STDIN_FILENO of bar be reading from the STDOUT_FILENO of bar. 
			//You can do this by using pipe(2) to create connected pairs of file descriptors and then use dup2(2) to set them up appropriately in the children. 
			//Close the unused ends 
			//(e.g., foo should close the descriptor that bar is using to read). Your shell will need to exec both processes and wait for both of them to return.
			//there's a very nice example here http://www6.uniovi.es/cscene/CS4/CS4-06.html
			}
		}
	}
	//if command is not empty at the end, execute it
	//also, every time when executing stuff, check if there's any error, i.e. if exec returns -1, then we should perror.
	pause();
	return pid;
}
int main() {
	//Test the grouping method again before using it! Sorry I was too tired to do that. We also need a method to free memory from char***
	signal(SIGINT,SIG_IGN);
	char* prompt="dinoshell: ";
	int done=0;
	int strcount;
	while(!done){
		//printf("\n%d\n",getpid());
		printf("%s",prompt);
		char** strings=parse(&strcount);
		if(!strings) done=1;
		else if(!strings[0]) ;
		else if (!strcmp(strings[0],"exit")) done=1;
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
				child=-1;
			}
			else {
				signal(SIGINT,SIG_DFL);
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
