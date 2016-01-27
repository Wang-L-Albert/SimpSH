
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define noarg 0
#define hasarg 1
#define noflag 0

#define RDONLY 'a'
#define WRONLY 'b'
#define COMMAND 'c'
#define APPEND 'd'
#define CLOEXEC 'e'
#define CREAT 'f'
#define DIRECTORY 'g'
#define DSYNC 'h'
#define EXCL 'i'
#define NOFOLLOW 'j'
#define NONBLOCK 'k'
#define RSYNC 'l'
#define SYNC 'm'
#define TRUNC 'n'
#define RDWR 'o'
#define PIPE 'p'
#define WAIT 'q'
#define CLOSE 'r'
#define ABORT 's'
#define CATCH 't'

struct command{
	int pid;
	char* name;
	int cmdPos;
	int cmdEnd;
	char cmdArgs[200];
};

void catch_sig(int sigNum){
	fprintf(stderr, "Signal %d was caught. \n", sigNum);
	exit(sigNum);
}


int numErrors = 0;
int main(int argc, char* argv[]){
	//set up dynamic arrays
	//there will never be more than argc child processes, so this is more than enough.
	struct command* cmdList =  malloc(argc*sizeof(struct command));
	int* fidList = (int*) malloc(argc*sizeof(int));
	int numCmd = 0;
	int numFid = 0;
	int fid = 0;
	int oflags = 0;
	
	static int verbose_flag = 0;
	struct option optionlist[] = {
		//*name, hasarg, *flag, val
		{"rdonly",  hasarg, noflag, RDONLY},
		{"wronly",  hasarg, noflag, WRONLY},
		{"verbose", noflag, &verbose_flag, 1},
		{"command", hasarg, noflag, COMMAND},
		{"append", noarg, noflag, APPEND},
		{"cloexec", noarg, noflag, CLOEXEC},
		{"creat", noarg, noflag, CREAT},
		{"directory", noarg, noflag, DIRECTORY},
		{"dsync", noarg, noflag, DSYNC},
		{"excl", noarg, noflag, EXCL},
		{"nofollow", noarg, noflag, NOFOLLOW},
		{"nonblock", noarg, noflag, NONBLOCK},
		{"rsync", noarg, noflag, RSYNC},
		{"sync", noarg, noflag, SYNC},
		{"trunc", noarg, noflag, TRUNC},
		{"rdwr", hasarg, noflag, RDWR},
		{"pipe", noarg, noflag, PIPE},
		{"wait", noarg, noflag, WAIT},
		{"close", hasarg, noflag, CLOSE},
		{"abort", noarg, noflag, ABORT},
		{"catch", hahsarg, noflag, CATCH},
		{0 , 0, 0, 0} 
		//last element indicated by 0's
	}; //********expand this to be variable later

	int optVal; //will store the val returned by getopt_long
	int option_ind = 0;
	while (1){ 
		optVal = getopt_long(argc, argv, "", optionlist, &option_ind); //option_ind is the option's index in argv[].
		//global var optarg now points to option_ind+1, and optind to the index of the next index after optarg (first non-option 
		//in argv[] if no more options).
		if (optVal == '?'){//occurs when we find an option without the appropriate argument
			fprintf(stderr, "%s is missing an argument\n", optionlist[option_ind].name);
			numErrors++;
			continue;
		}
		if (optVal == -1){//no more options to parse
			break;
		}
		//use optVal for switch statements, option_ind for access                 /////////////////////////////////
		switch (optVal) {
			case RDONLY://rdonly
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
				//we open the file as pointed to by optarg in read only mode
				fid = open(optarg, O_RDONLY);
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in read only mode failed. \n", optarg);
					numErrors++;
					continue;
				}
				fidList[numFid++] = fid;
				//increment fid?
				//printf("%s\n", ("Opened in read only."));
				//printf("optarg: %s\n", optarg);
				//printf("%d\n", (fidList[numFid-1]));
				break;
			case WRONLY://wronly
			{
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
				fid = open(optarg, O_WRONLY);
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in write only mode failed. \n", optarg);
					fprintf(stderr, "fuck");
					numErrors++;
					continue;
				}
				fidList[numFid++] = fid;
				//printf("%s\n",("Opened in write only."));
				//printf("optarg: %s\n", optarg);
				//printf("%d\n", (fidList[numFid-1]));
			}
				break;
			case RDWR:
				{
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					fid = open(optarg, O_RDWR);
					if (fid == -1){ //there was an error opening a file
						fprintf(stderr, "Opening file %s in read and write only mode failed. \n", optarg);
						numErrors++;
						continue;
					}
					fidList[numFid++] = fid;
				}
				
				break;
			case PIPE:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					int* pipeFid = (int*) malloc(3*sizeof(int));
					int createPipe = pipe(pipeFid);
					if (createPipe == -1){ //there was an error opening a pipe
						fprintf(stderr, "Creating a pipe failed. \n");
						numErrors++;
						continue;
					}
					fidList[numFid++] = pipeFid[0];
					fidList[numFid++] = pipeFid[1];
					free (pipeFid);
				}
				break;
			case WAIT:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					printf("Entering wait \n");
					for (int b = 0; b < numFid; b++){
						printf("closing down fid's \n");
						close (fidList[b]);
					}
					int exitStatus;
					for (int i = 0; i < numCmd; i++){
						printf("waiting on %s\n", cmdList[i].name);
						waitpid(cmdList[i].pid, &exitStatus, 0);
						printf("Finished waiting on %s\n", cmdList[i].name);
						int exitNorm = WIFEXITED(exitStatus);
						if (exitNorm){ //if exited normally
							printf("%s exited normally\n", cmdList[i].name);
							int errNum = WEXITSTATUS(exitStatus); //get error status
							printf("%d ", errNum); //print out error number
							//print out option name + arguments
							printf("%s\n", cmdList[i].cmdArgs);
						}
					}
					
				}
				break;
			case CLOSE:
				{
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					int error = close(fidList[atoi(optarg)]);
					if (error == -1){
						fprintf(stderr, "Closing file descriptor %d failed. \n", atoi(optarg));
						numErrors++;
						continue;
					}
				}
				break;
			case ABORT:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					int *a = 0;
					int b = *a;
				}
				break;
			case CATCH:
				{
					int signalNum = atoi(optarg);
					if (signalNum == NULL){
						fprintf(stderr, "No signal number passed into --catch. \n");
						numErrors++;
						continue;
					}
					signal(signalNum, catch_sig);
					break;
				}

			case APPEND:
				{
					if(verbose_flag) printf("--%s ", optionlist[option_ind].name);
					oflags|=O_APPEND;				
				}
				break;
			case CLOEXEC:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_CLOEXEC;
				}
				break;

			case CREAT:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_CREAT;
					break;
				}
			case DIRECTORY:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_DIRECTORY;
					break;
				}
			case DSYNC:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_DSYNC;
				}
				break;

			case EXCL:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_EXCL;
				}
				break;

			case NOFOLLOW:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_NOFOLLOW;
				}
				break;

			case NONBLOCK:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_NONBLOCK;
				}
				break;

			case RSYNC:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_RSYNC;
				}
				break;

			case SYNC:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_SYNC;
				}
				break;

			case TRUNC:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					oflags|=O_TRUNC;
				}
				break;
				
			case COMMAND:{//command
				int optEnd = optind;
				//need to check if any more options or not.
				//printf("optind inside command: %d\n", optind);
				const char dashes[3] = "--";
				for (int a = optind; a < argc; a++){
					char * nextOpt = strstr(argv[a], dashes);
					if(nextOpt != NULL){ //THERE IS ANOTHER OPTION
						optEnd = a;
						break;
					} else if (a == (argc - 1)){
						optEnd = argc;
					}
				}
				if(verbose_flag)
					{
						if(verbose_flag) printf("--%s %s ", optionlist[option_ind].name, optarg);
						for(int i = optind; i < optEnd; i++)
						{
							printf("%s ", argv[i]);
						}
						printf("\n");
				}
				//check for false fd's
				int fidCheck = optind-1;
				int falseFid = 0;
				for (int i = 0; i < 3; i++){
					//printf("inside for loop: argParse: %d\n", argParse);
					//printf("filedescrip: %s\n", argv[argParse]);
					int newIO = strtol(argv[fidCheck++], NULL, 10);
					if (newIO == -1){
						fprintf(stderr, "An invalid file descriptor was passed into command");
						falseFid = 1;
						numErrors++;
						break;
					}				
				}
				if (falseFid){
					break;
				}
				//check for valid fd's
				pid_t childPid = fork();
				if (childPid == 0){ //if it is a child
					//now three times from optind-1 to get the I/O re-routes
					int argParse = optind-1;
					for (int i = 0; i < 3; i++){
						//printf("inside for loop: argParse: %d\n", argParse);
						//printf("filedescrip: %s\n", argv[argParse]);
						int newIO = strtol(argv[argParse++], NULL, 10);
						//printf("newIO: %d\n", newIO );
						//printf("fidlist real newIO: %d\n", fidList[newIO] );
						dup2(fidList[newIO], i);						
					}
					//now that IO is rewritten, get the string
					//argParse should now be 4, the position of the char* for our command
					char* newCmd = argv[argParse++];
					//printf("passed in command: %s\n", newCmd);
					//now store the arguments for CMD in a new array.
					int argSize = optEnd-argParse+1;
					char** newArgs = NULL;
					newArgs = (char**) malloc((argSize+1)*sizeof(char *));/////may be used uninit in this func
					int newArgIndex;
					//printf("argSize: %d\n", argSize);
					newArgs[0]=newCmd;
					for (newArgIndex = 1; newArgIndex < argSize; newArgIndex++){
						newArgs[newArgIndex] = argv[argParse++];
						//printf("newArgs[newArgIndex] :        %s\n", newArgs[newArgIndex]);
					}
					//printf("newCmd:     %s\n", newCmd);
					//place nullptr at the end of the array.
					//printf("newArgIndex: %d\n", newArgIndex);
					newArgs[newArgIndex] = NULL;
					//carry out new execution
					for (int b = 0; b < numFid; b++){
						close (fidList[b]);
					}
					int failedExec = execvp(newCmd, newArgs);
					if (failedExec == -1){
						fprintf(stderr, "Executing command %s failed. \n", newCmd);
						numErrors++;
						exit(1);
					}
				} 
				cmdList[numCmd].pid = childPid;
				cmdList[numCmd].cmdPos = optind+2;
				cmdList[numCmd].cmdEnd = optEnd;
				for (int d = optind+2; d < optEnd; d++){
					strcat(cmdList[numCmd].cmdArgs, argv[d]);
					strcat(cmdList[numCmd].cmdArgs, " ");
				}
				cmdList[numCmd++].name = argv[optind+2];
			}	break;
			
		}
	}
	for (int i = 0; i < numFid; i++){
		close (fidList[i]);
	}	
	free(cmdList);
	free(fidList);
	exit(numErrors);
}
