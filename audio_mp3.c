#include <stdio.h>
#include <math.h>

#include <SDL.h>

#include <mpg123.h>
#include <mpglib.h>

#include "notch.h"
#include "audio_mp3.h"

NOTCH_FILTER * bp_filter[MAX_BANDS];

float audio_buffer[MAX_AUDIO_BUFFER_SIZE];
char audio_buffer_char[MAX_AUDIO_BUFFER_SIZE];

#define MP3_BUFFER_SIZE MAX_AUDIO_BUFFER_SIZE

int nof_bands=0;

float audio_bars[MAX_BANDS];

char * song;
int songsize;

int mp3_blocksize=0;

// mpglib structure
struct mpstr mp;

// the SDL audio spec
SDL_AudioSpec SDL_audio;

int filesize(char * filename) {
    FILE * f;
    int size;
    f=fopen(filename,"rb");
    if (f==NULL) {
	printf("error opening %s!\n",filename);
	return -1;
    }
    fseek(f,0,SEEK_END);
    size=ftell(f);
    fclose(f);
    return (size);
}

int read_file(char * buffer, char * filename) {
    int i=0;
    FILE * f;
    printf("Reading %s...\n",filename);

    f=fopen(filename,"rb");
    if (f==NULL) {
	printf("error opening file %s\n",filename);
        return -1;
    }
    while (!feof(f)) {
	*(buffer+i++)=fgetc(f);
    }
    fclose(f);
    printf("done\n");
    return 1;
}


void mp3_callback(void * unused, Uint8 * stream, int len) {
    int i,b;
    int ret;
    float f;
    static int playpos=0;
    static int playfinished=0;
    int chars[4];
    float temp_bars[MAX_BANDS];

    if (playfinished)
	return;

    for (b=0; b<nof_bands; b++)
        temp_bars[b]=0.0;

    for (i=0; i<len; i++) {
	if (mp3_blocksize==0) {
	    ret = decodeMP3(&mp,NULL,0,audio_buffer_char,MP3_BUFFER_SIZE,&mp3_blocksize);
	    if (ret!=MP3_OK)
		playfinished=1;
	    playpos=0;
	}
	*stream = audio_buffer_char[playpos];
	chars[i % 4]=(signed char)*stream;
	if (i % 4==3) {
	    audio_buffer[i/4]=
		((float)((chars[0]+chars[2])/2+
			 ((chars[1]+chars[3])/2)*256)*2.0/65536.0);

	    for (b=0; b<nof_bands; b++) {
		f=process_notch(bp_filter[b],audio_buffer[i/4]);
		if (fabs(f)>temp_bars[b])
		    temp_bars[b]=fabs(f);
	    }


	}
	stream++;
	playpos++;
        mp3_blocksize--;
    }


    {
#define HEIGHT 1.0
#define D 0.45
#define TAU 0.25
#define DIF 5.0
	float lk=2.0;
	float l0=2.0;

	float scale = HEIGHT / ( log((1 - D) / D) * 2 );
	float x00 = D*D*1.0/(2 * D - 1);
	float y00 = -log(-x00) * scale;
	float y;
	int i;
	for (i=0; i<nof_bands; i++) {
	    y=temp_bars[i];
	    y=y*(i*lk+l0);

	    y = ( log(y - x00) * scale + y00 ); /* Logarithmic amplitude */

	    y = ( (DIF-2.0)*y +
		  (i==0       ? 0 : temp_bars[i-1]) +
		  (i==31 ? 0 : temp_bars[i+1])) / DIF;

	    y=(1.0-TAU)*audio_bars[i]+TAU*y;
	    audio_bars[i]=y;
	}
    }


}


int openaudio(char * filename, int buffer_size) {
    // init mpglib
    if (InitMP3(&mp)==MP3_ERR) {
	fprintf(stderr,"mpglib error!\n");
        return -1;
    }

    // open SDL audio
    if (SDL_Init(SDL_INIT_AUDIO)<0) {
	printf("SDL audio init error!\n");
        return -1;
    }
    SDL_audio.freq=44100;
    SDL_audio.format=AUDIO_S16;
    SDL_audio.channels=2;
    SDL_audio.samples=buffer_size;
    SDL_audio.callback=mp3_callback;
    SDL_audio.userdata=NULL;

    if (SDL_OpenAudio(&SDL_audio,0)<0) {
	printf("SDL_audio error!\n");
        return -1;
    }

    // read the file
    songsize = filesize(filename);
    song=(char *)malloc(songsize);
    if (read_file(song,filename)<0) {
	return -1;
    }
    return 0;

}

/* Starts sampling and analyzing */
int startaudio() {
    if (decodeMP3(&mp,song,songsize,audio_buffer_char,MP3_BUFFER_SIZE,&mp3_blocksize)==MP3_ERR) {
	printf("mpglib start error!\n");
        return -1;
    }
    SDL_PauseAudio(0);
    return 0;
}

/* Closes audio interface */
int closeaudio() {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    return 0;
}

/* sets band i to correspond to frequencies low...high */
void set_band(int i, float cutoff) {
    bp_filter[i]=init_notch(cutoff);
}
