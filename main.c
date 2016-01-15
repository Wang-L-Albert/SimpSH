#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
int numErrors = 0;
int main(int argc, char* argv[]){
	//set up dynamic arrays
	//there will never be more than argc child processes, so this is more than enough.
	pid_t* pidList = (pid_t*) malloc(argc*sizeof(pid_t));
	int* fidList = (int*) malloc(argc*sizeof(int));
	int numPid = 0;
	int numFid = 0;
	int fid = 0;
	static int verbose_flag = 0;
	struct option optionlist[] = {
		//*name, hasarg, *flag, val
		{"rdonly",  1, 0, 'r'},
		{"wronly",  1, 0, 'w'},
		{"verbose", 0, &verbose_flag, 1},
		{"command", 1, 0, 'c'},
		{0       ,  0, 0,  0 } 
		//last element indicated by 0's
	}; //********expand this to be variable later

	int optVal; //will store the val returned by getopt_long
	int option_ind = 0;
	while (1){ 
		optVal = getopt_long(argc, argv, "", optionlist, &option_ind); //option_ind is the option's index in argv[].
		//global var optarg now points to potion_ind+1, and optind to the index of the next index (first non-option 
		//in argv[] if no more options.
		if (optVal == '?'){//occurs when we find an option without the appropriate argument
			fprintf(stderr, "%s is missing an argument\n", optionlist[option_ind].name);
			numErrors++;
			continue;
		}
		if (optVal == -1){//no more options to parse
			break;
		}
		//use optVal for switch statements, option_ind for access
		switch (optVal) {
			case 'r'://rdonly
				//we open the file as pointed to by optarg in read only mode
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
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
			case 'w'://wronly
				if(verbose_flag) printf("--%s %s\n",optionlist[option_ind].name, optarg);
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
			// case 'v'://verbose
			// 	//option ind includes --verbose
			// 	for (int i = optind; i < argc; i++)
			// 	{
			// 		printf("%s ", argv[i]);
			// 	}
			// 	break;
			case 'c':{//command
				pid_t childPid = fork();
				if (childPid == 0){ //if it is a child
					//re-reroute I/O
					//at this point, optarg points to the next arg, optind next ind
					//need a variable for end of our stream of arguments
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
							printf("--%s ", optionlist[option_ind].name);
							printf("%s ", optarg);
							for(int i = optind; i < optEnd; i++)
							{
								printf("%s ", argv[i]);
							}
							printf("\n");
					}
					/* int tempOptInd = optind;
					int tempOpt_Ind = 0;
					printf("optind before getopt: %d\n", optind);
					getopt_long(argc, argv, "", optionlist, &tempOpt_Ind);
					printf("optind after getopt: %d\n", optind);
					if (optind <= tempOptInd){//would only be true if no more options and it went back to first non-opt arg
						printf("argc:   %d\n",argc);
						optEnd = argc;

					} else { //are more --commands
				
						optEnd = optind-2;
						optind = optEnd;//undo the search, we only want to verify there was another one. 
					printf("optind inside else:  %d\n", optind);
					}
					*/
					//now three times from option_ind+1 to get the I/O re-routes
					int argParse = optind-1;
					//printf("option_ind: %d\n", option_ind);
					//printf("OptEnd: %d\n", optEnd);
					//printf("tempOptInd: %d\n", tempOptInd);
					//printf("argParse: %d\n", argParse);
					// dup2(3,0);
					// dup2(5,2);
					// printf("NOOOOOOOOOOOOOOOO 1\n" );
					// dup2(4,1);
					
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
					newArgs = (char*) malloc((argSize+1)*sizeof(char *));/////may be used uninit in this func
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
					int failedExec = execvp(newCmd, newArgs);
					if (failedExec == -1){
						fprintf(stderr, "Executing command %s failed. \n", newCmd);
						numErrors++;
						exit(1);
					}
				} 
				//int status;
				//waitpid(childPid, &status, 0);
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
