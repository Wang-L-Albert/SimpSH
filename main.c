
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resouce.h>
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
#define IGNORE 'u'
#define DEFAULT 'v'
#define PAUSE 'w'
#define PROFILE 'x'

struct command{
	int pid;
	char* name;
	int cmdPos;
	int cmdEnd;
	char cmdArgs[200];
};

	struct rusage p_start, c_start, p_end, c_end;
	time_t t_sec, t_usec, u_sec, u_microsec, s_sec, s_microsec;
	time_t t_c_sec, t_c_usec, c_u_sec, c_u_microsec, c_s_sec, c_s_microsec;
	int numErrors = 0;

void catch_sig(int sigNum){
	fprintf(stderr, "Signal %d was caught. \n", sigNum);
	exit(sigNum);
}

int profileEnd(struct rusage* usage, time_t u_second, time_t u_microSecond, time_t s_second, time_t s_microSecond, time_t totalSec, time_t totalMicrosec ){
	int getTime2 = getrusage(RUSAGE_SELF, usage);
	if (getTime2 == -1){
		fprintf(stderr, "Getting rusage for opening file %s in read only mode failed. \n", optarg);
		numErrors++;
	}
	//now calculate total time
	//calculate user time, start with microsec
	u_microSecond = p_end.ru_utime.tv_usec-p_start.ru_utime.tv_usec;
	//check to see if negative, if so then adjust for time
	if (u_microSecond < 0){
		p_end.ru_time.tv_sec--;
		u_microSecond += 1000000;
	}
	u_second = p_end.ru_utime.tv_sec - p_start.ru_utime.tv_sec;
		//calculate kernel time, start with microsec
	s_microSecond = p_end.ru_stime.tv_usec-p_start.ru_stime.tv_usec;
	//check to see if negative, if so then adjust for time
	if (s_microSecond < 0){
		p_end.ru_time.tv_sec--;
		s_microSecond += 1000000;
	}
	s_second = p_end.ru_stime.tv_sec - p_start.ru_stime.tv_sec;
	t_sec = u_second + s_second;
	t_usec = u_microSecond + s_microSecond
	printf("Completed in %d seconds and %d microseconds. \n %d seconds and %d microseconds were spent in user mode. \n 
		%d seconds and %d microseconds were spent in kernel mode.", totalSec, totalMicrosec, u_second, u_microSecond, s_second, s_microSecond);

}


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
	static int profile_flag = 0;
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
			/*	if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
					if (getTime1 == -1){
						fprintf(stderr, "Getting rusage for opening file %s in read only mode failed. \n", optarg);
						numErrors++;
						fidList[numFid++] = -1;
						continue;
					}
				}*/
				//we open the file as pointed to by optarg in read only mode
				fid = open(optarg, O_RDONLY | oflags, 0777);
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
		//		profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
			}
				break;
			case WRONLY://wronly
			{
				if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
		/*		if(profile_flag) {//get time before it processes
					int getTime1 = getrusage(RUSAGE_SELF, &p_start);
					if (getTime1 == -1){
						fprintf(stderr, "Getting rusage for opening file %s in write only mode failed. \n", optarg);
						numErrors++;
						fidList[numFid++] = -1;
						continue;
					}
				}*/
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
		//		profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
		
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
					//profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);

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

			//		profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
				}
				break;
			case WAIT:
				{
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

					}

					//DOFORPARENT
					if(profile_flag){//get time after it processes and calculate total time
						//for parent
						int getTime2 = getrusage(RUSAGE_SELF, p_end);
						if (getTime2 == -1){
							fprintf(stderr, "Getting rusage for wait failed. \n", optarg);
							numErrors++;
							continue;
						}
						//now calculate total time
						//calculate user time, start with microsec

						u_microsec = p_end.ru_utime.tv_usec - p_start.ru_utime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (u_microsec < 0){
							p_end.ru_time.tv_sec--;
							u_microsec += 1000000;
						}
						u_sec = p_end.ru_utime.tv_sec - p_start.ru_utime.tv_sec;	

						//calculate kernel time, start with microsec
						s_microsec = p_end.ru_stime.tv_usec - p_start.ru_stime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (s_microsec < 0){
							p_end.ru_time.tv_sec--;
							s_microsec += 1000000;
						}
						s_sec = p_end.ru_stime.tv_sec - p_start.ru_stime.tv_sec;

						t_sec = u_sec + s_sec;
						t_usec = u_microsec + s_microsec

						//child
						int getTime2 = getrusage(RUSAGE_CHILDREN, c_end);
						if (getTime2 == -1){
							fprintf(stderr, "Getting rusage for all children failed. \n", optarg);
							numErrors++;
							continue;
						}
						//now calculate total time
						//calculate user time, start with microsec

						c_u_microsec = c_end.ru_utime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (c_u_microsec < 0){
							c_end.ru_time.tv_sec--;
							c_u_microsec += 1000000;
						}
						c_u_sec = c_end.ru_utime.tv_sec;	

						//calculate kernel time, start with microsec
						c_s_microsec = c_end.ru_stime.tv_usec;
						//check to see if negative, if so then adjust for time
						if (c_s_microsec < 0){
							c_end.ru_time.tv_sec--;
							c_s_microsec += 1000000;
						}
						c_s_sec = c_end.ru_stime.tv_sec;

						t_c_sec = c_u_sec + c_s_sec;
						t_c_usec = c_u_microsec + c_s_microsec;

						printf("\"--pipe\" completed in %d seconds and %d microseconds. \n %d seconds and %d microseconds were spent in user mode. \n 
							%d seconds and %d microseconds were spent in kernel mode.", t_sec, t_usec, u_sec, u_microsec, s_sec, s_microsec);
						printf("All children completed in %d seconds and %d microseconds. \n %d seconds and %d microseconds were spent in user mode. \n 
							%d seconds and %d microseconds were spent in kernel mode.", t_c_sec, t_c_usec, c_u_sec, c_u_microsec, c_s_sec, c_s_microsec);

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
				//	profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
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
					if (optarg == NULL){
						fprintf(stderr, "No signal number passed into --catch. \n");
						numErrors++;
						continue;
					}
					if(verbose_flag) printf("--%s %s\n", optionlist[option_ind].name, optarg);
	/*				if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for catch failed. \n");
							numErrors++;
							continue;
						}
					}	 */
					int signalNum = atoi(optarg);
					signal(signalNum, catch_sig);
	//			profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
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
			/*		if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for ignore failed. \n");
							numErrors++;
							continue;
						}
					}	*/
					int signalNum = atoi(optarg);
					signal(signalNum, SIG_IGN);
					//profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
					
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
			/*		if(profile_flag) {//get time before it processes
						int getTime1 = getrusage(RUSAGE_SELF, &p_start);
						if (getTime1 == -1){
							fprintf(stderr, "Getting rusage for default failed. \n");
							numErrors++;
							continue;
						}
					}	*/
					int signalNum = atoi(optarg);
					signal(signalNum, SIG_DFL);
			//		profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
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
				profileEnd(&p_end, u_sec, u_microsec, s_sec, s_microsec, t_sec, t_usec);
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
