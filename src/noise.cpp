#include <cstdlib>

#include "noise.h"

namespace noise {/*}*/

// Static data
#define NOISE_LENGTH 512
int p[NOISE_LENGTH];
float g[NOISE_LENGTH];

void initialize() {
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

// Noise functions
float noise(int x) { return g[x & 0xff]; }
float noise(int x, int y) { return g[(x + p[y & 0xff]) & 0xff]; }
float noise(int x, int y, int z) { return g[(x + p[(y + p[z & 0xff]) & 0xff]) & 0xff]; }

inline float lerp3(float t, float a, float b, float c) {
	float st = t * t;
	return (c*(st+2*t+1) + b*(5-st) + a*(st-2*t+1)) / (7+st);
}

float noise(float x) {
	int   px = fist(x);
	      x -= px;    

	float d = noise(px-1);
	float e = noise(px  );
	float f = noise(px+1);
	
	x*=2;
	return  lerp3(x,d,e,f);
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

float noise(float x, float y, float z) {
	int   px = fist(x), py = fist(y), pz = fist(z);
	      x -= px;      y -= py;      z -= pz;
	float sz = z * z;
	float p = sz+2*z+1;
	float q = 5-sz;
	float r = sz-2*z+1;

	float a = p*noise(px-1, py-1, pz-1) + q*noise(px-1, py-1, pz  ) + r*noise(px-1, py-1, pz+1);
	float b = p*noise(px  , py-1, pz-1) + q*noise(px  , py-1, pz  ) + r*noise(px  , py-1, pz+1);
	float c = p*noise(px+1, py-1, pz-1) + q*noise(px+1, py-1, pz  ) + r*noise(px+1, py-1, pz+1);
	float d = p*noise(px-1, py  , pz-1) + q*noise(px-1, py  , pz  ) + r*noise(px-1, py  , pz+1);
	float e = p*noise(px  , py  , pz-1) + q*noise(px  , py  , pz  ) + r*noise(px  , py  , pz+1);
	float f = p*noise(px+1, py  , pz-1) + q*noise(px+1, py  , pz  ) + r*noise(px+1, py  , pz+1);
	float g = p*noise(px-1, py+1, pz-1) + q*noise(px-1, py+1, pz  ) + r*noise(px-1, py+1, pz+1);
	float h = p*noise(px  , py+1, pz-1) + q*noise(px  , py+1, pz  ) + r*noise(px  , py+1, pz+1);
	float i = p*noise(px+1, py+1, pz-1) + q*noise(px+1, py+1, pz  ) + r*noise(px+1, py+1, pz+1);
	
	x*=2;
	y*=2;
	float k=lerp3(x,a,b,c);
	float l=lerp3(x,d,e,f);
	float m=lerp3(x,g,h,i);
	
	return  lerp3(y,k,l,m)/(7+sz);
}


/*{*/}


