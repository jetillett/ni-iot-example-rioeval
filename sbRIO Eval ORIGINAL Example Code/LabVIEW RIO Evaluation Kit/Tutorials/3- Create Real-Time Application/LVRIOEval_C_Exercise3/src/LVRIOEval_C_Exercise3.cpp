#include <LVRIOEval_C_Exercise3.h>

#define MY_PRIORITY (49)
#define MAX_SAFE_STACK (8*1024)
#define NSEC_PER_SEC (1000000000)
enum MachineState {Stage1, Stage2, Stage3, Alarm, Shutdown};

MachineState state = Stage1;
double amplitude = 0;
bool uiStop = false;


//Prefault the stack so that future stack faults wont impact determinism
void stack_prefault(void) {
	unsigned char dummy[MAX_SAFE_STACK];
	memset(dummy, 0, MAX_SAFE_STACK);
	return;
}

//TCP server implementation to send data from the RIO target to a remote TCP client using port 10117
void* TCPServer(void* threadid) {

	int sockfd, newsockfd, portnum;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr; //server and client address

	//port used to communicate with server
	portnum = 10117;

	//create IP domain socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr,"Error creating the socket.\n");
	}

	//make sure serv_addr is cleared and then set values for each member
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portnum);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	int binderror = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (binderror < 0)
		fprintf(stderr, "Error Binding.\n");

    //initiate connect to server address(struct sockaddr *) &serv_addr on socket
	listen(sockfd,1);

	clilen = sizeof(cli_addr);

	bool stop = false;

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	while(!stop) {

		if ((write(newsockfd, &amplitude, sizeof(amplitude))) < 0) {
			fprintf(stderr,"Error reading data from socket.\n");
			stop = true;
		}

		if ((write(newsockfd, &state, sizeof(state))) < 0) {
			fprintf(stderr,"Error reading data from socket.\n");
			stop = true;
		}

		if ((write(newsockfd, &uiStop, sizeof(uiStop))) < 0) {
			fprintf(stderr,"Error reading data from socket.\n");
			stop = true;
		}

		uiStop = stop;
		usleep(500000);
	}

	close(newsockfd);
	close(sockfd);
	pthread_exit(NULL);
}


int main() {

	/*RT and execution variables*/
	struct sched_param schedparam;
	struct timespec t;
	uint64_t loopPeriod = 75000000; //ns (75ms)

	/*State machine and data variables*/
	NiFpga_Session session;
	bool smstop = false; //state machine stop command
	int vibSignalSize = 800;
	int vibSignal[vibSignalSize];
	double vibSignald[vibSignalSize];
	uint vibFIFOremaining;
	uint audioFIFOremaining;
	NiFpga_Bool estop = false;
	int16_t positionCurrent = 0;
	int positionThres[3] = {160, 320, 480};
	int alarmThres = 4;

	/*Threading variables*/
	pthread_t tcpserverthread;


	//Declare as a real time task
	schedparam.sched_priority = MY_PRIORITY;
	if(sched_setscheduler(0, SCHED_FIFO, &schedparam) == -1) {
			perror("sched_setscheduler failed");
			exit(-1);
	}

	//Lock memory
	if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
			perror("mlockall failed");
			exit(-2);
	}

	//Pre-fault stack
	stack_prefault();

	//Start after one second
	clock_gettime(CLOCK_MONOTONIC,&t);
	t.tv_sec++;

	//Load FPGA Interface Library
	NiFpga_Status status = NiFpga_Initialize();


	/* (CODE)
	 * Implement FPGA Open and Run here
	 *
	 */

	//spawn off the TCP Server to act in its own thread
	int tcptid = pthread_create(&tcpserverthread, NULL, TCPServer, (void *) 0);

	//Set the entry state to Stage1 and start state machine
	printf("Starting state machine...\n");
	while(NiFpga_IsNotError(status) && !smstop && !estop) {
	//Executes while no FPGA errors and both smstop and estop are false

		//Suspend thread until request time has elapsed
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		/* (CODE)
		 * FPGA FIFO Read Vibration Signal I32 here using the following variables and a wait of 5000ms
		 * 		int vibSignalSize = 800;
				int vibSignal[vibSignalSize];
				double vibSignald[vibSignalSize];
		 *
		 */

		//Convert I32 Vibration Signal array into a double array
		for(int i = 0; i < vibSignalSize; i++){
			vibSignald[i] = vibSignal[i]/10000000.0;
		}


		//Determine amplitude of the vibration signal using the "calc_amp" function defined in LVRIOEvalEx3.h
		amplitude = calc_amp(vibSignald, vibSignalSize);



		switch(state)
		{
		case Stage1:
			/* (CODE)
			 * FPGA Read EStop Boolean and update the following variable
			 * 		NiFpga_Bool estop;
			 */

			/* (CODE)
			 * FPGA Read Encoder Position I16 and update the following variable
			 * 		int16_t positionCurrent = 0;
			 */

			/* (CODE)
			 * FPGA Write PWM Boolean using a constant value of true
			 */

			//Check if the vibration signal and go to alarm state if alarm threshold is reached
			if(amplitude >= alarmThres)
				state = Alarm;
			else
				//Check if the Encoder Position has reached the threshold and, if so, transition to next stage
				if(positionCurrent >= positionThres[0]) {
					state = Stage2;
				}
				else
					state = Stage1;

			break;

		case Stage2:
			/* (CODE)
			 * FPGA Read EStop Boolean and update the following variable
			 * 		NiFpga_Bool estop;
			 */

			/* (CODE)
			 * FPGA Read Encoder Position I16 and update the following variable
			 * 		int16_t positionCurrent = 0;
			 */

			/* (CODE)
			 * FPGA Write PWM Boolean using a constant value of true
			 */


			//Check if the vibration signal and go to alarm state if alarm threshold is reached
			if(amplitude >= alarmThres)
				state = Alarm;
			else
				//Check if the Encoder Position has reached the threshold and, if so, transition to next stage
				if(positionCurrent >= positionThres[1]) {
					state = Stage3;
				}
				else
					state = Stage2;
			break;

		case Stage3:
			/* (CODE)
			 * FPGA Read EStop Boolean and update the following variable
			 * 		NiFpga_Bool estop;
			 */

			/* (CODE)
			 * FPGA Read Encoder Position I16 and update the following variable
			 * 		int16_t positionCurrent = 0;
			 */

			/* (CODE)
			 * FPGA Write PWM Boolean using a constant value of true
			 */

			//Check if the vibration signal and go to alarm state if alarm threshold is reached
			if(amplitude >= alarmThres)
				state = Alarm;
			else
				//Check if the Encoder Position has reached the threshold and, if so, transition to next stage
				if(positionCurrent >= positionThres[2]) {
					state = Alarm;
				}
				else
					state = Stage3;
			break;

		case Alarm:
			//Alarm state has been triggered. Notify the user by producing a sound and then proceed to Shutdown.
			/* (CODE)
			 * FPGA Write U32 Gain value to 5000
			 * 		NiFpga_Bool estop;
			 */

			/* (CODE)
			 * FPGA FIFO Write I32 AudioFIFO using the following variables and a 5000ms wait
			 * 		boingsound (defined in LVRIOEval_C_Exercise3.h)
			 * 		boingsoundsize (defined in LVRIOEval_C_Exercise3.h)
			 * 		audioFIFOremaining (defined in LVRIOEval_C_Exercise3.h)
			 */

			//sleep for 2 seconds while sound plays
			usleep(2000000);

			state = Shutdown;
			break;

		case Shutdown:
			//Shutdown state has been triggered. Reset FPGA values and exit state machine loop

			positionCurrent = 0;
			smstop = true;

			/* (CODE)
			 * FPGA Write Encoder Position I16 using the following variable
			 * 		int16_t positionCurrent;
			 */


			/* (CODE)
			 * FPGA Write PWM Boolean using a constant value false
			 */

			break;
		}

		//Schedule next execution start time
		t.tv_nsec += loopPeriod;

		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}

	}

	if(!NiFpga_IsNotError(status))
		printf("Main loop exit due to FPGA Session Error: %d\n", status);

	if(smstop)
		printf("State Machine has finished. Exiting program\n");

	printf("Calling FPGA close and code exit... \n");
	//Close the FPGA session
	NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));

	//Call Finalize() before exiting and after closing FPGA session
	NiFpga_MergeStatus(&status, NiFpga_Finalize());


	return 0;
}

