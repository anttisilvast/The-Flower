typedef struct {
    float cutoff;
    float a0;
    float b1;
    float b2;
    float x1;
    float x2;
} NOTCH_FILTER;

#define SAMPLING_RATE 44100

NOTCH_FILTER *  init_notch(float cutoff);
float process_notch(NOTCH_FILTER * l, float x0);
