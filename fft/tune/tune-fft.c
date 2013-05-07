/* 

Copyright 2009, 2011 William Hart. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY William Hart ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL William Hart OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of William Hart.

*/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include "flint.h"
#include "ulong_extras.h"
#include "fft.h"

int
main(void)
{
    mp_bitcnt_t depth, w, depth1, w1;
    clock_t start, end;
    double elapsed;
    double best;
    mp_size_t best_off, off, best_d, best_w;

    flint_rand_t state;

    printf("/* fft_tuning.h -- autogenerated by tune-fft */\n\n");
    printf("#ifndef FFT_TUNING_H\n");
    printf("#define FFT_TUNING_H\n\n");
    printf("#include \"gmp.h\"\n\n");
    printf("#define FFT_TAB \\\n");
    fflush(stdout);

    flint_randinit(state);
    _flint_rand_init_gmp(state);

    printf("   { "); fflush(stdout);
    for (depth = 6; depth <= 10; depth++)
    {
        printf("{ "); fflush(stdout);
        for (w = 1; w <= 2; w++)
        {
            int iters = 100*((mp_size_t) 1 << (3*(10 - depth)/2)), i;
            
            mp_size_t n = (1UL<<depth);
            mp_bitcnt_t bits1 = (n*w - (depth + 1))/2; 
            mp_size_t len1 = 2*n;
            mp_size_t len2 = 2*n;

            mp_bitcnt_t b1 = len1*bits1, b2 = len2*bits1;
            mp_size_t n1, n2;
            mp_size_t j;
            mp_limb_t * i1, *i2, *r1;
   
            n1 = (b1 - 1)/FLINT_BITS + 1;
            n2 = (b2 - 1)/FLINT_BITS + 1;
                    
            i1 = flint_malloc(2*(n1 + n2)*sizeof(mp_limb_t));
            i2 = i1 + n1;
            r1 = i2 + n2;
   
            mpn_urandomb(i1, state->gmp_state, b1);
            mpn_urandomb(i2, state->gmp_state, b2);
  
            best_off = -1;
            
            for (off = 0; off <= 4; off++)
            {
               start = clock();
               for (i = 0; i < iters; i++)
                  mul_truncate_sqrt2(r1, i1, n1, i2, n2, depth - off, w*((mp_size_t)1 << (off*2)));
               end = clock();
               
               elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
               
               if (elapsed < best || best_off == -1)
               {
                  best_off = off;
                  best = elapsed;
               }
            }
           
            printf("%ld", best_off); 
            if (w != 2) printf(",");
            printf(" "); fflush(stdout);

            flint_free(i1);
        }
        printf("}");
        if (depth != 10) printf(",");
        printf(" "); fflush(stdout);
    }

    printf("}\n\n");
    
    best_d = 12;
    best_w = 1;
    best_off = -1;

    printf("#define MULMOD_TAB \\\n");
    fflush(stdout);
    printf("   { "); fflush(stdout);
    for (depth = 12; best_off != 1 ; depth++)
    {
        for (w = 1; w <= 2; w++)
        {
            int iters, i;
            mp_size_t n = (1UL<<depth);
            mp_bitcnt_t bits = n*w;
            mp_size_t int_limbs = (bits - 1)/FLINT_BITS + 1;
            mp_size_t j;
            mp_limb_t c, * i1, * i2, * r1, * tt;
        
            if (depth <= 21) iters = 32*((mp_size_t) 1 << (21 - depth));
            else iters = FLINT_MAX(32/((mp_size_t) 1 << (depth - 21)), 1);

            i1 = flint_malloc(6*(int_limbs+1)*sizeof(mp_limb_t));
            i2 = i1 + int_limbs + 1;
            r1 = i2 + int_limbs + 1;
            tt = r1 + 2*(int_limbs + 1);
                
            mpn_urandomb(i1, state->gmp_state, int_limbs*FLINT_BITS);
            mpn_urandomb(i2, state->gmp_state, int_limbs*FLINT_BITS);
            i1[int_limbs] = 0;
            i2[int_limbs] = 0;

            depth1 = FLINT_CLOG2(bits);
            depth1 = depth1/2;

            w1 = bits/(1UL<<(2*depth1));

            best_off = -1;
            
            for (off = 0; off <= 4; off++)
            {
               start = clock();
               for (i = 0; i < iters; i++)
                  _fft_mulmod_2expp1(r1, i1, i2, int_limbs, depth1 - off, w1*((mp_size_t)1 << (off*2)));
               end = clock();
               
               elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
               
               if (best_off == -1 || elapsed < best)
               {
                  best_off = off;
                  best = elapsed;
               }
            }
           
            start = clock();
            for (i = 0; i < iters; i++)
                flint_mpn_mulmod_2expp1_basecase(r1, i1, i2, 0, bits, tt);
            end = clock();
               
            elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
            if (elapsed < best)
            {
                best_d = depth + (w == 2);
                best_w = w + 1 - 2*(w == 2);
            }

            printf("%ld", best_off); 
            if (w != 2) printf(", "); fflush(stdout);

            flint_free(i1);
        }
        printf(", "); fflush(stdout);
    }
    printf("1 }\n\n");
    
    printf("#define FFT_N_NUM %ld\n\n", 2*(depth - 12) + 1);
    
    printf("#define FFT_MULMOD_2EXPP1_CUTOFF %ld\n\n", ((mp_limb_t) 1 << best_d)*best_w/(2*FLINT_BITS));
    
    flint_randclear(state);
    
    printf("#endif\n");
    return 0;
}
