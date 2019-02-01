/*CryptoLocator v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)
  This program is released under the GNU GPL License version 3.
 */

#include "CL_Includes.h"
#define N 10
void CLFindRNDSubstr (long int start,long int stop, FILE *fp, CLMap *map,
                      int mode, int minByteSubstr);
void CLFreqTestOvl (long int start,long int stop, FILE *fp, CLMap *map,
                     int mode, int minByteSubstr);
/****
* printCLMap - print the results from a CLMap structure.
*/
void printCLMap (CLMap map){
   int loop;
   if (map.numSubstrings){
      for (loop=0;loop<map.numSubstrings;loop++){
         printf("RND substring candidate at [%ld-%ld], with p=%e\n",
            map.begin[loop],map.end[loop],map.p[loop]);
      }
   }
   printf("substrings= %d overflow flag= %d\n\n",map.numSubstrings,
   map.overflow);
}

int main(int argc, char **argv)
{
   FILE *fp;
   char * filename;
   /*char defaultFName[]="test.tst";*/
   CLMap stage;
   struct stat fileStat;
   int fsize;
   printf ("CryptoLocator v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)\n");
   if (argc!=2){
      /*filename=defaultFName;*/
      printf("Syntax: %s <filename>\n\n",argv[0]);
      exit(1);
   }
   else{
      filename=argv[1];
   }
   fp=fopen(filename,"rb");
   if (fp){
      stat(filename,&fileStat );
      fsize=fileStat.st_size;    /*Get size of file.*/
      printf("*** Stage 0 - Substring ID with Frequency Test from offset %ld to offset %ld\n",
             (long int)0,(long int)fsize);
      CLFindRNDSubstr (0, fsize, fp, &stage, 1, 13);
      printCLMap(stage);
      printf("*** Stage 1 - Frequency Test from offset %ld to offset %ld\n",
             (long int)0,(long int)fsize);
      CLFreqTestOvl (0, fsize, fp, &stage, 1, 13);
      printCLMap(stage);
      printf("*** Stage 2 - Runs Test from offset %ld to offset %ld\n",
            (long int)0,(long int)fsize);
      CLFreqTestOvl (0, fsize, fp, &stage, 2, 13);
      printCLMap(stage);
      printf("*** Stage 3 - Serial Test from offset %ld to offset %ld\n",
            (long int)0,(long int)fsize);
      CLFreqTestOvl (0, fsize, fp, &stage, 4, 32);
      printCLMap(stage);
      printf("*** Stage 4 - App. Entropy Test from offset %ld to offset %ld\n",
            (long int)0,(long int)fsize);
      CLFreqTestOvl (0, fsize, fp, &stage, 5, 32);
      printCLMap(stage);
      printf("*** Stage 5 - DFT Test from offset %ld to offset %ld\n",
            (long int)0,(long int)fsize);
      CLFreqTestOvl (0, fsize, fp, &stage, 3, 125);
      printCLMap(stage);
   }
   else{
      printf("Error, couldn't open file: %s\n",filename);
      return(1);
   }
   printf("Closing file: %s\n\n",filename);
   fclose(fp);
   return(0);
}

