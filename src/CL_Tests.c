/*CryptoLocator v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)
  This program is released under the GNU GPL License version 3.
 */

#include "CL_Includes.h"
int isIntegrated = 0;            /*Flag to check if test string integration has
                                   taken place (We only need to integrate once.
                                   */

/*****
1) CLFreqTestSubstr - The test performs a frequency monobit test
(as defined in SP800-22 by NIST) on the substring defined
by the parameters; it returns the probability that this substring
is random.
 *****/
double CLFreqTestSubstr (unsigned char * byteStr, int substrLen){
  unsigned char byte;
  int loop1, loop2;
  double sObs,n,s,p,q;
  n=substrLen*8.0;    /*# of bits in byte substring.*/
  s=0.0;              /*Process all bytes within substring*/
  for (loop1=0;loop1<substrLen;loop1++){
     byte=byteStr[loop1];
     for(loop2=0;loop2<8;loop2++){   /*Process bits within each byte.*/
        s+=(((int)(byte&0x80)>>7)*2-1); /*+1 if bit=1, -1 if bit=0.*/
        byte=byte<<1;                   /*Shift 1 bit to the left.*/
     }
  }
  sObs=fabs(s)/sqrt(n);     /*Calculate randomness probability (p),*/
  q=(sObs/sqrt(2.0));           /*as specified in SP800-22 (NIST).*/
  p=erfc(q);
  return (p);
}

/*****
2) CLRunstTestSubstr - The test performs a Runs test
(as defined in SP800-22 by NIST) on the substring defined
by the parameters; it returns the probability that this substring
is random.
 *****/
double CLRunsTestSubstr (unsigned char * byteStr, int substrLen){
   unsigned char byte;
   int r, isNotFirst, loop1, loop2;
   double vObs,pi,p,q,n;
   n=substrLen*8.0;    /*# of bits in byte substring.*/
   pi=0.0;
   vObs=0.0;
   r=0; isNotFirst=0;
                       /*Process all bytes within substring:*/
   for (loop1=0;loop1<substrLen;loop1++){
      byte=byteStr[loop1];
      for(loop2=0;loop2<8;loop2++){   /*Process bits within each byte.*/
         pi+=(int)((byte&0x80)>>7);        /*+1 if bit=1, +0 if bit=0. */
         r+=(int)((byte&0x80)>>7);    /*add the most significant bit*/
         if (isNotFirst){
            if ((r&0x01) == ((r&0x02)>>1)){  /*Only compare the lowest 2 bits*/
               vObs+=1.0;
            }
         }
         else{
            isNotFirst=1;
         }
         r=r<<1;
         byte=byte<<1;                /*Shift 1 bit to the left.*/
      }
   }
   vObs+=1.0;
   pi=pi/n;
   q=fabs(vObs-2*n*pi*(1-pi));
   q=q/(2*sqrt(2*n)*pi*(1-pi));
   p=erfc(q);                 /*Calculate randomness probability (p),*/
   return (p);                /*as specified in SP800-22 (NIST).*/
}

/*****
3) CLDFTTestSubstr - The test performs a Discrete Fourier Transform (Spectral)
test (as defined in SP800-22 by NIST) on the substring defined
by the parameters; it returns the probability that this substring
is random.
 *****/
double CLDFTTestSubstr (unsigned char * byteStr, int substrLen){
   unsigned char byte;
   int loop1, loop2, mIndex;
   double *M,*S;
   double T,n,p,d,q,N0,N1;
   fftw_plan plan;
   n=substrLen*8.0;    /*# of bits in byte substring.*/
   T=sqrt(3*n);
   N0=0.95*n/2;
   M= (double *) fftw_malloc(sizeof(double)*n);
   if (!M){ printf ("Error, fftw_malloc\n"); exit (1);}
   S= (double *) fftw_malloc(sizeof(double)*n);
   if (!S){ printf ("Error, fftw_malloc\n"); exit (1);}
   plan = fftw_plan_r2r_1d(n, S, M, FFTW_R2HC, FFTW_ESTIMATE);
   mIndex=0;             /*Process all bytes within substring*/
   for (loop1=0;loop1<substrLen;loop1++){
     byte=byteStr[loop1];
     for(loop2=0;loop2<8;loop2++){   /*Process bits within each byte.*/
        S[mIndex]=(double)(((int)(byte&0x80)>>7)*2-1);/*+1 if b=1, -1 if b=0.*/
        byte=byte<<1;                   /*Shift 1 bit to the left.*/
        mIndex++;
     }
   }
   fftw_execute(plan);
   N1=0.0;
   for (mIndex=1;mIndex<(n/2);mIndex++){
      if (sqrt(M[mIndex]*M[mIndex]+M[(int)(n-mIndex)]*M[(int)(n-mIndex)])<T){
         N1+=1.0;  /*if modulus is < T, add 1 to N1.*/
      }
   }
   d=(N1-N0)/(sqrt(n*0.95*0.05/2.0));
   q=fabs(d)/sqrt(2.0);
   p=erfc(q);
   fftw_destroy_plan(plan);
   fftw_free(M); fftw_free(S);
   return (p);
}

/*****
4) CLSerialTestSubstr - The test performs a Serial test with block m=2
(as defined in SP800-22 by NIST) on the substring defined
by the parameters; it returns the probability that this substring
is random.
 *****/
double CLSerialTestSubstr (unsigned char * byteStr, int substrLen){
   unsigned char byte;
   int r, isNotFirst, loop1, loop2;
   double v0=0.0,v1=0.0,v00=0.0,v01=0.0,v10=0.0,v11=0.0;
   double p1,p2,n,psi2,psi1,psi0=0.0;
   double deltaPsi, deltaPsiSqrd;
   n=substrLen*8.0;    /*# of bits in byte substring.*/
   r=0; isNotFirst=0;
                       /*Process all bytes within substring:*/
   for (loop1=0;loop1<substrLen;loop1++){
      byte=byteStr[loop1];
      for(loop2=0;loop2<8;loop2++){   /*Process bits within each byte.*/
         r+=(int)((byte&0x80)>>7);    /*add the most significant bit*/
         if (isNotFirst){
            if ((r&0x01)==1){ /*count m-1 block frequencies*/
               v0++;
            }
            else{
               v1++;
            }
            if ((r&0x3)==0){ /*count m block frequencies*/
               v00++;
            }else if((r&0x3)==1){
               v01++;
            }else if((r&0x3)==2){
               v10++;
            }else if((r&0x3)==3){
               v11++;
            }
         }
         else{
            isNotFirst=1;
            if ((r&0x01)==1){ /*count m-1 block frequencies*/
               v0++;
            }
            else{
               v1++;
            }
         }
         r=r<<1;
         byte=byte<<1;                /*Shift 1 bit to the left.*/
      }
   }
   r+=(int)(byteStr[0]>>7);    /*Add 1 bit padding for block m=2*/
   if ((r&0x3)==0){                /*Count m block frequencies w last bit.*/
      v00++;
   }else if((r&0x3)==1){
      v01++;
   }else if((r&0x3)==2){
      v10++;
   }else if((r&0x3)==3){
      v11++;
   }
   psi2=2*2/n*(v00*v00+v01*v01+v10*v10+v11*v11)-n;
   psi1=2/n*(v0*v0+v1*v1)-n;
   psi0=0.0;
   deltaPsi=psi2-psi1;
   deltaPsiSqrd=psi2-2*psi1+psi0;
   p1=igamc(1,deltaPsi/2.0);
   p2=igamc(0.5,deltaPsiSqrd/2.0);
   if (p1<p2){
      return (p1);
   } else {
      return (p2);
   }
}

/*****
5) CLApEnTestSubstr - The test performs an Approximate Entropy test with
block m=2 (as defined in SP800-22 by NIST) on the substring defined
by the parameters; it returns the probability that this substring
is random.
 *****/
double CLApEnTestSubstr (unsigned char * byteStr, int substrLen){
   unsigned char byte;
   int r, isNotFirst, loop1, loop2;
   double c00=0.0,c01=0.0,c10=0.0,c11=0.0;
   double c000=0.0,c001=0.0,c010=0.0,c011=0.0,c100=0.0,
          c101=0.0,c110=0.0,c111=0.0;
   double p,n,phi2,phi3,ApEn,x2Obs;
   n=substrLen*8.0;    /*# of bits in byte substring.*/
   r=0; isNotFirst=0;
                       /*Process all bytes within substring:*/
   for (loop1=0;loop1<substrLen;loop1++){
      byte=byteStr[loop1];
      for(loop2=0;loop2<8;loop2++){   /*Process bits within each byte.*/
         r+=(int)((byte&0x80)>>7);    /*add the most significant bit*/
         if (isNotFirst>=1){
            if ((r&0x3)==0){ /*count m block frequencies*/
               c00++;
            }else if((r&0x3)==1){
               c01++;
            }else if((r&0x3)==2){
               c10++;
            }else if((r&0x3)==3){
               c11++;
            }
            if (isNotFirst<2){
               isNotFirst=2;
            }
         }
         else{
            isNotFirst=1;
         }
         if (isNotFirst==2){        /*count m+1 block frequencies*/
            if ((r&0x7)==0){
               c000++;
            }else if((r&0x7)==1){
               c001++;
            }else if((r&0x7)==2){
               c010++;
            }else if((r&0x7)==3){
               c011++;
            }else if((r&0x7)==4){
               c100++;
            }else if((r&0x7)==5){
               c101++;
            }else if((r&0x7)==6){
               c110++;
            }else if((r&0x7)==7){
               c111++;
            }
         }
         r=r<<1;
         byte=byte<<1;                /*Shift 1 bit to the left.*/
      }
   }
   r+=(int)(byteStr[0]&0x80>>7);    /*Add 1 bit padding for block m=2.*/
   if ((r&0x3)==0){                /*Count m block frequencies w/ last bit.*/
      c00++;
   }else if((r&0x3)==1){
      c01++;
   }else if((r&0x3)==2){
      c10++;
   }else if((r&0x3)==3){
      c11++;
   }
   if ((r&0x7)==0){              /*Do the same for m+1*/
      c000++;
   }else if((r&0x7)==1){
      c001++;
   }else if((r&0x7)==2){
      c010++;
   }else if((r&0x7)==3){
      c011+=1.0;
   }else if((r&0x7)==4){
      c100++;
   }else if((r&0x7)==5){
      c101++;
   }else if((r&0x7)==6){
      c110++;
   }else if((r&0x7)==7){
      c111++;
   }
   r=r<<1;
   r+=(int)((byteStr[0]&0x40)>>6); /*Add 1 bit more padding for block m=2+1.*/
   if ((r&0x7)==0){                /*count the final block freq.*/
      c000++;
   }else if((r&0x7)==1){
      c001++;
   }else if((r&0x7)==2){
      c010++;
   }else if((r&0x7)==3){
      c011++;
   }else if((r&0x7)==4){
      c100++;
   }else if((r&0x7)==5){
      c101++;
   }else if((r&0x7)==6){
      c110++;
   }else if((r&0x7)==7){
      c111++;
   }
   c00/=n; c01/=n; c10/=n; c11/=n;
   c000/=n; c001/=n; c010/=n; c011/=n; c100/=n; c101/=n; c110/=n; c111/=n;
   phi2=c00*log(c00)+c01*log(c01)+c10*log(c10)+c11*log(c11);
   phi3=c000*log(c000)+c001*log(c001)+c010*log(c010)+c011*log(c011)+\
         c100*log(c100)+c101*log(c101)+c110*log(c110)+c111*log(c111);
   ApEn=phi2-phi3;
   x2Obs=2*n*(log(2.0)-ApEn);
   p=igamc(2,x2Obs/2.0);
   return (p);
}

/*****
CLFindRNDSubstr - Creates a Map of potentially random byte substrings by
executing the test defined by 'mode'.
 *****/
void CLFindRNDSubstr (long int start,long int stop, FILE *fp, CLMap *map,
                      int mode, int minByteSubstr){
   unsigned char *buffer;
   long int eps=0, bytesRead=0, rLength=0;
   int isRandom=0, err=0, isComplete=0, rIndex=0;
   double p=0.0;

   if ((stop-start)<minByteSubstr){
      printf ("Error, test block is too small"); exit(1);
   }
   err=fseek (fp,(long int)start,SEEK_SET);
   if (err) {
      printf ("Error, fseek\n"); exit(1);
   }
   buffer= (unsigned char *) malloc(stop-start);
   if (!buffer){
      printf ("Error, malloc\n"); exit (1);
   }
   bytesRead=fread(buffer, 1, (long int) stop-start, fp);
   if (!bytesRead) {
      printf ("Error, fread\n"); exit(1);
   }
   map->numSubstrings=0;
   map->overflow=0;
   while(!isComplete){    /*Process all substrings in the range [start,stop].*/
                          /*Process all bytes within substring:*/
      if (mode ==1){      /*Apply the frequency (monobit) test.*/
         p=CLFreqTestSubstr (&(buffer[eps]), minByteSubstr);
      }
      if (isRandom){            /*If already processing a random substring...*/
         if (p<ALPHA){          /*...and last substring IS NOT random.*/
            isRandom=0;
            map->end[rIndex]=map->begin[rIndex]+rLength;
            if (!map->overflow){
               rIndex++;
               if (rIndex==MAX_CRYPTO_SUBSTRINGS){
                  map->overflow=1;
               }
            }
            rLength=0;
         }
         else{ rLength++;}      /*...and last substring IS random.*/
      }
      else{                           /*Not processing a random substring...*/
         if ((p>=ALPHA)&&(!map->overflow)){ /*...and last substring IS random.*/
            isRandom=1;
            map->begin[rIndex]=eps;
            map->p[rIndex]=p;
            rLength=minByteSubstr;
            map->numSubstrings++;
         }
      }
      eps++;
      if ((bytesRead-eps)<minByteSubstr) {
         isComplete=1;
         if (isRandom){ /*If we reach end of string in a random state,
                          close the substring.*/
            map->end[rIndex]=map->begin[rIndex]+rLength;
         }
      }
   }
   free (buffer);
}

/*****
 CLSelectTest - Function Wrapper that executes the corresponding statistical
 test and returns the result.
 *****/
double CLSelectTest (unsigned char * byteStr, int substrLen, int mode){
   double p=0.0;
   if (mode==1){
      p=CLFreqTestSubstr (byteStr,substrLen);
   } else if (mode==2){
      p=CLRunsTestSubstr (byteStr,substrLen);
   } else if (mode==3){
      p=CLDFTTestSubstr (byteStr,substrLen);
   } else if (mode==4){
      p=CLSerialTestSubstr (byteStr,substrLen);
   } else if (mode==5){
      p=CLApEnTestSubstr (byteStr,substrLen);
   }
   else{
      printf("Error, Test #%d is not defined",mode);
   }
   return (p);
}

/*****
CLFreqTestOvl - Creates a Map of non overlapped substrings by
executing the Frequency test. It uses as a base the CLMap provided as input (
which should contain already the results of running CLFreqTestSubstr on the
whole file segment being analysed). It returns the modified CLmap to the caller.
 *****/
void CLFreqTestOvl (long int start,long int stop, FILE *fp, CLMap *map,
                     int mode, int minByteSubstr){
   unsigned char *buffer;
   long int bytesRead=0, begin, end;
   int isOvl=0, err=0, oIndex=0, nIndex=0;
   double p;
   CLMap newMap, newMapTested;
                  /*First, load the whole file in memory:*/
   if ((stop-start)<minByteSubstr){
      printf ("Error, test block is too small"); exit(1);
   }
   err=fseek (fp,(long int)start,SEEK_SET);
   if (err) {
      printf ("Error, fseek\n"); exit(1);
   }
   buffer= (unsigned char *) malloc(stop-start+minByteSubstr+1);
   if (!buffer){
      printf ("Error, malloc\n"); exit (1);
   }
   bytesRead=fread(buffer, 1, (long int) stop-start, fp);
   if (!bytesRead) {
      printf ("Error, fread\n"); exit(1);
   }
   if (!isIntegrated){
      newMap.numSubstrings=0;
      newMap.overflow=0;
                  /*Process all substrings detected in map:*/
      while (oIndex<(map->numSubstrings)){
         if (isOvl){
            newMap.end[nIndex]=map->end[oIndex];
         }
         else{
            newMap.begin[nIndex]=map->begin[oIndex];
            newMap.end[nIndex]=map->end[oIndex];
            newMap.numSubstrings++;
         }
            /*Consider overlaps and contiguous strings as one.*/
         if ((oIndex+1 < map->numSubstrings)&&
             (map->begin[oIndex+1] <= (map->end[oIndex]+1))){
            isOvl=1;
         }
         else{
            isOvl=0;
            nIndex++;
         }
         oIndex++;
      }
      isIntegrated=1;
   }
   else{
      newMap=*map;
   }

   /*Try again the algorithm with a minimum string length for better
    edge detection:*/
   for (oIndex=0;oIndex<newMap.numSubstrings;oIndex++){
      if ((newMap.end[oIndex]-newMap.begin[oIndex]+1)>minByteSubstr){
         begin=newMap.begin[oIndex];
         do {     /*refine lower edge*/
            p=CLSelectTest(&(buffer[begin]),minByteSubstr,mode);
            begin++;
         } while ((p<ALPHA)&&(begin<newMap.end[oIndex]));
         newMap.begin[oIndex]=begin-1;
         end=newMap.end[oIndex];
         do {     /*refine upper edge*/
            p=CLSelectTest(&(buffer[end-minByteSubstr+1]),minByteSubstr,
                           mode);
            end--;
         } while ((p<ALPHA)&&(end>newMap.begin[oIndex]));
         newMap.end[oIndex]=end+1;
      }
   }
   /*Test again all integrated strings with test str. length= integrated string
    length and produce a new map with those that pass the test:*/
   newMapTested.numSubstrings=0;
   newMapTested.overflow=0;
   nIndex=0;
   for (oIndex=0;oIndex<newMap.numSubstrings;oIndex++){
      if ((newMap.end[oIndex]-newMap.begin[oIndex]+1)<minByteSubstr){
         printf("Warning: string at [%ld-%ld] discarded (< min. test string)\n",
                newMap.begin[oIndex],newMap.end[oIndex]);
      }
      else{
         p=CLSelectTest(&(buffer[newMap.begin[oIndex]]),
                        newMap.end[oIndex]-newMap.begin[oIndex]+1,mode);
         if (p>=ALPHA){
            newMapTested.numSubstrings++;
            newMapTested.begin[nIndex]=newMap.begin[oIndex];
            newMapTested.end[nIndex]=newMap.end[oIndex];
            newMapTested.p[nIndex]=p;
            nIndex++;
         }
         else{
            printf("Warning: string at [%ld-%ld] discarded "
                   "(p=%e)\n",
                   newMap.begin[oIndex],newMap.end[oIndex],p);
         }
      }
   }
   *map=newMapTested;
   free (buffer);
}
