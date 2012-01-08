
// The size of the texture to hold the captured screens. Dimensions should be
// greater than the maximum resolution (therefore 1024x1024 is sufficient).
#define TWIDTH 1024
#define THEIGHT 1024

// structs for output and input textures of the effects
typedef struct {
    int w,h; // dimensions
    int bind; // the GL bind
} TEXTURE;



// a non-textured plane
void nont_plane(float zx, float zy);

TEXTURE * get_texture();

// Capture input i into texture o.
// i is presumed to be the current screen buffer
// zoom is 0.0 ...
void capture(TEXTURE * o, TEXTURE * i, float zx, float zy);

// draws a plane containing texture t to screen s.
void plane(TEXTURE * s, TEXTURE * t, float zx, float zy);

void color_plane(TEXTURE * s, TEXTURE * t, float zx, float zy,
		 float r1, float b1, float g1,
		 float r2, float b2, float g2,
		 float r3, float b3, float g3,
		 float r4, float b4, float g4
		);


void capture(TEXTURE * o, TEXTURE * i, float zx, float zy);

void effect_shine(TEXTURE * out, TEXTURE * in, int tick, float shine_distance);
