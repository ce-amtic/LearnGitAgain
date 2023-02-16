#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef __TEST_FUNCTION_H
#define __TEST_FUNCTION_H

void init();

double f (const char *file_path);

bool check_ans (double u_a, double u_b, double u_c, unsigned int u_random_seed);

void debug_mode(unsigned int random_seed);

#endif