#include "mathlib.h"

bool around_multiple(mathld_t x, mathld_t axis, mathld_t toler) {
	axis = absolute(axis);
    if (axis < constants::PRECISE_TOLERANCE) throw;
	x = absolute(x);
    mathld_t multi = round(x / axis, 0) * axis;
	return absolute(x - multi) < toler;
}

mathld_t arcsine(mathld_t x) {
    if (x > 1 || x < -1) throw;
	mathld_t y = x;
	mathld_t tmp = x;
	mathul_t N = 0;
	while (++N < constants::TAYLOR_CONVERGENCE) {
		tmp *= (x * x * (2 * N - 1) * (2 * N - 1)) / ((2 * N + 1) * (2 * N));
		y += tmp;
	}
	return y;
}
