/*CryptoLocator v1.02 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)
  This program is released under the GNU GPL License version 3.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <complex.h>
#include "fftw3.h"               /*Include function definitions for
                                   external library FFTW*/

/* GLOBAL DEFINITIONS*/
#define MAX_CRYPTO_SUBSTRINGS 10240  /*Max. # of random substrings to be
                                       recorded within a CLMap structure.*/
#define ALPHA 0.01                  /*Statistical threshold below which the
                                       sequence is considered non-random.*/

                           /*Functions from external library CEPHES:*/
extern double igamc( double a, double x );
extern double igam( double a, double x );

/* GLOBAL STRUCTURES */
typedef struct {
   long int begin[MAX_CRYPTO_SUBSTRINGS];
   long int end[MAX_CRYPTO_SUBSTRINGS];
   double p[MAX_CRYPTO_SUBSTRINGS];
   int numSubstrings;
   int overflow;
} CLMap;



