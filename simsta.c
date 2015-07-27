/*  Returning to this after a mont or two, it was incredibly hard to work out
 *  Let's see: a certain site undergoes changes of base. Each change can be seen as a jump, and we don't know how many
 *  jumps there will per site. Normally the number of positions coming from the jumps would be one more than 
 *  the number of jumps, but in this cas,e the final position is actually known, because cute off the sites development
 *  after a fixed time. This means that the number of positions and the number of changes (i.e. bases adopted) is the same */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <cairo/cairo.h>

#define NUMSITES 40 /*  number of sites running*/
#define NSTATES 4 /*  number of states our sites can be in: 4 for DN, 20 for AA, etc. */
#define LENC 10.0 /* length of course */
#define BUF 32

#define SETSEED 5 /*  if -DUNPREDRA is not fed in */

/*  OK; now we need cairo defines */
#define NAMBUF 32 /*  colour name length */
#define WID 640 /*  width of canvas */
#define LMAR 32 /* left margin */
#define RMAR LMAR /* right margin */
#define HEI 480 /* height of canvas */
#define HBARNUMS (NUMSITES+1) /* our horizontal bars is when the sites' tracks will be */
#define STRBUF 32 /* buffer for strings */

/* only global */
const float TFONTSZ=16.0; /* Title font size */
const float FONTSZ=12.0; /* the size (number of colours) that our colorscheme has */

typedef enum /*  simple enum to hold our states */
{
    A,
    B,
} sta;

typedef struct /*  struct for holding our colours, we'll be having an array of these. */
{
    char nam[NAMBUF];
    float rgb[3];
} colstru_f;

typedef struct /* our definition of a site is a struct */
{
    int sitenid; /* id number of the site */
    int currp; /* curr num of jumps: also uses as index to posarr[] */
    int jbf; /* buffer allow for jumps */
    float mxdisp; /* current maximum displacement */
    float *posarr; /* the array of position values: to be a cumulative */
    base *brec; /* the array of past and current bases: one less than total of posarr */
    base latestb; /* the next base */
    char starsymb; /* the starting symbol (character) */
    char endsymb; /* the end symb */
} sitedef;/* our definition of a site is a struct */ 

int *memind(int n) /* return an n x n-1 matrix of indices which exclude the diags */
{
    /* build our excluding-diag-index array first */
    int i, j, k, m, l=0;
    int *excind = calloc(n*(n-1), sizeof(int));

    for(i=0;i<n;++i)
        for(j=0;j<n;++j) { 
            k=n*i+j;
            m=k%n;
            if(k!=i*n+i)
                excind[l++] = m; 
        } /* excind is built */
    return excind;
}

base getnextrbase(float *nsf, int n, int *excind, base sta) /* get our next random base */
{
    int i;
    base retbase;

    float r= (float)rand()/RAND_MAX * nsf[n-1];

    for(i=0;i<n-1;++i)
        if(r<nsf[i+1]) { /* shouldn't this be if(r<nsf[sta*(n-1)+i+1])? */
            retbase = (base) excind[sta*(n-1) + i]; 
            break;
        }
    return retbase;
}

float *mat2cum(float *arr, int n, int *excind)
{
    int i, j; /* note cumulative array will calculate second values onward */

    float *cumarr;
    cumarr=calloc(n*n, sizeof(float)); 
    for(i=0;i<n;++i)
        for(j=1;j<n;++j)
            cumarr[n*i+j] = cumarr[n*i+j-1] + (arr[i*n+excind[i*(n-1) +j-1]] / -arr[i*n + i]);

    return cumarr;
}

void sitesubproc(sitedef* sites, char symb) /* the function for the site substitution process */
{
    int i;
#ifdef UNPREDRA
    struct timeval tnow;
    gettimeofday(&tnow, NULL);
    /*  use the five last digits of the current microsecond reading to generate a seed */
    unsigned int tseed=(unsigned int)((tnow.tv_usec/100000.0 - tnow.tv_usec/100000)*RAND_MAX);
    srand(tseed);
#else
    srand(SETSEED);
#endif

    /* what's the starting symbol? */
    for(i=0;i<NUMSITES;++i) {
        switch (symb) {
            case 'A':
                sites[i].latestb=A; /*  */
                sites[i].starsymb='A'; /*  */
                break;
            case 'C':
                sites[i].latestb=C; /*  */
                sites[i].starsymb='C'; /*  */
                break;
            case 'G':
                sites[i].latestb=G; /*  */
                sites[i].starsymb='G'; /*  */
                break;
            case 'T':
                sites[i].latestb=T; /*  */
                sites[i].starsymb='T'; /*  */
                break;
        }
    }

    float ar[16] = {-0.886, 0.190, 0.633, 0.063, 0.253, -0.696, 0.127, 0.316, 1.266, 0.190, -1.519, 0.063, 0.253, 0.949, 0.127, -1.329}; /*  this is a given: substitution matrix */

    int *excind=memind(4); /* a matrix of indices will be 4 rows by 3 columns */
    float *nsf = mat2cum(ar,4, excind); /* an array to hold the off diagonal entries, divided by the diagonal entry, all made negative */

    float ura; /*  variable to hold one uniform random variable 0-1 */
    base currba;
    for(i=0;i<NUMSITES;++i) {
        sites[i].latestb = sites[i].brec[0] = G;
        while(1) { /* infinite loop to be broken out of when maxlen reaches a certain condition */
            ura= (float)rand()/RAND_MAX;
            currba = sites[i].latestb;
            //            printf("currba: %u\n",currba);
            /*  armed with our ura, we can generate the Exp() */
            sites[i].posarr[sites[i].currp + 1] = sites[i].posarr[sites[i].currp] + (-1.0/ar[4*currba+currba])*log(1+ura); 
            sites[i].brec[sites[i].currp + 1] = getnextrbase(nsf + 4*currba, 4, excind, currba);

            sites[i].latestb = sites[i].brec[sites[i].currp + 1];
            sites[i].mxdisp = sites[i].posarr[sites[i].currp + 1];

            sites[i].currp++;
            /* check posarr buf sz */
            if(sites[i].currp==sites[i].jbf) {
                sites[i].jbf += BUF;
                sites[i].posarr=realloc(sites[i].posarr, sites[i].jbf * sizeof(float));
                sites[i].brec=realloc(sites[i].brec, sites[i].jbf * sizeof(base));
            }
            /*  breaking out when condition met */
            if(sites[i].mxdisp >= LENC)
                break; /*  this site has now crossed finishing line, go to next site*/
        }
        /*  rationalise posarr array size here */
        sites[i].posarr=realloc(sites[i].posarr, sites[i].currp * sizeof(float));
        sites[i].brec=realloc(sites[i].brec, sites[i].currp * sizeof(base));
        /*  FInd out at what symbol site finished at */
        switch(sites[i].brec[sites[i].currp-1]) {
            case 0:
                sites[i].endsymb='A'; break;
            case 1:
                sites[i].endsymb='C'; break;
            case 2:
                sites[i].endsymb='G'; break;
            case 3:
                sites[i].endsymb='T'; break;
        }
    }
    free(nsf);
    free(excind);
}

int main(int argc, char *argv[])
{
    int i, j;
    sitedef* sitearr; /*  declare */
    /* now, initialise */
    sitearr=malloc(sizeof(sitedef) * NUMSITES);
    for(i=0;i<NUMSITES;++i) {
        sitearr[i].mxdisp=0.0;
        sitearr[i].currp=0;
        sitearr[i].jbf=BUF;
        sitearr[i].posarr=calloc(sizeof(float), sitearr[i].jbf);
        sitearr[i].sitenid=i+1;
        sitearr[i].brec=calloc(sizeof(base), sitearr[i].jbf);
        /* latestb, starsymb and endsymb will all be set by the subroutine */
    }

    /*  OK, the race takes place here: */
    sitesubproc(sitearr, 'G');


    /*  Now we can around to visualising how it was run */
    /* We want two colours for the alternating jumps of each site (from the rectnam program) but we create an array of different colours so we can choose, to get a nice scheme */
    colstru_f colsf[21] = { {"foreground", {0.866667, 0.866667, 0.866667} }, {"background", {0.066667, 0.066667, 0.066667} }, {"color0", {0.098039, 0.098039, 0.098039} }, {"color8", {0.627451, 0.321569, 0.176471} }, {"color1", {0.501961, 0.196078, 0.196078} }, {"color9", {0.596078, 0.168627, 0.168627} }, {"color2", {0.356863, 0.462745, 0.184314} }, {"color10", {0.537255, 0.721569, 0.247059} }, {"color3", {0.666667, 0.600000, 0.262745} }, {"color11", {0.937255, 0.937255, 0.376471} }, {"color4", {0.196078, 0.298039, 0.501961} }, {"color12", {0.168627, 0.309804, 0.596078} }, {"color5", {0.439216, 0.423529, 0.603922} }, {"color13", {0.509804, 0.415686, 0.694118} }, {"color6", {0.572549, 0.694118, 0.619608} }, {"color14", {0.631373, 0.803922, 0.803922} }, {"color7", {1.000000, 1.000000, 1.000000} }, {"color15", {0.866667, 0.866667, 0.866667} }, {"indyell", {0.945098, 0.596078, 0.000000} }, {"italgreen", {0.000000, 0.337255, 0.301961} }, {"strongred", {0.776471, 0.000000, 0.137255} } };

    /*  get our canvas set up with background colour */
    cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WID, HEI);
    cairo_t *cr = cairo_create (surface);
    cairo_rectangle (cr, 0, 0, WID, HEI); 
    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_fill (cr);
    /*  we also want some text on the image */
    cairo_select_font_face (cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_font_extents_t fe;
    cairo_text_extents_t te;

    /*  OK, position up the sites' tracks. The HBARNUMS+1, the plus oen is for the title bar */
    int *hbars;
    hbars=calloc(HBARNUMS, sizeof(int)); /*  will hold midpoint of each bar */
    hbars[0] = HEI/HBARNUMS/2;
    for(i=1;i<HBARNUMS;++i)
        hbars[i] = hbars[i-1] + HEI/HBARNUMS;

    /* title */
    cairo_set_font_size (cr, TFONTSZ);
    char tistr[]= "DNA site substitutions over time";
    cairo_text_extents (cr, tistr, &te);
    cairo_move_to (cr, WID/2 - te.x_bearing - te.width/2, hbars[0] +8 - fe.descent + fe.height/2);
    cairo_set_source_rgb(cr, 0.5, 0.6, .782);
    cairo_show_text (cr, tistr);
    /*  and finally reset font size */
    cairo_set_font_size (cr, FONTSZ);

    /* OK, now we're going to paint up the position vectors */
    int canvfr, canvto;
    cairo_set_line_width (cr, 3);
    char *somestr;
    somestr=calloc(sizeof(char), STRBUF);
    for(i=0;i<NUMSITES-1;++i) {
        for(j=1;j<=sitearr[i].currp;++j) {
            canvfr= LMAR + (int)(WID-RMAR-LMAR)*(sitearr[i].posarr[j-1]/LENC);
            if(j != sitearr[i].currp)
                canvto=LMAR + (int)(WID-RMAR-LMAR)*(sitearr[i].posarr[j]/LENC);
            else  /* final position is cut to the "end of the line" */
                canvto=WID-RMAR; 

            switch(sitearr[i].brec[j-1]) {
                case 0:
                    cairo_set_source_rgba(cr, colsf[18].rgb[0], colsf[18].rgb[1], colsf[18].rgb[2], 0.95);
                    break;
                case 1:
                    cairo_set_source_rgba(cr, colsf[6].rgb[0], colsf[6].rgb[1], colsf[6].rgb[2], 0.95);
                    break;
                case 2:
                    cairo_set_source_rgba(cr, colsf[20].rgb[0], colsf[20].rgb[1], colsf[20].rgb[2], 0.95);
                    break;
                case 3:
                    cairo_set_source_rgba(cr, colsf[11].rgb[0], colsf[11].rgb[1], colsf[11].rgb[2], 0.95);
                    break;
            }
            cairo_move_to(cr, canvfr ,hbars[i+1]);
            cairo_line_to(cr, canvto ,hbars[i+1]);
            cairo_stroke (cr);
        }
        /* Now stick in the startin symbol at beginning */
        sprintf(somestr, "%c", sitearr[i].starsymb);
        cairo_text_extents (cr, somestr, &te);
        cairo_move_to (cr, LMAR - 14 - te.x_bearing, hbars[i+1] +4 - fe.descent + fe.height/2);
        cairo_set_source_rgb(cr, 0.9, 0.8, .7);
        cairo_show_text (cr, somestr);
        /* Now stick in the end symbol at beginning */
        sprintf(somestr, "%c", sitearr[i].endsymb);
        cairo_text_extents (cr, somestr, &te);
        cairo_move_to (cr, WID-RMAR + 8 - te.x_bearing, hbars[i+1] +4 - fe.descent + fe.height/2);
        cairo_set_source_rgb(cr, 0.9, 0.8, .7);
        cairo_show_text (cr, somestr);
    }

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "dric6a.png");
    cairo_surface_destroy (surface);

    int *nucrec=calloc(sizeof(int), NSTATES);
    for(i=0;i<NUMSITES;++i)
        switch(sitearr[i].endsymb) {
            case 'A':
                nucrec[0]++; break;
            case 'C':
                nucrec[1]++; break;
            case 'G':
                nucrec[2]++; break;
            case 'T':
                nucrec[3]++; break;
        }

    printf("Final Distribution:\n"); 
    for(i=0;i<NSTATES;++i) 
        printf("%6i ",nucrec[i]); 
    printf("\n"); 
    for(i=0;i<NSTATES;++i) 
        printf("%3.4f ",(float)nucrec[i]/NUMSITES); 
    printf("\n"); 

    for(i=0;i<NUMSITES;++i) {
        free(sitearr[i].posarr);
        free(sitearr[i].brec); /*  actually had forgotten this */
    }
    free(somestr);
    free(sitearr);
    free(nucrec);
    return 0;
}
