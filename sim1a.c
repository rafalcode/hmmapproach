/* from http://preshing.com/20111007/how-to-generate-random-timings-for-a-poisson-process
 *
 * This was about the number of earthquakes of Richter scale 4 or above every year throughout
 * the world as 13000. 365*1440/13000 is 40, so every 40 minutes there's an earthquake.
 *
 * Put another way, the average time between earthquakes is 40 minutes.
 *
 * To get the rate, we invert, so every 1 minute there's a 1/40 chance of an earthquake.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define SETSEED 5 /*  if -DUNPREDRA is not fed in */

int main(int argc, char *argv[])
{
	if(argc!=3) {
		printf("Error. Pls supply 3 arguments 1: Length untrained seq 2) length training data 3) avg numb of state change per training length.\n");
		printf("Four file will be output: \"unkhmm.symb\", unkhmm.states\", \"trnhmm.symb\" and finally \"trnhmm.states\"");
		exit(EXIT_FAILURE);
	}

		FILE *fusy/*fileptr unkpown symbols*/=fopen("./unkhmm.symb", "w");
		FILE *fusta/*fileptr unknown states*/=fopen("./unkhmm.states", "w");
        FILE *ftsy/*fileptr train symbs*/=fopen("./trnhmm.symb", "w");
        FILE *ftsta/*fileptr train states*/=fopen("./trnhmm.states", "w");
    int sqlen=atoi(argv[1]);
    float evplen=atof(argv[2]); /* events per length */
#ifdef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

    float lam = evplen/sqlen;
    int i=0;
    float ura; /*  variable to hold one uniform random variable 0-1 */
    float wt /* waiting time */;
    float twt=.0, oldtwt=.0;
    char cc[2]={'A', 'B'};
    unsigned char c=0;

    while(1) { /* infinite loop to be broken out of */
        ura= 1. - (float)random()/(RAND_MAX+1L); /* because we don't want to have zero, ever, for the log function */
        wt= -log(ura)/lam;
        twt += wt;
#ifdef DBG
        printf("Free up to %.2f: ", twt); 
#endif
        while((i<=(int)twt) & (i<sqlen)) {
            fputc(ftsy, cc[0x01&c]);
            i++;
        }
        printf("\n"); 
        if(twt >= sqlen)
            break;
//        printf("wt=%.3f\n", wt); 
        c++;
// if(i >= NLOOPS)
    }
    printf("\ni is %i\n", i); 
    return 0;
}
