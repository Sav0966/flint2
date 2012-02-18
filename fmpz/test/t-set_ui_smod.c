/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "ulong_extras.h"
#include "fmpz.h"

int
main(void)
{
    int i;
    flint_rand_t state;

    printf("set_ui_smod....");
    fflush(stdout);

    flint_randinit(state);

    for (i = 0; i < 100000; i++)
    {
        fmpz_t a, b, mz;
        mp_limb_t m, r;

        fmpz_init(a);
        fmpz_init(b);
        fmpz_init(mz);

        do { m = n_randtest(state); } while (m < 2);

        fmpz_set_ui(mz, m);
        fmpz_randtest_mod_signed(a, state, mz);

        r = fmpz_fdiv_ui(a, m);

        fmpz_set_ui_smod(b, r, m);

        if (!fmpz_equal(a, b))
        {
            printf("FAIL:\n");
            printf("a: "); fmpz_print(a); printf("\n");
            printf("m: %lu\n", m);
            printf("r: %lu\n", m);
            printf("b: "); fmpz_print(b); printf("\n");
            abort();
        }

        fmpz_clear(a);
        fmpz_clear(b);
        fmpz_clear(mz);
    }

    flint_randclear(state);
    _fmpz_cleanup();
    printf("PASS\n");
    return 0;
}
