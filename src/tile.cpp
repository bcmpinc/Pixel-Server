#include <cstdlib>
#include "tile.h"

inline int fist(float f) {
	int r;
	__asm__ __volatile__ ("fistpl %0" : "=m" (r) : "t" (f) : "st") ;
	return r;
}

// Color functions.
#define CLAMP(x) (x<0?0:x>255?255:x)
inline int rgb(int r, int g, int b) {
	return CLAMP(r)|CLAMP(g)<<8|CLAMP(b)<<16;
}
inline int rgb(float r, float g, float b) {
	return rgb(fist(r*255),fist(g*255),fist(b*255));
}

// Noise functions
#define NOISE_LENGTH 512
int p[NOISE_LENGTH];
float g[NOISE_LENGTH];
void tile_init(int argc, char ** argv) {
	srand(1337);
	for (int i=0; i<NOISE_LENGTH; i++) {
		p[i]=i;
		g[i]=rand() / (RAND_MAX/2.0f) - 1;
	}
	for (int i=0; i<NOISE_LENGTH; i++) {
		int j = rand() % NOISE_LENGTH;
		p[i]^=p[j]^=p[i]^=p[j];
	}
}

float noise(int x, int y) {
	return g[(x + p[y & 0xff]) & 0xff];
}

float lerp3(float t, float a, float b, float c) {
	float st = t * t;
	return (c*(st+2*t+1) + b*(5-st) + a*(st-2*t+1)) / (7+st);
}

float noise(float x, float y) {
	int   px = fist(x), py = fist(y);
	      x -= px;      y -= py;

	float a = noise(px-1, py-1);
	float b = noise(px  , py-1);
	float c = noise(px+1, py-1);
	float d = noise(px-1, py  );
	float e = noise(px  , py  );
	float f = noise(px+1, py  );
	float g = noise(px-1, py+1);
	float h = noise(px  , py+1);
	float i = noise(px+1, py+1);
	
	x*=2;
	y*=2;
	float k=lerp3(x,a,b,c);
	float l=lerp3(x,d,e,f);
	float m=lerp3(x,g,h,i);
	
	return  lerp3(y,k,l,m);
}

float f(float x, float y) {
	return noise(x,y);
}

float fs(float x, float y) {
	return f(x/64,y/64)*0.5+f(x/37,y/37)*0.3+f(x/17,y/17)*0.14+f(x/7,y/7)*0.06;
}

int grad(float f) {
	f=.5-f/2;
	if (f<0.2f) return 0xffffff;
	if (f>0.8f) return 0x000000;
	if (f<0.4f) return rgb(1.2f-f,1.2f-f,1.6f-3*f);
	if (f<0.6f) return rgb(1.2f-f,2.0f-3*f,1.6f-3*f);
	return rgb(2.4f-3*f,2.0f-3*f,1.6f-3*f);
}

void tile(int tx, int ty, float sc, int * p) {
	for (int y=0; y<TILE_SIZE; y++) {
		for (int x=0; x<TILE_SIZE; x++) {
			p[x+y*TILE_SIZE] = grad(fs(sc*(tx*TILE_SIZE+x), sc*(ty*TILE_SIZE+y)));
		}
	}
}
