#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../headers/store_load.h"
#include "../headers/structuri_date.h"

/* valorile piexelilo pentru colorearea fiecarei ferestre in parte */
PIXEL culori_cifre[] = { (PIXEL){0,0,255}, (PIXEL){0,255,255}, (PIXEL){0,255,0},
                         (PIXEL){255,255,0},(PIXEL){255,0,255},(PIXEL){255,0,0},
                         (PIXEL){192,192,192}, (PIXEL){0,140,255}, (PIXEL){128,0,128},(PIXEL){0,0,128}
                        };

void TemplateMatching( IMAGINE S, IMAGINE I, double pS, FEREASTRA **detectie, int *dim );
unsigned char getGreyValue( PIXEL x );
double Suprapunere( FEREASTRA a, FEREASTRA b, int n, int m, int sn, int sm );

/* comparator pentru qSort */
int cmp( const void *a, const void *b ) {

    /* conversie de pointeri */
    FEREASTRA *A = (FEREASTRA*)a;
    FEREASTRA *B = (FEREASTRA*)b;

    if ( (*A).scor < (*B).scor ) return -1;
    if ( (*A).scor > (*B).scor ) return 1;
    return 0;


}

void Match() {

    char s[100];
    /* calea fisierului ce retine calea imaginilor seblon + o citesc + retin */
    printf( "Cale imaginini fisier sabloane: " );
    scanf( "%s", s );
    FILE *f = fopen( s, "r" );

    /* pe ce imaginea rulez potrivirea de sabloane + citire */
    printf( "Cale imagine de procesat: " );
    scanf( "%s", s );
    IMAGINE I = LoadImg( s );

    IMAGINE S; /* imagine de sablon */
    FEREASTRA *Detectii = NULL;

    int i, n = 0;
    int dimSH, dimSW;
    for ( i = 0; i < 10; ++i ) {
        fscanf( f, "%s", s );
        /* citesc si incarc o imagine sablon */
        S = LoadImg( s );
        dimSH = S.n;
        dimSW = S.m;
        int j, cm = n;
        /* rulez potrivirea cu prag 0.5 */
        TemplateMatching( S, I, 0.5, &Detectii, &n ); /* Detectii + n = parametrii de intoarcere care imi dau ferestrecele de detectie*/
        if(Detectii != NULL)
        /* dau ferestrelor noi gasite cifra corespunzatoare  */
            for( j = cm; j < n; ++j ) {
                Detectii[ j ].cifra = i;
            }
        /* eliberez imaginea sablon */
        freeImag( &S );
    }
    /* inchid fisierul cu sabloane */
    fclose( f );

    if(Detectii!=NULL){
        /* rulez eliminarea non maximelor */
        printf("\n");
        qsort( Detectii, n,sizeof(FEREASTRA),cmp);


        int j;
        for ( i = n - 1; i >= 0; --i ) {
            /* daca nu am eliminat deja fereastra i */
            if( Detectii[ i ].cifra != -1 ) {
                for ( j = i - 1; j >= 0; --j ) {
                    /* vad suprapunerea dintre maxim si ferestrele cu scor mai mic + le elimin */
                    if( Detectii[ j ].cifra != -1 && Suprapunere( Detectii[ i ], Detectii[ j ], I.n, I.m, dimSH, dimSW ) > 0.2 )
                        Detectii[ j ].cifra = -1;
                }
                /* fereastra curenta este maxima si o colorez corespunzator */
                colorare( I, Detectii[ i ].x, Detectii[ i ].y, dimSH, dimSW, culori_cifre[Detectii[i].cifra] );
            }
        }

    }
    /* salvez imaginea sablonata */
    printf( "Cale imagine de salvat: " );
    scanf( "%s", s );
    StoreImg( I, s );

    /* eliberez memoria */
    freeImag( &I );
    free(Detectii);

}

/* valoarea unui pixel pe scara de gri*/
unsigned char getGreyValue( PIXEL x ) {
    return ( unsigned char)( 0.299 * x.R + 0.587 * x.G + 0.114 * x.B );
}

void colorare( IMAGINE imag, int i, int j, int n, int m, PIXEL culoare ) {

    int x;
    /* marginile stanga- dreapta */
    for ( x = 0; x < n; ++x ) {
        imag.Img[ ( x + i ) * imag.m + j ] = culoare;
        imag.Img[ ( x + i ) * imag.m + j + m - 1 ] = culoare;
    }

    /* marginile de sus jos */
    for ( x = 0; x < m; ++x ) {
        imag.Img[  i * imag.m + x + j ] = culoare;
        imag.Img[ ( n - 1 + i ) * imag.m + j + x ] = culoare;
    }

}

double Suprapunere( FEREASTRA a, FEREASTRA b, int n, int m, int sn, int sm ) {

    int i,j,inter, x_a, y_a, x_b, y_b;
    x_a = a.x + sn;
    y_a = a.y + sm;
    inter = 0;

    for( i = 0; i < sn; ++i ) {
        for ( j = 0; j < sm; ++j ) {
            /* coltul dreapta jos */
            x_b = i+b.x;
            y_b = j+b.y;
            /* cresc numarul de pixeli din intersectie */
            if ( a.x <= x_b && x_b <= x_a && a.y <= y_b && y_b <= y_a ) inter++;
        }
    }

    /* procentajul de suprapunere */
    return ( double )inter / (sn * sm * 2 - inter );

}

void TemplateMatching( IMAGINE S, IMAGINE I, double pS, FEREASTRA **detectie, int *dim ) {

    int i, j, t, k, dim_loc;
    double scor, dimS,deviatieS, deviatieI, medI, medS;
    FEREASTRA *aux;
    dim_loc = *dim;


    dimS = S.n*S.m;
    aux = NULL;
    deviatieS = deviatieI = medS = 0.0; /* deviatie pentru sablon + deviatie pentru fereastra + media pixelilor din sablon */

    /* media pixelilor din sabon trecuti prin scala de gri */
    for( i = 0; i < dimS; ++i )
        medS += getGreyValue( S.Img[ i ] );
    medS /= S.n*S.m; /* termin de calculat valoarea medie */

    /* calculez deviatia = ( valoare gri pixel - medie ) la patrat */
    for ( i = 0; i < dimS; ++i )
        deviatieS += ( getGreyValue(S.Img[ i ]) - medS ) * ( getGreyValue(S.Img[ i ]) - medS );

    deviatieS /= dimS - 1; /* impart la numatul de pixeli */
    deviatieS = sqrt( deviatieS ); /* scot radicalul */


    /* ma pozitionez cu coltul stanga sus al fiecarui sablon intr.un pixel a.i. sa nu ies din imagine */
    for ( i = 0; i + S.n-1 < I.n ; ++i ) {
        for ( j = 0; j + S.m - 1 < I.m; ++j ) {

            deviatieI = medI = 0; /* resetez deviatile de calculata pentru noua fereastra */
            /* calculez valoarea medie a pixelilor greyscale din fereastra I */
            for ( k = 0; k < S.n ; ++k ) {
                for ( t = 0; t < S.m; ++t ) {
                    medI += getGreyValue(I.Img[ ( i + k ) * I.m + t + j ]);
                }
            }
            medI /= dimS; /* impart la dimensiunea ferestre = dimensiunea sablonului */

            scor = 0;
            /* calculez deviatiei pentru fereastra dupa formula ..... + cresc scorul de detectie*/
            for ( k = 0; k < S.n; ++k ) {
                for ( t = 0; t < S.m; ++t ) {
                    deviatieI += (getGreyValue(I.Img[ ( i + k ) * I.m + t + j ]) - medI) * ( getGreyValue(I.Img[ ( i + k ) * I.m + t + j ]) - medI );
                    scor += ( getGreyValue( I.Img[ ( i + k ) * I.m + t + j ] ) - medI ) * ( getGreyValue( S.Img[ k * S.m + t ] ) - medS ) / dimS;
                }
            }
            /* pentru o explicatie mai amanuntita a formulelor vezi
                EnuntProiect.pdf pagina 10 punctul b)
             */

            deviatieI = sqrt( deviatieI / (dimS-1) );
            scor /= deviatieI*deviatieS;

            if ( scor > pS ) {
                /* am gasit o detectie noua + fac loc pentru ea */
                aux = (FEREASTRA*)realloc( *detectie, sizeof(FEREASTRA)*( (*dim) + 1 ) );
                if ( aux != NULL ) {
                    /* adaug noua detectie la vectorul de detectii */
                    *detectie = aux;
                    (*detectie)[ *dim ] = (FEREASTRA){ -1, i, j, scor };
                    (*dim) += 1; /* cresc numarul de detectii */

                } else {
                    /* am ramas fara memorie*/
                    free( *detectie );
                    *detectie = NULL;
                    /* am eliberat memoria folosita pana acum */
                    *dim = 0;
                    printf("fara memorie:(\n"); /* afisez o eroare si opresc executia */
                    return ;
                }
            }

        }
    }

}
