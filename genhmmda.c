#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define SETSEED 5 /*  if -DUNPREDRA is not fed in */
#define DICENSIDES 6 /* number of sides to our dice */

int chanceit(int n, int *iarr)
{
    int i;
    float *arr=calloc(n, sizeof(float));

    arr[0]= (float)iarr[0];
    for(i=1;i<n;++i) 
        arr[i]= arr[i-1] + (float)iarr[i];
    for(i=0;i<n-1;++i) 
        arr[i] /= arr[n-1];

    /* this next step is entirely unnecessary, but it shows how the dice roll is being performed,
     * in that, the final - summation - figure is not required */
    arr=realloc(arr, (n-1)*sizeof(float)); /* lop off final value */
#ifdef DBG
    for(i=0;i<n-1;++i) 
        printf("%f ", arr[i]); 
    printf("\n"); 
#endif

    /* throw dice */
    float ura= 1. - (float)random()/(RAND_MAX+1L); /* because we don't want to have zero, ever, for the log function */
#ifdef DBG
    printf("rndnum was %f\n", ura); 
#endif

    for(i=0;i<n-1;++i) {
        if(ura>arr[i])
            continue;
        else
            break;
    }
    free(arr);
    return 1+i;
}

int main(int argc, char *argv[])
{
    if(argc!=4) {
        printf("Error. Pls supply 3 arguments 1) Length unknown seq 2) length training data 3) avg numb of state change per training length.\n");
        printf("Example usage: %s 200 20 2: average of two state changes is training data of length 20;\n", argv[0]);
        printf("This same ate used for unknown data of length 200\n");
        printf("Four files will be output: \"unkhmm.symb\", unkhmm.states\", \"trnhmm.symb\" and finally \"trnhmm.states\"");
        exit(EXIT_FAILURE);
    }
    int tsqlen=atoi(argv[1]);
    int sqlen=atoi(argv[2]);
    int rem=tsqlen%sqlen;
    if(rem) {
        printf("Sorry: one of the rules is that total unknown sequence length needs to be perfectly\n");
        printf("divisible by the training sequence length\n"); 
        exit(EXIT_FAILURE);
    }

#ifdef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

    float evplen=atof(argv[3]); /* events per length */
    float lam = evplen/sqlen;
    int i=0;
    float ura; /*  variable to hold one uniform random variable 0-1 */
    float wt /* waiting time */;
    float twt=.0;
    /* hard-coded data now follows: */
    char cc[2]={'A', 'B'};
    int die[2][DICENSIDES]={ {1,1,1,1,1,1}, {1,1,1,1,1,20} };
    /* end of hardcoded data */
    unsigned char c=0;

    /* first we're going to populate our training data files */
    FILE *ftsy/*fileptr train symbs*/=fopen("./trnhmm.symb", "w");
    FILE *ftsta/*fileptr train states*/=fopen("./trnhmm.states", "w");
    int tstacha=0;
    while(1) { /* infinite loop to be broken out of */
        ura= 1. - (float)random()/(RAND_MAX+1L); /* because we don't want to have zero, ever, for the log function */
        wt= -log(ura)/lam;
        twt += wt;
#ifdef DBG
        printf("Free up to %.2f: ", twt); 
#endif
        while((i<=(int)twt) & (i<sqlen)) {
            fputc(cc[0x01&c], ftsy);
            fputc(48+chanceit(DICENSIDES, die[0x01&c]), ftsta);
            i++;
        }
        if(twt >= sqlen)
            break;
        c++;
        tstacha++;
    }
    fputc('\n', ftsy);
    fputc('\n', ftsta);
    fclose(ftsy);
    fclose(ftsta);

    /* next we go for our unknown training dat files */
    FILE *fusy/*fileptr unkpown symbols*/=fopen("./unkhmm.symb", "w");
    FILE *fusta/*fileptr unknown states*/=fopen("./unkhmm.states", "w");
    c=0; /* yes, we start like we started the training sequence */
    i=0; /* yes, we start like we started the training sequence */
    twt=.0;
    /* change lam? No! the rate will not change! */
    int ustacha=0;
    while(1) { /* infinite loop to be broken out of */
        ura= 1. - (float)random()/(RAND_MAX+1L); /* because we don't want to have zero, ever, for the log function */
        wt= -log(ura)/lam;
        twt += wt;
#ifdef DBG
        printf("Free up to %.2f: ", twt); 
#endif
        while((i<=(int)twt) & (i<tsqlen)) {
            fputc(cc[0x01&c], fusy);
            fputc(48+chanceit(DICENSIDES, die[0x01&c]), fusta);
            i++;
        }
        if(twt >= tsqlen)
            break;
        ustacha++;
        c++;
    }
    fputc('\n', fusy);
    fputc('\n', fusta);
    fclose(fusy);
    fclose(fusta);

    printf("Program run report: %i states changes reg in training set; %i state changes in unknown set.\n", tstacha, ustacha); 
    return 0;
}
