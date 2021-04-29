#ifndef GETLINE_WIN32_H
#define GETLINE_WIN32_H

#include <stdio.h>

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif	// _WIN32

#endif	// GETLINE_WIN32_H
