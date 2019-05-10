#pragma once

typedef struct {
    int cifra, x, y;
    double scor;
}FEREASTRA;

union bytes{
    unsigned char b[4];
    unsigned int x;
};

typedef struct {
    unsigned char B, G, R;
} PIXEL ;

typedef struct {
    unsigned char Header[ 54 ];
    int n, m, pad;
    PIXEL *Img;
}IMAGINE;

