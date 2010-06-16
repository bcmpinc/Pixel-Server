#ifndef NOISE_H
#define NOISE_H

namespace noise {
	void initialize();
	float noise(int x);
	float noise(int x, int y);
	float noise(int x, int y, int z);
	float noise(int x, int y, int z, int w);
	float noise(float x);
	float noise(float x, float y);
	float noise(float x, float y, float z);
}

#endif
