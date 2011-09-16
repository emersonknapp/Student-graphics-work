#include <iostream>
#include <cmath>


double rand01 (void) {
	return rand() / double(RAND_MAX);
}

double randscreen (void) {
	return (rand() / double(RAND_MAX) * 2) - 1;
}