#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* cpp_strdup(const char* src) {
	if (!src) return nullptr;
	size_t len = strlen(src) + 1;
	char* dest = new char[len];
	strcpy(dest, src);
	return dest;
}
