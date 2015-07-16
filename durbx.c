/* durbx.c: code for the Durbin textbook example:
 * Beware alot of stuff is hard coded here, the point is to emulate
 * the example in simple C. The is a variation in this particular module
 * where I multiply transition matrices several times over */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMES 565
#define DIM1 4
#define DIM2 4
#define MATSZ (DIM1*DIM2)
#define EP .0000001

void chkrows(float *m)
{
    int i, j;
    float su;
    for(i=0;i<DIM1;++i) {
        su=.0;
        for(j=0;j<DIM2;++j) 
            su += m[i*DIM1+j];
        if( (su-1.) > EP)
            printf("dodgy mat at row %i! Total is %.6f\n", i, su); 
    }
}

void prttm(float *m, char *s)
{
    int i, j;
    printf("fr\\to:%5c %8c %8c %8c\n", s[0], s[1], s[2], s[3]);
    for(i=0;i<DIM1;++i) {
        for(j=0;j<DIM2;++j) 
            (j!=0)? printf("%.6f ", m[i*DIM1+j]) : printf("%2c:  %.6f ", s[i], m[i*DIM1+j]);
        printf("\n"); 
    }
}

void mm_us(float *m1, float *m2, float *mout)
{
    int i, j, k;
    float su;
    for(i=0;i<DIM1;++i)
        for(j=0;j<DIM2;++j) {
            su=.0;
            for(k=0;k<DIM1;++k)
                su += m1[i*DIM1+k] * m2[k*DIM1+j];
            mout[i*DIM1+j]=su;
        }
}

void mmltg(float *m, float *tmm, int times)
{
    int i;
    float *t2mm=malloc(MATSZ*sizeof(float));
    /* first mult */
    mm_us(m, m, tmm);

    for(i=1;i<times;++i) {
    mm_us(m, tmm, t2mm);
    memcpy(tmm, t2mm, MATSZ*sizeof(float));
    }
    free(t2mm);
}

int main(int argc, char *argv[])
{
    char s[DIM2]={'A', 'C', 'G', 'T'};
    /* THe original plus "yes,island" is this has .01 too much on the cytosine row */
    // float plus[MATSZ]= {.18, .274, .426, .12, .171, .368, .274, .188, .161, .339, .375, .125, .079, .355, .384, .182};
    float plus[MATSZ]= {.18, .274, .426, .12, .17025, .36725, .27325, .18725, .161, .339, .375, .125, .079, .355, .384, .182};
    float minus[MATSZ]= {.3, .205, .285, .21, .322, .298, .078, .302, .248, .246, .298, .208, .177, .239, .292, .292};
    chkrows(plus);
    float *tmm=malloc(MATSZ*sizeof(float));

    printf("Transition matrix for bases within an island:\n"); 
    prttm(plus, s);

    printf("Transition matrix for bases OUTSIDE of a CpG island:\n"); 
    prttm(minus, s);


    int times=TIMES;
    mmltg(plus, tmm, times);
    printf("plus transition matrix multiplied %i times:\n", times);
    prttm(tmm, s);

    mmltg(minus, tmm, times);
    printf("minus transition matrix multiplied %i times:\n", times);
    prttm(tmm, s);

    free(tmm);
    return 0;
}
