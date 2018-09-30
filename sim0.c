/* trying to sort out waiting time formula */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define SETSEED 5 /*  if -DUNPREDRA is not fed in */
#define MX 20

int main(int argc, char *argv[])
{
#ifndef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

    if(argc!=2) {
        printf("Error. Pls supply 1 argument: integer, number of exponential replicates\n");
        exit(EXIT_FAILURE);
    }
    int numreps = atoi(argv[1]);
    float ar = -0.75;
    float ar2 = 0.25;
    float count =40.; /* Every 40 minutes, 1 earthquake */
    float ar3 = 1/count;
    int i=0, j;
    float ura; /*  variable to hold one uniform random variable 0-1 */
    float wt[6]={0};

    int lessthanav = 0, grtthanav = 0;

    while(1) { /* infinite loop to be broken out of when maxlen reaches a certain condition */
        ura= 1. - (float)rand()/(RAND_MAX);
        wt[0]= -1.0/ar*log(1+ura); 
        wt[1]= -1.0/ar*log(ura); 
        wt[2]= -log(ura)/ar;
        wt[3]= -1.0/ar2*log(1+ura); 
        wt[4]= -1.0/ar2*log(ura); 
        wt[5]= -log(ura)/ar3;
        if(wt[5] > count)
            grtthanav++;
        else if(wt[5]< count)
            lessthanav++;
        printf("For ura=%4.4f, logura= %4.4f wts=", ura, log(ura));
        for(j=0;j<6;++j) 
            printf("%.3f ", wt[j]);
        printf("\n"); 
        i++;
        if(i > numreps)
            break;
    }
    printf("Numb under avrate = %i, number over = %i.\n", lessthanav, grtthanav); 
    return 0;
}

/* NOTES:
 * Originally I was checking out where I might be going wrong witht he waiting time formula
 * and was comparing  -log(ura)/ar2; and -1.0/ar2*log(ura); but if you look at the latter,
 * div and mult are equal priority in C (unlike against + or -) so the first op
 * will be executed first, and then the second, making these two expressions identical
 *
 * Furthermore, the 1+ura must be wrong, and maybe was due to the 1-ura-avoiding-zero-issue */
