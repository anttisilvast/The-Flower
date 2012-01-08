#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "audio_mp3.h"
#include "flower.h"

float rnd() {
    return((float)rand()/RAND_MAX);
}


typedef float Vect[3];
Vect kukka[7]={
    {0.0,0.5,0.0},
    {0.0,0.0,0.0},
    {0.1,-0.2,0.0},
    {0.3,-0.3,0.0},
    {0.5,-0.3,0.0},
    {0.6,-0.2,0.0},
    {0.6,-0.5,0.0}};

Vect kukka_morph[7]={
    {0.0,0.5,0.0},
    {0.0,0.0,0.0},
    {0.2,-0.2,0.0},
    {0.3,-0.15,0.0},
    {0.2,0.0,0.0},
    {0.1,0.0,0.0},
    {0.0,0.1,0.0}};

float tension=0.0, continuity=0.0, bias=0.0;
float tension_new=0.0, continuity_new=0.0, bias_new=0.0;

float rotx=-40.0;
float roty=0.0;

float spd=2.5;

float posz=0.0, posz_new=0.0;

float audio_strength=4.0;

void splineTCP(float u, Vect * control, Vect * result)
{

    (*result)[0] = (2*u*u*u - 3*u*u + 1)*control[1][0]
	+ (-2*u*u*u + 3*u*u)*control[2][0]
	+ (u*u*u - 2*u*u + u)*(0.5*(1-tension)*((1+bias)*(1-continuity)*(control[1][0]-control[0][0])
						+ (1-bias)*(1+continuity)*(control[2][0]-control[1][0])))
	+ (u*u*u - u*u)*(0.5*(1-tension)*((1+bias)*(1+continuity)*(control[2][0]-control[1][0])
					  + (1-bias)*(1-continuity)*(control[3][0]-control[2][0])));

    (*result)[1]
	= (2*u*u*u - 3*u*u + 1)*control[1][1]
	+ (-2*u*u*u + 3*u*u)*control[2][1]
	+ (u*u*u - 2*u*u + u)*(0.5*(1-tension)*((1+bias)*(1-continuity)*(control[1][1]-control[0][1])
						+ (1-bias)*(1+continuity)*(control[2][1]-control[1][1])))
	+ (u*u*u - u*u)*(0.5*(1-tension)*((1+bias)*(1+continuity)*(control[2][1]-control[1][1])
					  + (1-bias)*(1-continuity)*(control[3][1]-control[2][1])));

}
float timef;


void spline3DMorph(float factor, float poikkeama) {
#define NBTW 12
    int i,j,k;
    int size=sizeof(kukka)/sizeof(Vect);

    float vnyt,vnex;
    float ti=SDL_GetTicks();
    Vect r,r_morph;
    float rf[17*NBTW*3];
    int c;
    c=0;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    for (i=0; i<size-3; i++) {
	for (j=0; j<NBTW; j++) {
	    splineTCP((float)j/NBTW, &kukka[i], &r);
	    splineTCP((float)j/NBTW, &kukka_morph[i], &r_morph);
	    for (k=0; k<3; k++)
		rf[c*3+k]=(1.0-factor)*r[k]+factor*r_morph[k];

	    rf[c*3+2]=sin(c*M_PI/(NBTW*4))*0.07;
	    c+=1;

	}
    }
    for (i=0; i<(c-1); i++) {
	vnyt=(float)i/(float)(c-1)*(float)(size-3);
	vnex=(float)(i+1.0)/(float)(c-1)*(float)(size-3);

	rf[i*3]+=+sin((vnyt*2+ti*0.006))*0.02*poikkeama;
	rf[i*3+1]+=+sin((vnyt*2+ti*0.006))*0.02*poikkeama;
	rf[i*3+3]+=sin((vnex*2.1+ti*0.006))*0.02*poikkeama;
	rf[i*3+3+1]+=+sin((vnex*2.0+ti*0.006))*0.02*poikkeama;

	glColor3f(1.0,1.0,1.0);
	glPolygonOffset(3.0,2.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glBegin(GL_POLYGON);
	glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]);
	glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]);
	glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]);
	glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]);
	glEnd();

	glPolygonOffset(-1.0,-5.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glLineWidth(2.0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]*1);
	glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]*1);
	glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]*1);
	glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]*1);
	glEnd();
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}



#define F 0.006
#define FR 0.006

void render_flower() {
    int t;
    int time=SDL_GetTicks();

    tension=F*tension_new+(1.0-F)*tension;
    continuity=F*continuity_new+(1.0-F)*continuity;
    bias=F*bias_new+(1.0-F)*bias;

    for (t=0; t<12; t++) {
	glRotatef(360.0/12.0,0.0,0.0,1.0);
	spline3DMorph(sin(time*0.001*spd)*0.5+0.5,
		      (6.0)*audio_bars[(t*8) % nof_bands]*audio_strength

		     );
    }


}



float r1;
float r2;
float r3;
float r4;
float r5;

void init_flower() {
    srand(SDL_GetTicks());
    r1=rnd();
    r2=rnd();
    r3=rnd();
    r4=rnd();

}
void render_flower_effect() {
    
    posz=FR*posz_new+(1.0-FR)*posz;
    glTranslatef(audio_bars[1]*0.5,(audio_bars[15])*0.05,-posz-audio_bars[1]*3.0+0.1);

    glRotatef(r5*180.0,1.0,1.0,1.0);
    glRotatef(rotx,1.0,0.0,0.0);
    glRotatef(roty,0.0,1.0,0.0);

    glRotatef(SDL_GetTicks()*0.002+audio_bars[1]*14,0.0,0.0,1.0);
    render_flower();

    roty+=(audio_bars[15])*0.6;

    posz=0;

}

void flower_randomize(int tick) {
    static int event1_start=0;
    static int new_event1=15000;

    if ((tick-event1_start)>new_event1) {

	tension_new=(-rnd())*12.0;
	continuity_new=(rnd()-0.5)*32.0;

	event1_start=tick;
    }
}
