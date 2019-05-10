#pragma once
#include "structuri_date.h"

/* eliberez memeoria dintr.o imagine ce retine pixelii */
void freeImag( IMAGINE *imag ) {
    free( (*imag).Img );
}

int StoreImg( IMAGINE Img, char *path ) {

    /* stocheaza in memoria externa o imagine + la destnatie*/

    /* dechis fisierul */
    FILE *f = fopen( path, "wb" );
    int i, j;
    unsigned char x = 0;

    if ( f == NULL ) return 0;

    /* scriu headerul */
    for ( i = 0; i < 54; ++i )
        fwrite( &Img.Header[ i ], 1, 1, f );

    /* scriu pixelii */
    for ( i = Img.n-1; i >= 0; --i ) {
        for ( j = 0; j < Img.m; ++j ) {
            fwrite( &Img.Img[ i * Img.m + j ].B,1, 1, f );
            fwrite( &Img.Img[ i * Img.m + j ].G,1, 1, f );
            fwrite( &Img.Img[ i * Img.m + j ].R,1, 1, f );
        }
        fwrite( &x, sizeof( unsigned char ), Img.pad, f );
    }

    /* inchid fisier + returnez succes */
    fclose( f );

    return 1;

}

IMAGINE LoadImg( char *path ) {

    FILE *f = fopen( path, "rb" );
    int n, m, i, j, pad;

    if( f == NULL ) {
        printf("eroare");
    }
    /* iau dimensiunile imaginii*/
    fseek( f, 18, SEEK_SET );
    fread( &m, sizeof( unsigned int ), 1, f );
    fread( &n, sizeof( unsigned int ), 1, f );
    rewind( f );

    IMAGINE imag;
    /* aloc memorie pentru a retine pixelii*/
    imag.Img = ( PIXEL* )malloc( n * m * sizeof( PIXEL ) );

    /* citesc headerul */
    for ( i = 0; i < 54; ++i ) {
        fread( &(imag.Header[ i ]), sizeof( unsigned char ), 1, f );
    }
    /* calculez paddingul*/
    pad = ( 4 - ( 3 * m ) % 4 ) % 4;
    /* citesc pixelii in ordine inversa */
    for ( i = n-1; i >= 0; --i ) {
        for ( j = 0; j < m; ++j ) {
            fread( &imag.Img[ m * i + j ].B, 1, 1, f );
            fread( &imag.Img[ m * i + j ].G, 1, 1, f );
            fread( &imag.Img[ m * i + j ].R, 1, 1, f );
        }
        /* trec peste octetii de padding */
        fseek( f, pad, SEEK_CUR );
    }

    /* retin restul de imagini */
    imag.n = n;
    imag.m = m;
    imag.pad = pad;
    /* inchid fisierul*/
    fclose( f );
    /* returnez imaginea */
    return imag;

}

