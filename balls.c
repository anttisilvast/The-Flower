
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "audio_mp3.h"
#include "balls.h"

int palkit3_I3=64;
void render_balls(int tick) {
    int i,j;
    float f;

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,1.0);
    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR,GL_ONE);

    glColor3f(0.01,0.01,0.01);

    glPushMatrix();

    glRotatef(tick*0.05,0.0,0.0,1.0);
    for (j=0; j<nof_bands*4; j++) {
	glRotatef(sin(tick*0.001)*15.0+2,0.0,1.0,1.0);
        glTranslatef(cos(tick*0.0002+audio_bars[1])*0.0,0.0,+0.02);
	glBegin(GL_POLYGON);
	for (i=0; i<palkit3_I3; i++) {
	    f=audio_bars[j*1 % nof_bands]*0.64;
	    glVertex3f(cos(i*2*M_PI/palkit3_I3)*f*2,
		       sin(i*2*M_PI/palkit3_I3)*f,
		       0.0);

	}
	glEnd();
    }
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);

    glColor3f(0.010,0.010,0.013);

    glPushMatrix();

    glRotatef(tick*0.05,0.0,0.0,1.0);
    for (j=0; j<nof_bands*4; j++) {
	glRotatef(sin(tick*0.001)*15.0+2,0.0,1.0,1.0);
        glTranslatef(sin(tick*0.0001+audio_bars[1])*0.0,0.0,+0.02);
	glBegin(GL_POLYGON);
	for (i=0; i<palkit3_I3; i++) {
	    f=audio_bars[j*1 % nof_bands]*0.64;
	    glVertex3f(cos(i*2*M_PI/palkit3_I3)*f,
		       sin(i*2*M_PI/palkit3_I3)*f,
		       0.0);

	}
	glEnd();
    }
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}
