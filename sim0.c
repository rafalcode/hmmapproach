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
#ifdef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

    float ar = -0.75;
    float ar2 = 0.25;
    int i=0, j;
    float ura; /*  variable to hold one uniform random variable 0-1 */
    float wt[6]={0};

    while(1) { /* infinite loop to be broken out of when maxlen reaches a certain condition */
        ura= (float)rand()/RAND_MAX;
        wt[0]= -1.0/ar*log(1+ura); 
        wt[1]= -1.0/ar*log(ura); 
        wt[2]= -log(ura)/ar;
        wt[3]= -1.0/ar2*log(1+ura); 
        wt[4]= -1.0/ar2*log(ura); 
        wt[5]= -log(ura)/ar2;
        printf("For ura=%4.4f, wts=", ura);
        for(j=0;j<6;++j) 
            printf("%.3f ", wt[j]);
        printf("\n"); 
        i++;
        if(i > 6)
            break;
    }
    return 0;
}
