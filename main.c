
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>

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
#define IGNORE 'u'
#define DEFAULT 'v'
#define PAUSE 'w'
#define PROFILE 'x'
#define PARALLEL 'y'

struct command{
	int pid;
	char* name;
	int cmdPos;
	int cmdEnd;
	char cmdArgs[200];
};

	int numErrors = 0;
	struct rusage p_start, c_start, p_end, c_end;
	time_t t_sec=0, t_usec=0, u_sec=0, u_microsec=0, s_sec=0, s_microsec=0;
	time_t t_c_sec=0, t_c_usec=0, c_u_sec=0, c_u_microsec=0, c_s_sec=0, c_s_microsec=0;
	float totalParentTime;
	float totalChildTime;

void catch_sig(int sigNum){
	fprintf(stderr, "Signal %d was caught. \n", sigNum);
	exit(sigNum);
}

void profileEnd(struct rusage* usage){
	int getTime2 = getrusage(RUSAGE_SELF, usage);
	if (getTime2 == -1){
		fprintf(stderr, "Getting rusage for opening file %s in read only mode failed. \n", optarg);
		numErrors++;
	}
	//now calculate total time
	//calculate user time, start with microsec
	//printf("u_microsecond issss: %d \n", (int)usage->ru_utime.tv_usec );

	float firstUserTime = (float) p_start.ru_utime.tv_sec + (float)((p_start.ru_utime.tv_usec)/1e6); 
	float finalUserTime = (float) usage->ru_utime.tv_sec + (float)((usage->ru_utime.tv_usec)/1e6); 
	float totalUserTime = finalUserTime - firstUserTime;

	float firstKernelTime = (float) p_start.ru_stime.tv_sec + (float)((p_start.ru_stime.tv_usec)/1e6); 
	float finalKernelTime = (float) usage->ru_stime.tv_sec + (float)((usage->ru_stime.tv_usec)/1e6); 
	float totalKernelTime = finalKernelTime - firstKernelTime;

	float totalTime = totalUserTime + totalKernelTime;
/*
	*u_microSecond = usage->ru_utime.tv_usec - p_start.ru_utime.tv_usec;
	//check to see if negative, if so then adjust for time
	if (*u_microSecond < 0){
		usage->ru_utime.tv_sec--;
		*u_microSecond += 1000000;
	}
	*u_second = usage->ru_utime.tv_sec - p_start.ru_utime.tv_sec;
		//calculate kernel time, start with microsec
	*s_microSecond = usage->ru_stime.tv_usec - p_start.ru_stime.tv_usec;
	//check to see if negative, if so then adjust for time
	if (*s_microSecond < 0){
		usage->ru_stime.tv_sec--;
		*s_microSecond += 1000000;
	}
	*s_second = usage->ru_stime.tv_sec - p_start.ru_stime.tv_sec;
	*totalSec = *u_second + *s_second;
	*totalMicrosec = *u_microSecond + *s_microSecond; */
	
	printf("Completed in %f seconds. \n %f seconds were spent in user mode. \n %f seconds were spent in kernel mode. \n", totalTime, totalUserTime, totalKernelTime);

}

int main(int argc, char* argv[]){
	//set up dynamic arrays
	//there will never be more than argc chilu processes, so this is more than enough.
	struct command* cmdList =  malloc(argc*sizeof(struct command));
	int* fidList = (int*) malloc(argc*sizeof(int));
	int numCmd = 0;
	int numFid = 0;
	int fid = 0;
	int oflags = 0;
	int maxChildren = 0;
	int numChildren = 0;

	static int verbose_flag = 0;
	static int profile_flag = 0;
	static int parallel_flag = 0;
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
		{"catch", hasarg, noflag, CATCH},
		{"ignore", hasarg, noflag, IGNORE },
		{"default", hasarg, noflag, DEFAULT},
		{"pause", noarg, noflag, PAUSE},
		{"profile", noarg, &profile_flag, 1},
		{"parallel", hasarg, noflag, PARALLEL},
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
			{
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);

				//we open the file as pointed to by optarg in read only mode
				fid = open(optarg, O_RDONLY | oflags, 0777);
				if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
					if (getTime1 == -1){
						fprintf(stderr, "Getting rusage for opening file %s in read only mode failed. \n", optarg);
						numErrors++;
						fidList[numFid++] = -1;
						continue;
					}
					//printf("TIMEEE:  %d \n",(int)p_start.ru_utime.tv_usec);
				}
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in read only mode failed. \n", optarg);
					numErrors++;
					fidList[numFid++] = -1;
					continue;
				}
				fidList[numFid++] = fid;
				//increment fid?
				//printf("%s\n", ("Opened in read only."));
				//printf("optarg: %s\n", optarg);
				//printf("%d\n", (fidList[numFid-1]));

				if (profile_flag){
					profileEnd(&p_end);
				}
			}
				break;
			case WRONLY://wronly
			{
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
				if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
					if (getTime1 == -1){
						fprintf(stderr, "Getting rusage for opening file %s in write only mode failed. \n", optarg);
						numErrors++;
						fidList[numFid++] = -1;
						continue;
					}
				}
				fid = open(optarg, O_WRONLY | oflags, 0777);
				if (fid == -1){ //there was an error opening a file
					fprintf(stderr, "Opening file %s in write only mode failed. \n", optarg);
					numErrors++;
					fidList[numFid++] = -1;
					continue;
				}
				fidList[numFid++] = fid;
				//printf("%s\n",("Opened in write only."));
				//printf("optarg: %s\n", optarg);
				//printf("%d\n", (fidList[numFid-1]));
				if (profile_flag){
					profileEnd(&p_end);
				}
			}
				break;
			case RDWR:
				{
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for opening file %s in read and write mode failed. \n", optarg);
							numErrors++;
							fidList[numFid++] = -1;
							continue;
						}
					}
					fid = open(optarg, O_RDWR | oflags, 0777);
					if (fid == -1){ //there was an error opening a file
						fprintf(stderr, "Opening file %s in read and write only mode failed. \n", optarg);
						numErrors++;
						fidList[numFid++] = -1;
						continue;
					}
					fidList[numFid++] = fid;

					if (profile_flag){
						profileEnd(&p_end);
					}
				}
				
				break;
			case PIPE:
				{
					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
				/*	if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for opening a pipe failed. \n");
							numErrors++;
							continue;
						}
					}*/
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

					if (profile_flag){
						profileEnd(&p_end);
					}
				}
				break;
			case WAIT:
				{
					float c_firstUserTime, c_finalUserTime;
					float c_totalUserTime = 0;
					float c_firstKernelTime, c_finalKernelTime;
					float c_totalKernelTime = 0;
					float c_totalTime = 0;

					float p_firstUserTime, p_finalUserTime, p_totalUserTime;
					float p_firstKernelTime, p_finalKernelTime, p_totalKernelTime;
					float p_totalTime = 0;


					if(verbose_flag) printf("--%s\n", optionlist[option_ind].name);
					//DO FOR PARENT
					if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for wait failed. \n");
							numErrors++;
							continue;
						}
					}
					//printf("Entering wait \n");
					for (int b = 0; b < numFid; b++){
						//printf("closing down fid's \n");
						close (fidList[b]);
					}
					int exitStatus;
					//printf("numcmd = %d\n", numCmd);

					//DO FOR CHILDREN
					
					for (int i = 0; i < numCmd; i++){
						if(profile_flag) {//get time before it processes
							int getCTime1 = getrusage(RUSAGE_CHILDREN, &c_start);
							if (getCTime1 == -1){
								fprintf(stderr, "Getting rusage for child failed. \n");
								numErrors++;
								continue;
							}
						}
						//printf("waiting on %d, %s\n", i, cmdList[i].name);
						waitpid(cmdList[i].pid, &exitStatus, 0);
						//printf("Finished waiting on %s\n", cmdList[i].name);
						int exitNorm = WIFEXITED(exitStatus);
						if (exitNorm){ //if exited normally
							//printf("%s exited normally\n", cmdList[i].name);
							//printf("%s is in position %d \n", cmdList[i].name, cmdList[i].cmdPos);
							//printf("%s ends at %d \n", cmdList[i].name, cmdList[i].cmdEnd);
							int errNum = WEXITSTATUS(exitStatus); //get error status
							printf("%d ", errNum); //print out error number
							//print out option name + arguments
							printf("%s\n", cmdList[i].cmdArgs);
						}

						if(profile_flag){//get time after it processes and calculate total time
							int getCTime2 = getrusage(RUSAGE_CHILDREN, &c_end);
							if (getCTime2 == -1){
								fprintf(stderr, "Getting rusage for child failed. \n");
								numErrors++;
								continue;
							}
							/*
							//now calculate total time
							//calculate user time, start with microsec
							c_u_microsec = c_end.ru_utime.tv_usec-c_start.ru_utime.tv_usec;
							//check to see if negative, if so then adjust for time
							if (u_microsec < 0){
								c_end.ru_utime.tv_sec--;
								u_microsec += 1000000;
							}
							c_u_sec = c_end.ru_utime.tv_sec - c_start.ru_utime.tv_sec;		

							//calculate kernel time, start with microsec
							c_s_microsec = c_end.ru_stime.tv_usec-c_start.ru_stime.tv_usec;
							//check to see if negative, if so then adjust for time
							if (s_microsec < 0){
								c_end.ru_stime.tv_sec--;
								s_microsec += 1000000;
							}
							c_s_sec = c_end.ru_stime.tv_sec - c_start.ru_stime.tv_sec;

							t_c_sec += u_sec + s_sec;
							t_c_usec += u_microsec + s_microsec;*/
							c_firstUserTime = (float) c_start.ru_utime.tv_sec + (float)((c_start.ru_utime.tv_usec)/1e6); 
							c_finalUserTime = (float) c_end.ru_utime.tv_sec + (float)((c_end.ru_utime.tv_usec)/1e6); 
							c_totalUserTime += c_finalUserTime - c_firstUserTime;

							c_firstKernelTime = (float) c_start.ru_stime.tv_sec + (float)((c_start.ru_stime.tv_usec)/1e6); 
							c_finalKernelTime = (float) c_end.ru_stime.tv_sec + (float)((c_end.ru_stime.tv_usec)/1e6); 
							c_totalKernelTime += c_finalKernelTime - c_firstKernelTime;

							c_totalTime += c_totalUserTime + c_totalKernelTime;
						
						}


					}
					if (parallel_flag){
						numChildren = 0;
					}
					//DOFORPARENT
					if(profile_flag){//get time after it processes and calculate total time
						int getTime2 = getrusage(RUSAGE_SELF, &p_end);
						if (getTime2 == -1){
							fprintf(stderr, "Getting rusage for wait failed. \n");
							numErrors++;
							continue;
						}
						/*
						//now calculate total time
						//calculate user time, start with microsec
						time_t u_microsec = p_end.ru_utime.tv_usec-p_start.ru_utime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (u_microsec < 0){
							p_end.ru_utime.tv_sec--;
							u_microsec += 1000000;
						}
						time_t u_sec = p_end.ru_utime.tv_sec - p_start.ru_utime.tv_sec;	

						//calculate kernel time, start with microsec
						time_t s_microsec = p_end.ru_stime.tv_usec-p_start.ru_stime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (s_microsec < 0){
							p_end.ru_stime.tv_sec--;
							s_microsec += 1000000;
						}

						time_t s_sec = p_end.ru_stime.tv_sec - p_start.ru_stime.tv_sec;

						s_sec = p_end.ru_stime.tv_sec - p_start.ru_stime.tv_sec;

						t_sec = u_sec + s_sec;
						t_usec = u_microsec + s_microsec;
						*/
						p_firstUserTime = (float) p_start.ru_utime.tv_sec + (float)((p_start.ru_utime.tv_usec)/1e6); 
						p_finalUserTime = (float) p_end.ru_utime.tv_sec + (float)((p_end.ru_utime.tv_usec)/1e6); 
						p_totalUserTime = p_finalUserTime - p_firstUserTime;

						p_firstKernelTime = (float) p_start.ru_stime.tv_sec + (float)((p_start.ru_stime.tv_usec)/1e6); 
						p_finalKernelTime = (float) p_end.ru_stime.tv_sec + (float)((p_end.ru_stime.tv_usec)/1e6); 
						p_totalKernelTime = p_finalKernelTime - p_firstKernelTime;

						p_totalTime = p_totalUserTime + p_totalKernelTime;
						
						printf("\"--wait\" completed in %f seconds. \n %f seconds were spent in user mode. \n %f seconds were spent in kernel mode. \n", p_totalTime, p_totalUserTime, p_totalKernelTime);
						printf("All children completed in %f seconds. \n %f seconds were spent in user mode. \n %f seconds were spent in kernel mode. \n", c_totalTime, c_totalUserTime, c_totalKernelTime);
					}
					
				}
				break;
			case CLOSE:
				{
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
		/*			if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for close failed. \n");
							numErrors++;
							continue;
						}
					}	*/
					int error = close(fidList[atoi(optarg)]);
					fidList[atoi(optarg)] = -1;
					if (error == -1){
						fprintf(stderr, "Closing file descriptor %d failed. \n", atoi(optarg));
						numErrors++;
						continue;
					}
					if (profile_flag){
						profileEnd(&p_end);
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
			case PARALLEL:
				{

				//	printf("Entering parallel     : \n");
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);

			//		printf("optarg     : %s\n", optarg);
					int checkValidInt = atoi(optarg); //get our argument for PARALLEL

			//		printf("Entering checkValidInt is     : %d\n", checkValidInt);
					if (!isdigit(*optarg) || checkValidInt==0){ //if argument is not an int, we throw an error
						fprintf(stderr, "Argument for --parallel was not a positive integer.\n");
						numErrors++;
						continue;
					}
					parallel_flag = 1;

			//		printf("checkValidInt is  %d\n", checkValidInt);
					maxChildren = checkValidInt;
			//		printf("MAx children is  %d\n", maxChildren);
					break;
				}
			case CATCH:
				{
					if (optarg == NULL){
						fprintf(stderr, "No signal number passed into --catch. \n");
						numErrors++;
						continue;
					}
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for catch failed. \n");
							numErrors++;
							continue;
						}
					}	 
					int signalNum = atoi(optarg);
					signal(signalNum, catch_sig);
					if (profile_flag){
						profileEnd(&p_end);
					}
				}
				break;
			case  IGNORE:
				{
					if(optarg == NULL){
						fprintf(stderr, "No signal number passed into --ignore. \n");
						numErrors++;
						continue;
					}
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for ignore failed. \n");
							numErrors++;
							continue;
						}
					}	
					int signalNum = atoi(optarg);
					signal(signalNum, SIG_IGN);
					if (profile_flag){
						profileEnd(&p_end);
					}
					
				}
				break;
			case  DEFAULT:
				{
					if (optarg == NULL){
						fprintf(stderr, "No signal number passed into --default. \n");
						numErrors++;
						continue;
					}
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
					if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for default failed. \n");
							numErrors++;
							continue;
						}
					}	
					int signalNum = atoi(optarg);
					signal(signalNum, SIG_DFL);
					if (profile_flag){
						profileEnd(&p_end);
					}
				}
				break;
			case  PAUSE:
				{
					if(verbose_flag) printf("--%s \n", optionlist[option_ind].name);
					pause();
				}
				break;
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
				if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
					if (getTime1 == -1){
						fprintf(stderr, "Getting rusage for command failed. \n");
						numErrors++;
						continue;
					}
				}	

				//check for false fd's
				int fidCheck = optind-1;
				int falseFid = 0;
				for (int i = 0; i < 3; i++){
					//printf("inside for loop: argParse: %d\n", argParse);
					//printf("filedescrip: %s\n", argv[argParse]);
					int newIO = strtol(argv[fidCheck++], NULL, 10);
					if (fidList[newIO] == -1){
						fprintf(stderr, "An invalid file descriptor was passed into command \n");
						falseFid = 1;
						numErrors++;
						break;
					}				
				}
				if (falseFid){
					break;
				}
				//check for valid fd's

				//check if allowed to make a new child, stall if can't
				if(parallel_flag){
					if (numChildren >= maxChildren){
					//if too many children already, wait for one to finish before starting another
						printf("Max number of subprocesses (%d) already running. Please wait.\n", numChildren);
						int waitOnChild = 0;
						//waitpid(-1, &waitOnChild,0);
						if (waitOnChild == -1){
							fprintf(stderr, "Waiting on a child process to finish failed. System cannot create a new subprocesses. Program shutting down.\n");
							numErrors++;
							exit(numErrors);
						}
						numChildren--;
					}
				}
				
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
				if(parallel_flag){
					numChildren++;
					printf("Adding child\n");
				}
				cmdList[numCmd].pid = childPid;
				cmdList[numCmd].cmdPos = optind+2;
				cmdList[numCmd].cmdEnd = optEnd;
				for (int d = optind+2; d < optEnd; d++){
					strcat(cmdList[numCmd].cmdArgs, argv[d]);
					strcat(cmdList[numCmd].cmdArgs, " ");
				}
				cmdList[numCmd++].name = argv[optind+2];
				if (profile_flag){
					profileEnd(&p_end);
				}
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
