#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include<sys/wait.h>

#define MAXBUFF 512
static int *finalflag;
int pd[2];
int execute(char *tokens){
	//pid_t pid;
	//int status;
	char *entolh;
	char** kommatiaentolhs= malloc((MAXBUFF)*sizeof(char*));
	int point=0;

	entolh=strtok(tokens, " ");

	while(entolh != NULL){
		kommatiaentolhs[point]=entolh;
		entolh=strtok(NULL, " ");
		point++;
	}

	kommatiaentolhs[point]= NULL;

	if( kommatiaentolhs[0] != '\0'){
		if(strcmp(kommatiaentolhs[0],"quit") == 0) return 1;
	}

	if(execvp(kommatiaentolhs[0], kommatiaentolhs)<0){
		close(pd[0]);  /* Κλείσιμο άκρου ανάγνωσης */
		write(pd[1],"error",5);
		perror("Error in command");
		close(pd[1]);
		exit(-1);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	FILE *batchfile;
	int batchflag=0;
	pid_t *pid=(malloc)(sizeof(pid_t));
	int counter=0;
	int status;


		/* opening file for reading */
	if(argc == 2){
		batchflag=1;
		batchfile = fopen(argv[1], "r");
		if(batchfile == NULL)
		{
			 perror("Error opening file \n");
			 return(-1);
		}
	}

	for(;;){
		char *string= malloc((MAXBUFF)*sizeof(char));
		char *token;
		char **tokens= malloc((MAXBUFF)*sizeof(char*));
		int quitflag=0; // flag gia na doume pote mas dinei entolh eksodou
		finalflag= mmap(NULL, sizeof *finalflag, PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		*finalflag=0;
		char buf[6];

		if(batchflag ==1){
			if(fgets(string, MAXBUFF+1, batchfile) == NULL) break; // diavazw grammh grammh to arxeio

			if(string[strlen(string)-1] == '\n'){
				string[strlen(string)-1]= '\0';
			}
					/* efedrikos tropos */
			/*for(int z=0; z<512; z++){
				if(string[z]=='\n'){
					string[z]='\0';
					break;
				}
			} */
		}
		else {
 			printf("antoniadis_8279>");

 			if(fgets(string,MAXBUFF+1,stdin) == NULL){
                		perror("Error reading line");
                		exit(-1);
 			}
			if(string[strlen(string)-1] == '\n'){
				string[strlen(string)-1]= '\0';
			}

 			/*if((strlen(string)) > MAXBUFF+1){ // gia na elegxo an einai h grammh megaluterh apo to 512
 				perror("Error in command's length");
 				return -1;
 			} */
 		}

		int point=0;

		token=strtok(string,";");

		while(token != NULL){

			tokens[point]= token;
			point++;
			token=strtok(NULL, ";");
		}

		tokens[point] = NULL; // prepei na exei NULL sthn teleutaia tou 8esh

   	if( pipe(pd) < 0 ){
			 perror("Can’t open pipe");
			 exit(-1);
		 }
		for(int i=0; i<point; i++){

			if((pid[i]=fork())<0){
				perror("Error in fork");
				exit(-1);
			}
			if(pid[i]==0){
				quitflag=execute(tokens[i]);
				if(quitflag==1) *finalflag=1;
				exit(1);
			}
			if(i==counter){
				counter++;
				pid=realloc(pid,counter);
			}
		}
		int n;

		close(pd[1]);  /* Κλείσιμο άκρου εγγραφής */

		for(int j=0; j<point; j++){
			n=read(pd[0],buf,5);

		 do
		  {
			   waitpid(pid[j] ,&status, WUNTRACED) ; //o pateras perimenei ola ta paidia
		  }while (!WIFEXITED(status) && !WIFSIGNALED(status));

		}
		if(strcmp(buf,"error")==0) exit(0);
		
		close(pd[0]);
 		free(string);

 		if(*finalflag==1){
			printf("\n Quit command found \n");
			exit(0); //ama exw brei quit bgainw
 		}
		munmap(finalflag, sizeof *finalflag);
	}
	if(batchflag==1) fclose(batchfile);
	return 0;
}
