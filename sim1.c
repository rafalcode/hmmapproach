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
#define MX 20
#define NLOOPS 20

int main(int argc, char *argv[])
{
#ifdef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

//    float lam = 1./40;
    /* 13000 earthquakes are recorded per year on average, let's work in minutes. 1440 minutes per day, 365 days in the year */
    float lam = 1./(365.*1440/13000);
    printf("lambda = %3.4f\n", lam); 
    int i=0;
    float ura; /*  variable to hold one uniform random variable 0-1 */
    float nxtime /* next time for event, starting from zero */;
    float twt=.0; /* total waiting time .. cumulative on the "next times" */

    while(1) { /* infinite loop to be broken out of */
        ura= 1. - (float)random()/(RAND_MAX+1L); /* because we don't want to have zero, ever, for the log function */
        nxtime= -logf(ura)/lam;
        twt += nxtime;
//        printf("wt=%.3f\n", wt); 
        i++;
// if(i >= NLOOPS)
        if(twt >= 365.*1440)
            break;
    }
    printf("total num earthquakes in this replicate year is %i\n", i); 
    return 0;
}
