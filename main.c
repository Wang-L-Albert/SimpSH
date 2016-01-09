#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//int optid = 1; //tracks which read in argument we'll read next
//char* optarg; //will store the arguments for the option as we move it in.



int main(int argc, char* argv[]){
	//set up the options
	int fid;
	struct option optionlist[] = {
		{"rdonly", 1, 0, 'r'},
		{0       , 0, 0,  0 }
	}; //********expand this to be variable later

	int optVal; //will store the val returned by getopt_long
	int option_ind = 0;
	while (1){ //*********8check if this is the correct limit
		optVal = getopt_long(argc, argv, "", optionlist, &option_ind);
		if (optVal == -1){
			printf("%s\n", ("break on -1"));
			break;
		}
		//use optVal for switch statements, option_ind for access
		switch (optVal) {
			case 'r':
				//we open the file as pointed to by optarg in read only mode
				fid = open(optarg, O_RDONLY);
				printf("%s\n", ("Opened in read only."));
				printf("%d\n", (fid));
				break;
			default:
				printf("%s\n", "No match found.");
		}
	}
	close (fid);
}