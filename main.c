#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//int optid = 1; //tracks which read in argument we'll read next
//char* optarg; //will store the arguments for the option as we move it in.



int main(int argc, char* argv[]){
	//set up dynamic arrays
	//there will never be more than argc child processes, so this is more than enough.
	pid_t* pidList = (pid_t*) malloc(argc*sizeof(pid_t));
	int* fidList = (int*) malloc(argc*sizeof(int));
	int numPid = 0;
	int numFid = 0;

	struct option optionlist[] = {
		//*name, hasarg, *flag, val
		{"rdonly",  1, 0, 'r'},
		{"wronly",  1, 0, 'w'},
		{"verbose", 0, 0, 'v'},
		{0       ,  0, 0,  0 } 
		//last element indicated by 0's
	}; //********expand this to be variable later

	int optVal; //will store the val returned by getopt_long
	int option_ind = 0;
	while (1){ //*********check if this is the correct limit
		optVal = getopt_long(argc, argv, "", optionlist, &option_ind);
		if (optVal == -1){
			printf("%s\n", ("break on -1"));
			break;
		}
		//use optVal for switch statements, option_ind for access
		switch (optVal) {
			case 'r':
				//we open the file as pointed to by optarg in read only mode
				fidList[numPid++] = open(optarg, O_RDONLY);
				//increment fid?
				printf("%s\n", ("Opened in read only."));
				printf("optarg: %s\n", optarg);
				printf("%d\n", (fidList[numPid-1]));
				break;
			case 'w':
				fidList[numPid++] = open(optarg, O_WRONLY);
				printf("%s\n",("Opened in write only."));
				printf("%d\n", (fidList[numPid-1]));
				break;
			case 'v':
				//option ind includes --verbose
				for (int i = optind; i < argc; i++)
				{
					printf("%s ", argv[i]);
				}
				break;
			default:
				printf("%s\n", "No match found.");
		}
	}
	for (int i = 0; i < numFid; i++){
		close (fidList[i]);
	}	
	free(pidList);
	free(fidList);
}