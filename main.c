#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
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

struct Command {
	int pid;
	char name[100];
	int cmdPos;
	int cmdEnd;
};

int numErrors = 0;
int main(int argc, char* argv[]){
	//set up dynamic arrays
	//there will never be more than argc child processes, so this is more than enough.
	struct Commands* cmdList = (pid_t*) malloc(argc*sizeof(Command));
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
		if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg); ///////////////////////////////optarg might need to check if it's another option
		//use optVal for switch statements, option_ind for access                 /////////////////////////////////
		switch (optVal) {
			case RDONLY://rdonly
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
				fid = open(optarg, O_WRONLY);
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in write only mode failed. \n", optarg);
					numErrors++;
					continue;
				}
				fidList[numFid++] = fid;
				//printf("%s\n",("Opened in write only."));
				//printf("optarg: %s\n", optarg);
				//printf("%d\n", (fidList[numFid-1]));
				break;
			case RDWR:
				fid = open(optarg, O_RDWR);
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in read and write only mode failed. \n", optarg);
					numErrors++;
					continue;
				}
				fidList[numFid++] = fid;
				break;
			case PIPE:
				int pipeFid[2];
				int createPipe = pipe(pipeFid);
				if (createPipe == -1){ //there was an error opening a pipe
					fprintf(stderr, "Creating a pipe failed. \n");
					numErrors++;
					continue;
				}
				fidList[numFid++] = pipeFid[0];
				fidList[numFid++] = pipeFid[1];
				break;
			case WAIT:
				int exitStatus;
				for (int i = 0; i < numCmd; i++){
					waitpid(cmdList[i].pid, &exitStatus, 0);
					bool exitNorm = WIFEXITED(status);
					if (exitNorm){ //if exited normally
						int errNum = WEXITSTATUS(exitStatus); //get error status
						printf("%d ", errNum); //print out error number
						//print out option name + arguments
						for (int a = cmdList[i].cmdPos; a < cmdList[i].cmdEnd; a++) { //starting from the command name, loop till the next option and print
							printf("%s ", argv[a]);
						}
						printf("\n");
					}
				}
				for (int b = 0; b < numFid; b++){
					close (fidList[b]);
				}
				break;

			case APPEND:
				break;

			case CLOEXEC:
				break;

			case CREAT:
				break;

			case DIRECTORY:
				break;

			case DSYNC:
				break;

			case EXCL:
				break;

			case NOFOLLOW:
				break;

			case NONBLOCK:
				break;

			case RSYNC:
				break;

			case SYNC:
				break;

			case TRUNC:
				break;
				
			case COMMAND:{//command
				pid_t childPid = fork();
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
						for(int i = optind; i < optEnd; i++)
						{
							printf("%s ", argv[i]);
						}
						printf("\n");
				}
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
				cmdList[numCmd++].name = argv[tempPos];
			}	break;
			
		}
	}
	for (int i = 0; i < numFid; i++){
		close (fidList[i]);
	}	
	free(pidList);
	free(fidList);
	exit(numErrors);
}
