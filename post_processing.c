#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "post_processing.h"

TEXTURE * get_texture() {
    TEXTURE * texture;
    texture=malloc(sizeof(TEXTURE));
    glGenTextures(1,&texture->bind);
    texture->w=TWIDTH;
    texture->h=THEIGHT;

    glBindTexture(GL_TEXTURE_2D,texture->bind);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0,
		 GL_RGBA,
		 texture->w,texture->h,
		 0,
		 GL_RGB,
		 GL_UNSIGNED_BYTE,
		 NULL
		);
    return texture;

}
// Capture input i into o.
// i is presumed to be the current screen buffer
// zoom is 0.0 ...
void capture(TEXTURE * o, TEXTURE * i, float zx, float zy) {
    glBindTexture(GL_TEXTURE_2D,o->bind);
    glCopyTexSubImage2D(GL_TEXTURE_2D,0,
			i->w*0.5*(1.0-zx),
			i->h*0.5*(1.0-zy),
			i->w*0.5*(1.0-zx),
			i->h*0.5*(1.0-zy),
			zx*i->w,zy*i->h
		       );


}

void color_plane(TEXTURE * s, TEXTURE * t, float zx, float zy,
		 float r1, float b1, float g1,
		 float r2, float b2, float g2,
		 float r3, float b3, float g3,
		 float r4, float b4, float g4
		) {
    float kx=1.0,ky=0.75,kz=-0.99;
    float tsx=(float)s->w/t->w*(0.5-zx*0.5);
    float tsy=(float)s->h/t->h*(0.5-zy*0.5);
    float tx=tsx+zx*(float)s->w/t->w,
	ty=tsy+zy*(float)s->h/t->h;

    glBindTexture(GL_TEXTURE_2D,t->bind);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    if (zx>1.0) {
        tx=(float)s->w/t->w;
	ty=(float)s->h/t->h;
        kx=1.0*(1.0/zx);
        ky=0.75*(1.0/zy);
	tsx=0.0; tsy=0.0;

	tsx+=0.002; tsy+=0.002;
        tx-=0.002; ty-=0.002;
	glBegin(GL_POLYGON);
        glColor3f(r1,g1,b1);
	glTexCoord2f(tsx,tsy);
	glVertex3f(-kx,-ky,kz);
        glColor3f(r2,g2,b2);
	glTexCoord2f(tx,tsy);
	glVertex3f(+kx,-ky,kz);
        glColor3f(r3,g3,b3);
	glTexCoord2f(tx,ty);
	glVertex3f(+kx,+ky,kz);
        glColor3f(r4,g4,b4);
	glTexCoord2f(tsx,ty);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    } else {
	tsx+=0.002; tsy+=0.002;
        tx-=0.002; ty-=0.002;

	glBegin(GL_POLYGON);
        glColor3f(r1,g1,b1);
	glTexCoord2f(tsx,tsy);
	glVertex3f(-kx,-ky,kz);
        glColor3f(r2,g2,b2);
	glTexCoord2f(tx,tsy);
	glVertex3f(+kx,-ky,kz);
        glColor3f(r3,g3,b3);
	glTexCoord2f(tx,ty);
	glVertex3f(+kx,+ky,kz);
        glColor3f(r4,g4,b4);
	glTexCoord2f(tsx,ty);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    }

}

// draws a plane containing texture t to screen s.
void plane(TEXTURE * s, TEXTURE * t, float zx, float zy) {

    float kx=1.0,ky=0.75,kz=-0.99;
    float tsx=(float)s->w/t->w*(0.5-zx*0.5);
    float tsy=(float)s->h/t->h*(0.5-zy*0.5);
    float tx=tsx+zx*(float)s->w/t->w,
	ty=tsy+zy*(float)s->h/t->h;

    glBindTexture(GL_TEXTURE_2D,t->bind);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    if (zx>1.0) {
        tx=(float)s->w/t->w;
	ty=(float)s->h/t->h;
        kx=1.0*(1.0/zx);
        ky=0.75*(1.0/zy);
	tsx=0.0; tsy=0.0;

	tsx+=0.002; tsy+=0.002;
        tx-=0.002; ty-=0.002;
	glBegin(GL_POLYGON);
	glTexCoord2f(tsx,tsy);
	glVertex3f(-kx,-ky,kz);
	glTexCoord2f(tx,tsy);
	glVertex3f(+kx,-ky,kz);
	glTexCoord2f(tx,ty);
	glVertex3f(+kx,+ky,kz);
	glTexCoord2f(tsx,ty);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    } else {
	tsx+=0.002; tsy+=0.002;
        tx-=0.002; ty-=0.002;

	glBegin(GL_POLYGON);
	glTexCoord2f(tsx,tsy);
	glVertex3f(-kx,-ky,kz);
	glTexCoord2f(tx,tsy);
	glVertex3f(+kx,-ky,kz);
	glTexCoord2f(tx,ty);
	glVertex3f(+kx,+ky,kz);
	glTexCoord2f(tsx,ty);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    }
}

void nont_plane(float zx, float zy) {

    float kx=1.0,ky=0.75,kz=-0.99;
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    if (zx>1.0) {
        kx=1.0*(1.0/zx);
        ky=0.75*(1.0/zy);
	glBegin(GL_POLYGON);
	glVertex3f(-kx,-ky,kz);
	glVertex3f(+kx,-ky,kz);
	glVertex3f(+kx,+ky,kz);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    } else {
	glBegin(GL_POLYGON);
	glVertex3f(-kx,-ky,kz);
	glVertex3f(+kx,-ky,kz);
	glVertex3f(+kx,+ky,kz);
	glVertex3f(-kx,+ky,kz);
	glEnd();
    }
}

float shine_strength=1.2;
int shine_N=10;
int shine_clear_first=1;

void effect_shine(TEXTURE * out, TEXTURE * in, int tick, float shine_distance) {
    int i;
    float c=shine_strength*1.0/(float)shine_N;

    if (shine_clear_first)
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f(c,c,c);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    for (i=0; i<shine_N; i++) {
	    glEnable(GL_BLEND);
            glBlendFunc(GL_ONE,GL_ONE);
            glPushMatrix();
            glTranslatef(0.0,0.0,i*shine_distance);
	    plane(out,in,1.0,1.0);
            glPopMatrix();
    }


    glPopMatrix();
}
