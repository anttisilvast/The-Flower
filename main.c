

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

// screen
#define WIDTH 800
#define HEIGHT 600

#include "audio_mp3.h"

#include "post_processing.h"

#include "flower.h"
#include "balls.h"

int fullscreen=0;

TEXTURE * screen;
TEXTURE * texture[10];

int init_actant(char * songname) {
    int i;
    float b;

    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0 ) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	return -1;
    }

    if (SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_OPENGL|SDL_DOUBLEBUF|fullscreen)<0) {
	fprintf( stderr, "SDL Video mode set failed: %s\n",
		 SDL_GetError());
	return -1;
    }

    /* get some textures */
    screen=(TEXTURE *)malloc(sizeof(TEXTURE));
    screen->w=WIDTH;
    screen->h=HEIGHT;
    screen->bind=-1; // just a dummy, everything is always dumped straight to the screen buffer

    // prepare textures for temporary use

    for (i=0; i<10; i++) {
	texture[i]=malloc(sizeof(TEXTURE));
	glGenTextures(1,&texture[i]->bind);
	texture[i]->w=TWIDTH;
	texture[i]->h=THEIGHT;

	glBindTexture(GL_TEXTURE_2D,texture[i]->bind);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0,
		     GL_RGBA,
		     texture[i]->w,texture[i]->h,
		     0,
		     GL_RGB,
		     GL_UNSIGNED_BYTE,
		     NULL
		    );
    }


    init_flower();
    /* init the audio library */
    if (openaudio(songname,1024)<0) {
        return -1;
    }

    /* init 32 different bandpass filters from 80 Hz to 22 kHz */
    nof_bands=32;
    for (i=0; i<nof_bands; i++) {
	b=80.0+i*(22000.0-80.0)/nof_bands;
	set_band(i,b);
    }

    /* then start the stream */
    if (startaudio()<0) {
        return -1;
    }

    return 0;
}

int main(int argc, char * argv[]) {
    int quit=0;
    int frame=0;
    float col1[]={1,1,1};
    int bg=0;

    int tick;

    SDL_Event e;
 
    if (argc==1) {
	printf("Use: act <track.mp3>\n\n");
	exit(-1);
    }

    if (argc>2)
	if (!strcmp(argv[2],"-f"))
	    fullscreen=SDL_FULLSCREEN;

    init_actant(argv[1]);

    printf("Press ESC to quit.\n");
    SDL_ShowCursor(0);

    while (!quit) {


        tick=SDL_GetTicks();


	//flower_randomize(tick);


	if (bg==0)
	    col1[0]=col1[1]=col1[2]=audio_bars[0]*0.3+0.3;
	else
	    col1[0]=col1[1]=col1[2]=0;

	glClearColor(col1[0],0,col1[2]*0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60.0, (float)WIDTH/(float)HEIGHT, 0.01, 1135.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef(0.0,0.0,-1.0);

	render_flower_effect();
	render_balls(tick);

	capture(texture[0],screen,1.0,1.0);
	effect_shine(screen, texture[0],tick,-0.01);

        SDL_GL_SwapBuffers();
        frame++;
	while(SDL_PollEvent(&e)>0) {

	    if (e.type==SDL_QUIT)
		quit=1;

	    if (e.type==SDL_KEYDOWN)
		switch (e.key.keysym.sym) {
		case SDLK_1:
		    bg=(bg+1) % 2;
		    break;
		case SDLK_ESCAPE:
		    quit=1;
		    break;
		default:
                    break;
		}

	}
    }

    SDL_Quit();

    return 0;
}
