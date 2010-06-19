#ifndef RGB_H
#define RGB_H

// Color functions.
int rgb(int r, int g, int b);
int rgb(float r, float g, float b);
int rgbu(float r, float g, float b);
int rgbl(float t, int a, int b);

struct Gradient {
	float pos;
	int color;
	inline bool operator< (Gradient &b) {
		return pos < b.pos;
	}
};

/**
 * Calculates the color in the gradient.
 * @param g An array of colors and positions on the gradient.
 *   These positions must be in ascending order.
 * @param length The length of the gradient array.
 * @param pos The position to calculate the color of.
 */
int grad(Gradient*, int length, float pos);

#endif
