#ifndef NOISE_H
#define NOISE_H

namespace noise {
	void initialize();
	float noise(int x);
	float noise(int x, int y);
	float noise(int x, int y, int z);
	float noise(float x);
	float noise(float x, float y);
	float noise(float x, float y, float z);
}

inline int fist(float f) {
	int r;
	__asm__ __volatile__ ("fistpl %0" : "=m" (r) : "t" (f) : "st") ;
	return r;
}

#endif
