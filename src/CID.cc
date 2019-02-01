/*CryptoID v1.02 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)

  This program is released under the GNU GPL License version 3.

  Due to libfxt.a being a C++ library, this program has to be compiled as
  C++ as well to handle name mangling properly.
*/
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <complex.h>

void slow_fract_ft(double *fr, double *fi, unsigned long n, double v);
//void fft_fract (double *fr, double *fi, unsigned long n, double v);
                            /*Warning: fast version produces errors!*/

/***
* insertionSort - sorts an array of double values. Based on the example
* provided at: http://linux.wku.edu/~lamonml/algor/sort/insertion.html.
*/

void insertionSort(double numbers[], long int array_size)
{
   long int i, j;
   double index;
   for (i=1; i < array_size; i++){
      index = numbers[i];
      j = i;
      while ((j > 0) && (numbers[j-1] > index))
      {
         numbers[j] = numbers[j-1];
         j = j - 1;
      }
      numbers[j] = index;
   }
}

int main(int argc, char **argv)
{
   double *x, *y, *z, median, encD, encP, rndD, rndP;
   FILE *fp;
   char * filename;
   unsigned char byte;
   struct stat fileStat;
   long int fsize, realFsize, loop, start, end;
   printf ("CryptoID v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)\n");
   if (argc==4){
      filename=argv[1];
      start=strtoul(argv[2],NULL,0);
      end=strtoul(argv[3],NULL,0);
   }else{
      printf("Syntax: %s <fname> <byte sequence start> <byte sequence end>\n",
            argv[0]);
      printf("(Note: file offsets start at 0, not 1)\n");
      exit (1);
   }
   fp=fopen(filename,"rb");
   if (fp){
      stat(filename,&fileStat);
      realFsize=fileStat.st_size;    /*Get real size of file.*/
      if (end>(realFsize-1)){
         end=realFsize-1;
         printf("Warning, end is larger than file size, setting end to "
                "real file size -1\n");
      }
      fsize=end-start+1;
      x = (double *) malloc(fsize*sizeof(double));
      if (!x){
         printf ("Error, malloc X\n"); exit (1);
      }
      y = (double *) malloc(fsize*sizeof(double));
      if (!y){
         printf ("Error, malloc Y\n"); exit (1);
      }
      z = (double *) malloc(fsize*sizeof(double));
      if (!y){
         printf ("Error, malloc Z\n"); exit (1);
      }
      fseek (fp,(long int)start,SEEK_SET);   /*Go to start of sequence.*/
      for (loop=0; loop<fsize; loop++){      /*Read byt sequence.*/
         byte=(unsigned char)fgetc(fp);
         x[loop]=(double)((unsigned int)(byte));
         y[loop]=0.0;
      }

      //fft_fract(x,y,fsize,0.5);    /*Warning: fast version produces errors!*/
      slow_fract_ft(x,y,fsize,0.5);  /*Compute the 45º Fractional Fourier
                                       Transform of byte sequence.*/
         /*Calculate the median of the 45º FrFT.*/
      for (loop=0; loop<fsize; loop++){ /*Modulus of each complex #.*/
         z[loop]=sqrt(x[loop]*x[loop]+y[loop]*y[loop]);
      }
      insertionSort(z, fsize);
      if ((fsize%2)==0){   /*Median for even sequence lengths...*/
         median=(z[fsize/2-1]+z[fsize/2])/2;
      }else{               /*...and for odd lengths.*/
         median=z[(fsize-1)/2];
      }
      printf("Median(Modulus(FrFt(Sequence,0.5))): %e\n",median);
      printf("Size of byte sequence:  %lu\n",fsize);
         /*Check if the sequence looks random.*/
      rndD= 2249.706     /*Calculate randomness degree.*/
            +2237.244*log(median)
            -4984.742*pow(median,(1.0/3.0))
            -33.214*sqrt(fsize)+1136.130*sqrt(median)
            +0.470*fsize-3.321*median;
      rndP=1/(1+exp(-rndD));  /*Calculate randomness probability.*/
      printf("Randomness degree:      %e\n",rndD);
      printf("Randomness probability: %e\n",rndP);
      if (rndP>0.5){
            /*Check if it is an encrypted/truly random sequence or not.*/
         encD= 415819.3     /*Calculate encryption degree.*/
               -693813*log(fsize)+28638.075*log(median)
               +1041967*pow(fsize,(1.0/3.0))-62965.2*pow(median,(1.0/3.0))
               -204806*sqrt(fsize)+14195.104*sqrt(median)
               +454.372*fsize-39.649*median;
         encP=1/(1+exp(-encD));  /*Calculate encryption probability.*/
            /*Print results.*/
         printf("Encryption degree:      %e\n",encD);
         printf("Encryption probability: %e\n",encP);
         if (encP<=0.5){
            printf("Conclusion: The byte sequence appears to be somewhat"
                   " random (i.e. it could be non-random compressed"
                   " data).\n");
         }else{
            printf("Conclusion: The byte sequence appears to be truly"
                   " random (i.e. it could be data encrypted with"
                   " a robust cryptographic algorithm).\n");
         }
      }else{
         printf("Conclusion: The byte sequence does not appear to"
                " be random.\n");
      }
      free(x);
      free(y);
      free(z);
   }else{
      printf("Error, cannot open file %s\n",filename);
      return(1);
   }
   fclose(fp);
   return(0);
}
