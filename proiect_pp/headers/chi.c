#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "structuri_date.h"
#include "store_load.h"


void ChiPatrat( IMAGINE imag ) {
    int k, i, j, d, dim;
    dim = imag.n * imag.m;
    float xPatrat, valMedie;

    valMedie = 1.0*imag.n*imag.m/256;
    xPatrat = 0;
    /* iau fiecare canal de culoare + aritmetica pointeri */
    for ( k = 0; k < 3; ++k ) {
        xPatrat = 0;
        /* Iau fiecare intensitate de culoare pe un canal */
        for ( i = 0; i < 256; ++i ) {
            d = 0;
            for ( j = 0; j < dim; ++j ) {
                /* .B este primul canal */
                if ( *(&imag.Img[ j ].B+k) == i ) {
                    d++;
                }
            }
            /* actualizez valoare testului */
            xPatrat += ( d - valMedie)*( d - valMedie);
        }
        xPatrat /= valMedie;
        /* afisez */
        switch(k){
            case 0 : printf("B : "); break;
            case 1 : printf("G : "); break;
            case 2 : printf("R : "); break;
        }
        printf("%.2f | ",xPatrat);
    }
    printf("\n");
}

void chi() {

    char s[100];

    /* citesc calea imaginii de evaluat */
    printf("Cale imagine pentru chi:");
    scanf( "%s", s );
    /* o incarc in memorie */
    IMAGINE imag = LoadImg( s );
    ChiPatrat( imag );

    /* eliberez memoria */
    freeImag( &imag );

}
