#include "getline_win32.h"
#include <stdlib.h>

#ifdef _WIN32
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
	if (!lineptr || !n || !stream)
		return -1;
	if (*n == 0)
	{
		if (!*lineptr)
		{
			*lineptr = (char*)malloc(16);
			if (*lineptr)
				*n = 16;
			else
				return -1;
		}
		else
			return -1;
	}
	char c;
	ssize_t count = 0;
	size_t ret;
	char* ptr = *lineptr;
	while (1)
	{
		ret = fread(&c, 1, 1, stream);
		if (ret != 1)
			break;
		if (*n <= count + 1)
		{
			char* tmp = (char*)realloc(*lineptr, *n + 16);
			if (tmp)
			{
				*lineptr = tmp;
				*n += 16;
				ptr = *lineptr + count;
			}
			else
			{
				// memory error
				count = -1;
				break;
			}
		}
		*ptr = c;
		ptr++;
		count++;
		if (c == '\n')
			break;
	}
	*ptr = 0;
	return count;
}
#endif
