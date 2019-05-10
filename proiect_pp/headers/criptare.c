#include <stdio.h>
#include <stdlib.h>

#include "../headers/structuri_date.h"
#include "../headers/store_load.h"
#include "../headers/chi.c"

/* declarare de functii pentru a nu avea probleme de accesibilitate*/
void swap( unsigned int *a, unsigned int*b );
unsigned int* GetPermutare( int dim, char *caleeFisier );
unsigned int* XorShift32( int nnumar_elemente, char *caleeFisier );
PIXEL* ShuffleImg( PIXEL *Img, int dim, char *caleeFisier );
IMAGINE CriptareImg( IMAGINE imag, char *caleeFisier );
IMAGINE DecriptareImag( IMAGINE imag, char *caleeFisier );

void MeniuCriptare () {

    int n, m, i, j, x, ok;
    char caleeImag[ 1000 ], caleeDest[ 1000 ], caleeSecret[ 1000 ];
    ok = 1;

    while( ok ) {
        printf("0) Ieire\n");
        printf("1) Criptare imagine\n");
        printf("2) Decriptare imagine\n");
        printf("3) Testul X^2\n--> Introdu optiunea :\n-->");
        scanf("%d",&ok);
        if ( ok ) {
            if ( ok == 3 ) {
                chi();
            } else {
                 printf("--->Calea imaginii: ");
                scanf("%s",caleeImag);
                IMAGINE I = LoadImg( caleeImag );
                printf("--->Calea stocare: ");
                scanf("%s",caleeDest);

                printf("--->Calea secret: ");
                scanf("%s",caleeSecret);

                if( ok == 1 ) I = CriptareImg( I, caleeSecret );
                else I = DecriptareImag( I, caleeSecret );

                StoreImg( I, caleeDest );

            }
        }
    }


}


void swap( unsigned int *a, unsigned int*b ) {
    unsigned int c = *a;
    *a = *b;
    *b = c;
}

IMAGINE DecriptareImag( IMAGINE imag, char *caleeFisier ) {
    /* iau numerele aleatoare */
    unsigned int SV, *R = XorShift32( 2*imag.n*imag.m-1,caleeFisier );
    unsigned int *P = GetPermutare(imag.n*imag.m,caleeFisier ); /* iau permutarea */
    unsigned int *V = malloc( imag.n*imag.m * sizeof(unsigned int)); /* inversa permutarii */
    PIXEL *nou = malloc( imag.n*imag.m * sizeof(PIXEL)); /* vectorul de pixel dezpermutati */
    union bytes bSV, bR;
    int i;

    /* citesc al doilea numar secret */
    FILE *f = fopen(caleeFisier,"r");
    fscanf(f,"%*u%u",&SV);
    fclose(f);

    int dim = imag.n*imag.m-1;

    bSV.x = SV;
    bR.x = R[ dim ];
    /* decriptez dupa metoda data */
    for ( i = imag.n*imag.m-1; i >= 0; --i ) {
        bR.x = R[ dim + i ];
        imag.Img[i].B = (imag.Img[i-1].B^imag.Img[i].B)^bR.b[0];
        imag.Img[i].G = (imag.Img[i-1].G^imag.Img[i].G)^bR.b[1];
        imag.Img[i].R = (imag.Img[i-1].R^imag.Img[i].R)^bR.b[2];
    }
    imag.Img[0].B =  (bSV.b[0]^imag.Img[0].B)^bR.b[0];
    imag.Img[0].G =  (bSV.b[1]^imag.Img[0].G)^bR.b[1];
    imag.Img[0].R =  (bSV.b[2]^imag.Img[0].R)^bR.b[2];

    /* calculez inversa si dezpermut pixelii */
    for ( i = 0; i < dim ;++i )
        V[ i  ] = P[ i ];

    for ( i = 0; i < dim; ++i )
        nou[ i ] = imag.Img[ V[ i ] ];

    /* eliberez memoria + pixelii vechi */
    free( V );
    free( P );
    free( imag.Img );
    /* dau pixelii noi si intorc imaginea*/
    imag.Img = nou;
    return imag;

}

IMAGINE CriptareImg( IMAGINE imag, char *caleeFisier ) {

    imag.Img = ShuffleImg( imag.Img, imag.n * imag.m, caleeFisier ); /* pixeli permutati */
    unsigned int SV, *R = XorShift32( 2*imag.n*imag.m-1, caleeFisier ); /* numere random */
    union bytes bSV, bR;
    int i;

    /* al doilea numar secret pentru cirptare */
    FILE *f = fopen(caleeFisier,"r");
    fscanf(f,"%*u%u",&SV);
    fclose(f);

    /* dimensiunea imaginii */
    int dim = imag.n*imag.m-1;

    bSV.x = SV;
    bR.x = R[ dim ];

    /* criptez imaginea prin metoda descrisa */

    imag.Img[0].B =  (bSV.b[0]^imag.Img[0].B)^bR.b[0];
    imag.Img[0].G =  (bSV.b[1]^imag.Img[0].G)^bR.b[1];
    imag.Img[0].R =  (bSV.b[2]^imag.Img[0].R)^bR.b[2];


    for ( i = 1; i < imag.n * imag.m; ++i ) {
        bR.x = R[ dim + i ];
        imag.Img[i].B = (imag.Img[i-1].B^imag.Img[i].B)^bR.b[0];
        imag.Img[i].G = (imag.Img[i-1].G^imag.Img[i].G)^bR.b[1];
        imag.Img[i].R = (imag.Img[i-1].R^imag.Img[i].R)^bR.b[2];
    }

    /* eliberez memoria */
    free( R );

    /* intorc imaginea criptata */
    return imag;

}

PIXEL* ShuffleImg( PIXEL *Img, int dim, char *caleeFisier ) {
    /* primeste PIXELII unei imagini si ii permuta */

    int i;
    unsigned int *P = GetPermutare( dim, caleeFisier ); /* permutare aleatoare */
    PIXEL *Shuffled = ( PIXEL * )malloc( dim *sizeof(PIXEL) ); /* pixelii permutati */

    /* permut */
    for( i = 0; i < dim; ++i ) {
        Shuffled[ P[ i ] ] = Img[ i ];
    }

    /* eliberez memoria */
    free( P );
    free( Img );
    /* intorc noi pixeli */
    return Shuffled;

}

unsigned int* GetPermutare( int dim, char *caleeFisier ) {

    /* returneaza o permutare pseudoaleatoarea pe baza xorShift32 */

    unsigned int *V = XorShift32( dim-1, caleeFisier );
    unsigned int *P = malloc( dim * sizeof( unsigned int ) );
    int i, r;
    /* poernesc de la permutarea indetinca */
    for ( i = 0; i < dim; ++i ) P[ i ] = i;

    for ( i = dim - 1; i >= 1; --i ) {
        r = V[ dim - i - 1 ] % ( i + 1 ); /* liniarizare modulo */
        swap( &P[ i ], &P[ r ] );
    }

    /* eliberez memoria si intorc permutare */
    free( V );
    return P;

}

unsigned int* XorShift32( int nnumar_elemente, char *caleeFisier ) {
    /* da cele numar_elemente numere aleatoare + calea fisier cheie secreta */
    unsigned int i, r, n;
    /* vectorul de returnat */
    unsigned int *V = ( unsigned int* )malloc( nnumar_elemente * sizeof( int ) );

    FILE *f = fopen( caleeFisier, "r" );
    if ( f == NULL ) {
        printf( "fisier inexistent" );
        return NULL;
    }
    /* seedul pentru generator*/
    fscanf( f, "%u", &r );
    fclose( f );

    /* Alg xorShift32*/
    for ( i = 0; i < nnumar_elemente; ++i ) {
        r ^= r << 13;
        r ^= r >> 17;
        r ^= r << 5;
        V[ i ] = r;
    }

    return V;

}
