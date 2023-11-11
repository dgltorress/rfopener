// Args.h : declarations for args

#include "Args.h"
#include "termcolor.h" // easy console colors, available at https://github.com/ikalnytskyi/termcolor

int Args::checkFlag(const char* arg) {
	for (int i = 0; i < ARG_COUNT; ++i) {
		if (
			(strcmp(arg, FLAGS_SHORTENED[i]) == EQUAL_COMPARE) ||
			(strcmp(arg, FLAGS_WHOLE[i]) == EQUAL_COMPARE)
		) {
		    return i;
        }
	}
	return def;
}
