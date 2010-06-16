#include <cstdlib>

#include "noise.h"
#include "float.h"

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
		int j = rand() & (NOISE_LENGTH-1);
		p[i]^=p[j]^=p[i]^=p[j];
	}
}

// Noise functions
#define P(x) (p[((x))&(NOISE_LENGTH-1)])
#define G(x) (g[((x))&(NOISE_LENGTH-1)])
float noise(int x) { return G(x); }
float noise(int x, int y) { return G(x + P(y)); }
float noise(int x, int y, int z) { return G(x + P(y + P(z))); }
float noise(int x, int y, int z, int w) { return G(x + P(y + P(z + P(w)))); }

float noise(float x) {
	int   px = fist(x);
	      x -= px;    

	float d = noise(px-1);
	float e = noise(px  );
	float f = noise(px+1);
	
	x*=2;
	return  lerp(x,d,e,f);
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
	float k=lerp(x,a,b,c);
	float l=lerp(x,d,e,f);
	float m=lerp(x,g,h,i);
	
	return  lerp(y,k,l,m);
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
	float k=lerp(x,a,b,c);
	float l=lerp(x,d,e,f);
	float m=lerp(x,g,h,i);
	
	return  lerp(y,k,l,m)/(7+sz);
}


/*{*/}


