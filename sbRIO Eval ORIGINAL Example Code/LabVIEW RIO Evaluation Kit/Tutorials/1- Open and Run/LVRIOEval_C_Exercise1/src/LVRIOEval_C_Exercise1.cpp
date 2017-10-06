
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <sched.h>
#include <sys/mman.h>
#include <NiFpga_LEDPWMFPGA.h>

#define MY_PRIORITY (49)
#define MAX_SAFE_STACK (8*1024)
#define NSEC_PER_SEC (1000000000)

//Prefault the stack so that future stack faults wont impact determinism
void stack_prefault(void) {
	unsigned char dummy[MAX_SAFE_STACK];
	memset(dummy, 0, MAX_SAFE_STACK);
	return;
}

//Return LED Pulse values for cRIO FPGAs requiring U8 inputs
uint8_t* LEDPulse(uint8_t *LEDData, int LEDDataSize, uint64_t loopperiod, double blinkperiod, int reltime) {

	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC,&t);

	//y(t)=A*sin(2*pi*f*t+p*rad*i)
	for(int i=0; i<=LEDDataSize-1; i++) {
	LEDData[i] = 255*sin(2*3.141592*(loopperiod/(blinkperiod*1000000000)*reltime+1.5708*i))+0.5;
	}
	return LEDData;
}

int main() {
	NiFpga_Session session;
	struct sched_param schedparam;
	struct timespec t;

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

	//Open a session with the FPGA, download and run the bitstream on the FPGA and store any error info in "status"
	printf("Opening FPGA Session and running bitfile...\n");
	NiFpga_MergeStatus(&status, NiFpga_Open("/home/admin/LVRIOEvalProject/NiFpga_LEDPWMFPGA.lvbitx",
												NiFpga_LEDPWMFPGA_Signature,
												"RIO0",
												0,
												&session));

	//Initialize variables that control the application behavior
	bool PulseLEDs = true; //false = no pulse, true = pulse
	uint64_t LoopPeriod = 15000000; //ns (15ms)
	double BlinkPeriod = 3; //s
	uint8_t LED0Intensity = 100; //%
	uint8_t LED1Intensity = 25; //%
	int numLEDs = 2;
	uint8_t LEDIntensityData[numLEDs]; //Data to transfer to FPGA
	int loopiterations = 1000;
	int count = 0;

	printf("Starting main execution loop...\n");
	while(NiFpga_IsNotError(status) && count<loopiterations) {

		//Suspend thread until request time has elapsed
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		if (PulseLEDs) {

			LEDPulse(LEDIntensityData, numLEDs, LoopPeriod, BlinkPeriod, count);
		}
		else {
			//Set specified intensity for each LED
			LEDIntensityData[0] = LED0Intensity*2.55;
			LEDIntensityData[1] = LED1Intensity*2.55;
		}

		//Send LED Intensity values to the FPGA
		NiFpga_MergeStatus(&status, NiFpga_WriteArrayU8(session,
													   NiFpga_LEDPWMFPGA_ControlArrayU8_LEDIntensity,
													   LEDIntensityData,
													   NiFpga_LEDPWMFPGA_ControlArrayU8Size_LEDIntensity));

		//Schedule next execution start time
		t.tv_nsec += LoopPeriod;

		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
		count++;
	}

	if(!NiFpga_IsNotError(status))
		printf("Main loop exit due to FPGA Session Error: %d\n", status);


	printf("Calling FPGA close and code exit... \n");
	//Close the FPGA session
	NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));

	//Call Finalize() before exiting and after closing FPGA session
	NiFpga_MergeStatus(&status, NiFpga_Finalize());

	return 0;
}
