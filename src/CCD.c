/*CryptoCodeDetector v1.02 - (c) 2008 Omar A. Herrera Reyna
 (oherrera@prodigy.net.mx)
 This program is released under the GNU GPL License version 3.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "distorm.h"

#define DISAS_SCOPE 32     /*Size of dissassembled code for references -1.*/
#define BUF_SIZE 16
#define MAX_SECTIONS 50
#define MAX_REFS 100
#define MAX_INSTRUCTIONS 100000  /*Max. # of instructions to disassemble.*/

typedef struct {         /*Structure to save executable code info.*/
   unsigned long secAddr[MAX_SECTIONS]; /*RVA of section.*/
   unsigned long secOffs[MAX_SECTIONS]; /*Offset in file of the section.*/
   unsigned long secSize[MAX_SECTIONS]; /*Size of section.*/
   unsigned char secName[MAX_SECTIONS][9]; /*Name of section (8chars)*/
   unsigned long entryAddr;   /*RVA of entry point.*/
   unsigned long entryOffs;   /*Offset in file of the entry code.*/
   unsigned long imageBase;   /*Base of image for RVAs.*/
   unsigned int secNum;       /*Number of sections.*/
   unsigned int entrySec;
} EntryData;

/******
* getEntryData - Parses executable file headers and locates entry data for
* code and the locations, and sizes of sections. Information is returned in
* a structure of type EntryData.
*/

EntryData getEntryData (FILE * fp, char * filename){
   EntryData ed;
   unsigned char ulongBuf[4];
   unsigned char section[40];
   unsigned long miscL, addrOH;
   unsigned short sizeOH, miscS, secNum;
   int loop,addIBase;

   ed.secNum=0;
   ed.entryAddr=0;
   fseek(fp,0,SEEK_SET);
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading file header\n"); exit (1);
   }
   miscS=*((unsigned short *)ulongBuf);
   if (miscS != 0x5A4D){ /*No executable file signature*/
      printf("Error, file %s does not contain an EXE header\n",
            filename); exit (1);
   }
   fseek(fp,0x3C,SEEK_SET);
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading data from EXE header\n"); exit (1);
   }
   miscL=*((unsigned long *)ulongBuf); /*Get Addr. for PE Hdr.*/
   fseek(fp,miscL,SEEK_SET); /*Go to PE Hdr*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading PE header\n"); exit (1);
   }
   miscL=*((unsigned long *)ulongBuf);
   if (miscL != 0x4550){ /*No PE Header signature*/
      printf("Error, file %s is not a valid PE executable\n",
            filename); exit (1);
   }
   fseek(fp,0x02,SEEK_CUR); /*Go to # of sections.*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading # of Sections\n"); exit (1);
   }     /*Reading 4 bytes leaves us at the beginning of the Optional Hdr.*/
   secNum=*((unsigned short *)ulongBuf); /*We only use 2 for the sections.*/
   fseek(fp,0x0A,SEEK_CUR); /*Go to Size of Optional Hdr.*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading size of Optional Hdr.\n"); exit (1);
   }     /*Reading 4 bytes leaves us at the beginning of the Optional Hdr.*/
   addrOH=ftell(fp); /*Save address of Optional Hdr.*/
   sizeOH=*((unsigned short *)ulongBuf); /*We only use 2 for the size.*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading PE ID\n"); exit (1);
   }
   miscS=*((unsigned short *)ulongBuf); /*We only use 2 bytes for the ID.*/
   if (miscS == 0x010B){
      printf ("* PE32 Header found\n");
      addIBase=8;
   }else if (miscS == 0x020B){
      printf ("* PE32+ Header found\n");
      addIBase=4;
   }else {
      printf ("Error, unknown PE format: %04X\n",miscS); exit(1);
   }
   fseek(fp,0x0C,SEEK_CUR); /*Go to Entry Point RV Addr.*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading Entry point RVA\n"); exit (1);
   }
   miscL=*((unsigned long *)ulongBuf);
   ed.entryAddr=miscL;
   printf ("* Code Entry Point RVA: %lu (0x%08X)\n",miscL,(uint32_t)miscL);
   fseek(fp,addIBase,SEEK_CUR); /*Go to Image Base.*/
   if (fread(ulongBuf,sizeof(char),4,fp)!=4){
      printf("Error reading Base of Image\n"); exit (1);
   }
   miscL=*((unsigned long *)ulongBuf);
   ed.imageBase=miscL;

   fseek(fp,addrOH+sizeOH,SEEK_SET); /*Go to Sections Hdr.*/
   for (loop=0; loop<secNum;loop++){ /*Parse all sections.*/
      if (fread(section,sizeof(char),40,fp)!=40){
         printf("Error reading Sections\n"); exit (1);
      }
      strncpy((char *)ed.secName[loop],(char *)section,8);
      ed.secName[loop][8]='\0';        /*Copy name of section*/
      ed.secAddr[loop]=*((unsigned long *)&section[12]);
      ed.secSize[loop]=*((unsigned long *)&section[16]);
      ed.secOffs[loop]=*((unsigned long *)&section[20]);
   }
   ed.secNum=secNum;
   loop=0;        /*Locate section of Entry Point:*/
   while (!((ed.entryAddr>=ed.secAddr[loop])&&
            (ed.entryAddr<(ed.secAddr[loop]+ed.secSize[loop])))&&
            loop<secNum){
      loop++;
   }
   if (loop==secNum){
      printf ("Error, Entry Point is outside any valid section\n"); exit (1);
   }
   ed.entrySec=loop;
   ed.entryOffs=ed.entryAddr-ed.secAddr[loop]+ed.secOffs[loop];
   printf ("* Offset of Entry Point in file: %lu (0x%08X)\n",ed.entryOffs,
            (uint32_t)ed.entryOffs);
   printf ("* Entry point is located in section: %s\n",ed.secName[loop]);
   return (ed);
}

/*****
* printInstruction - Prints a dissassembled instruction
*/
void printInstruction (_DecodedInst decInst, EntryData fed,
                       unsigned long disIndex){
   printf ("   0x%08X|0x%08X:  %-20s %s %s\n",
               (uint32_t)(disIndex+fed.secOffs[fed.entrySec]),
               (uint32_t)(disIndex+fed.secAddr[fed.entrySec]+fed.imageBase),
               decInst.instructionHex.p, decInst.mnemonic.p,
               decInst.operands.p);
}

int main (int argc, char **argv){
   unsigned char *buf;
   unsigned long watchdog,watchdogEnd,watchdogIB,disIndex,miscL;
   long int wdSize;
   FILE *fp;
   char *filename, *strPtr;
   _DecodeResult decRes;
   _DecodedInst   *decInstBefore,*decInstAfter;
   EntryData fed;
   long int loop,loop2,iPtr,iPtr2,watchdogSec,adjust;
   unsigned int usedInstBefore,usedInstAfter,instToDisasm;
   decInstBefore = (_DecodedInst *) malloc(sizeof(_DecodedInst)*
                   MAX_INSTRUCTIONS);
   decInstAfter = (_DecodedInst *) malloc(sizeof(_DecodedInst)*
                  MAX_INSTRUCTIONS);
   printf("CryptoCodeDetector v1.0-(c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)\n");
   if(decInstBefore==NULL||decInstAfter==NULL){
      printf("Error in memory allocation\n"); exit (1);
   }
   if (argc==4){
      filename=argv[1];
      watchdog=strtoul(argv[2],NULL,0);
      watchdogEnd=strtoul(argv[3],NULL,0);
   }else{
      printf("Syntax %s <fname> <watchdog start offset> <watchdog end offset>\n",argv[0]);
      printf("(Note: Offsets start at position 0, not 1)\n");
      exit (1);
   }
   wdSize= watchdogEnd-watchdog;
   if (wdSize<=0){
      printf("Error, watchdog end <= watchdog start\n"); exit (1);
   }
   fp=fopen(filename,"rb");
   if (fp){
      fed = getEntryData(fp, filename);
      loop=0;        /*Locate section of watchdog:*/
      while (!((watchdog>=fed.secOffs[loop])&&
               (watchdog<(fed.secOffs[loop]+fed.secSize[loop])))&&
               loop<fed.secNum){
         loop++;
      }
      if (loop==fed.secNum){
         printf ("Error, watchdog start is outside any valid section\n");
         exit (1);
      }
      watchdogSec=loop;
      printf ("* Location (offset) of Watchdog in file: [%lu,%lu] "
              "([0x%08X,0x%08X])\n",watchdog,watchdog+wdSize,(uint32_t)watchdog,
              (uint32_t)(watchdog+wdSize));
      printf ("* Watchdog start is located in section: %s\n",fed.secName[loop]);
      if ((long int)((fed.secOffs[loop]+fed.secSize[loop])-watchdog)<125){
         printf ("    Warning, watchdog start is too close to end "
                 "of section!\n    Use the start offset of next section "
                 "instead.\n");
      }
      printf ("* Base of Image for RVAs (iBase) is: 0x%08X\n",
             (uint32_t)fed.imageBase);
      watchdogIB=watchdog-fed.secOffs[loop]+fed.secAddr[loop]+fed.imageBase;
      printf ("* Watchdog start RVA + Image Base: 0x%08X\n",(uint32_t)watchdogIB);
      printf ("* %d sections detected:\n",fed.secNum);
      for (loop=0;loop<fed.secNum;loop++){
         if (fed.secSize[loop]==0){ /*Adjust offset for Size=0.*/
            adjust=0;
         } else {
            adjust=1;
         }
         printf ("    Section: %9s  at [%lu,%lu] ([0x%08X,0x%08X])\n",
               fed.secName[loop],
	       fed.secOffs[loop],
               fed.secOffs[loop]+fed.secSize[loop]-adjust,
               (uint32_t)fed.secOffs[loop],
	       (uint32_t)(fed.secOffs[loop]+fed.secSize[loop]-adjust));
      }
      buf = (unsigned char *) malloc(fed.secSize[fed.entrySec]);
      fseek(fp,fed.secOffs[fed.entrySec],SEEK_SET);
      if (fread(buf,sizeof(char),fed.secSize[fed.entrySec],
          fp)!=fed.secSize[fed.entrySec]){
         printf("Error reading code section %s\n",
         fed.secName[fed.entrySec]); exit (1);
      }
      disIndex=fed.entryAddr - fed.secAddr[fed.entrySec];
/*    printf("* Function disassembly:\n");
      printf("*   fileOffs |iBase+RVA : <opcode> [param.] "
             "   <mnemonic> [operands]\n"); */
         /*To see the full disassembly uncomment the block above.*/
      if (disIndex>0){
               /*Disassemble instructions before Entry Point:*/
         instToDisasm=disIndex;
         decRes= distorm_decode(fed.secAddr[fed.entrySec]+fed.imageBase,
               &buf[0],instToDisasm,Decode32Bits,decInstBefore,
               MAX_INSTRUCTIONS,&usedInstBefore);
         iPtr=0;
/*       for (loop=0;loop<usedInstBefore;loop++){
            printInstruction (decInstBefore[loop], fed, iPtr);
            iPtr+=decInstBefore[loop].size;
         }  */ /*To see the full disassembly uncomment this block.*/
      } else {
         usedInstBefore=0;
      }
         /*Disassemble instructions after Entry Point:*/
      instToDisasm=fed.secAddr[fed.entrySec]+fed.secSize[fed.entrySec]
                  -fed.entryAddr;
      decRes= distorm_decode(fed.secAddr[fed.entrySec]+fed.imageBase
            +disIndex,&buf[disIndex],instToDisasm,Decode32Bits,decInstAfter,
            MAX_INSTRUCTIONS,&usedInstAfter);
      iPtr=disIndex;
/*    for (loop=0;loop<usedInstAfter;loop++){
         printInstruction (decInstAfter[loop], fed, iPtr);
         iPtr+=decInstAfter[loop].size;
      } */ /*To see the full disassembly uncomment this block.*/
      fclose(fp);
         /*Track references to watchdog range before Entry Point:*/
      iPtr=0;
      for (loop=0;loop<usedInstBefore;loop++){
         if(decInstBefore[loop].operands.length>0){
            if ((strPtr=strstr((char *)decInstBefore[loop].operands.p,"0x"))){
               miscL=strtoul(strPtr,&strPtr,0);
               if ((miscL>=watchdogIB)&&(miscL<=watchdogIB+wdSize-1)){
                  printf ("* Watchdog reference found:\n");
                  printf("*   fileOffs |iBase+RVA : <opcode> [param.] "
                         "   <mnemonic> [operands]\n");
                  iPtr2=iPtr; /*Print code segment*/
                  for (loop2=1; loop2<=(DISAS_SCOPE/2);loop2++){
                     iPtr2-=decInstBefore[loop-loop2].size;
                  }
                  for (loop2=(DISAS_SCOPE/2);loop2>0;loop2--){
                     printInstruction (decInstBefore[loop-loop2],fed,iPtr2);
                     iPtr2+=decInstBefore[loop-loop2].size;
                  }
                  printf(" /------------------------\n|");
                  printInstruction (decInstBefore[loop], fed, iPtr);
                  printf(" \\------------------------\n");
                  iPtr2+=decInstAfter[loop].size;
                  for (loop2=1;loop2<=(DISAS_SCOPE/2);loop2++){
                     printInstruction (decInstBefore[loop+loop2],fed,iPtr2);
                     iPtr2+=decInstBefore[loop+loop2].size;
                  }
               }
            }
         }
         iPtr+=decInstBefore[loop].size;
      }  /*Track references to watchdog range after Entry Point:*/
      for (loop=0;loop<usedInstAfter;loop++){
         if(decInstAfter[loop].operands.length>0){
            if ((strPtr=strstr((char *)decInstAfter[loop].operands.p,"0x"))){
               miscL=strtoul(strPtr,&strPtr,0);
               if ((miscL>=watchdogIB)&&(miscL<=(watchdogIB+wdSize-1))){
                  printf ("* Watchdog reference found:\n");
                  printf("*   fileOffs |iBase+RVA : <opcode> [param.] "
                         "   <mnemonic> [operands]\n");
                  iPtr2=iPtr; /*Print code segment*/
                  for (loop2=1; loop2<=(DISAS_SCOPE/2);loop2++){
                     iPtr2-=decInstAfter[loop-loop2].size;
                  }
                  for (loop2=(DISAS_SCOPE/2);loop2>0;loop2--){
                     printInstruction (decInstAfter[loop-loop2],fed,iPtr2);
                     iPtr2+=decInstAfter[loop-loop2].size;
                  }
                  printf(" /------------------------\n|");
                  printInstruction (decInstAfter[loop], fed, iPtr);
                  printf(" \\------------------------\n");
                  iPtr2+=decInstAfter[loop].size;
                  for (loop2=1;loop2<=(DISAS_SCOPE/2);loop2++){
                     printInstruction (decInstAfter[loop+loop2],fed,iPtr2);
                     iPtr2+=decInstAfter[loop+loop2].size;
                  }
               }
            }
         }
         iPtr+=decInstAfter[loop].size;
      }
   }
   free (decInstBefore);
   free (decInstAfter);
   return 0;
}
