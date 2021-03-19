#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "backprop.h"


void train(NN* nn);
void test(NN* nn, int num_samples = 10);
int ns_test();