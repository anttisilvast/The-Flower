#include <stdio.h>
#include <math.h>
#include <string.h>

#include "notch.h"

NOTCH_FILTER *  init_notch(float cutoff) {
    NOTCH_FILTER * l=malloc(sizeof(NOTCH_FILTER));
    float steep = 0.99;;
    float r = steep * 0.99609375;
    float f = cos(M_PI * cutoff / SAMPLING_RATE);
    l->cutoff = cutoff;
    l->a0 = (1 - r) * sqrt( r * (r - 4 * (f * f) + 2) + 1);
    l->b1 = 2 * f * r;
    l->b2 = -(r * r);

    l->x1 = 0.0;
    l->x2 = 0.0;
    return l;
}

float process_notch(NOTCH_FILTER * l, float x0) {
    float outp = l->a0 * x0 + l->b1 * l->x1 + l->b2 * l->x2;
    l->x2 = l->x1;
    l->x1 = outp;

    return outp;
}

