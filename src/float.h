#ifndef FLOAT_H
#define FLOAT_H

/**
 * Fast round to integer function.
 * Requires intel architecture. (afaik)
 */
inline int fist(float f) {
	int r;
	__asm__ __volatile__ ("fistpl %0" : "=m" (r) : "t" (f) : "st") ;
	return r;
}

/**
 * Cubic interpolation function.
 * It calculates a weighted mean of the three input values. 
 * The weights are:
 *  a: t^2+2t+1
 *  b: 5-t^2
 *  c: t^2-2t+1
 *
 * It has the following properties:
 *  - lerp3(-1,a,b,c) = .5*(a+b)
 *  - lerp3( 1,a,b,c) = .5*(b+c)
 *  - lerp3(x,a,b,c) + lerp3(x,d,e,f) = lerp3(x,a+d,b+e,c+f)
 *  - lerp3(x,a,b,c) = lerp(-x,c,b,a)
 */
inline float lerp(float t, float a, float b, float c) {
	float st = t * t;
	return (c*(st+2*t+1) + b*(5-st) + a*(st-2*t+1)) / (7+st);
}
/**
 * Linear interpolation function
 */
inline float lerp(float t, float a, float b) {
	return ((1-t)*a+(1+t)*b)*.5;
}

#endif
