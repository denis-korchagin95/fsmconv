#ifndef FSMCONV_STREAM_H
#define FSMCONV_STREAM_H 1

#define STREAM_CHAR_STACK_SIZE (4)

#include <stdio.h>

struct source_location
{
	unsigned int line;
	unsigned int column;
};

struct stream
{
	struct source_location location;
	char * name;
	FILE * file;
	char char_stack[STREAM_CHAR_STACK_SIZE];
	unsigned int char_stack_pos;
};

extern int stream_init(struct stream * stream, const char * pathname);
extern void stream_destroy(struct stream * stream);

extern int stream_getchar(struct stream * stream);
extern void stream_ungetchar(struct stream * stream, int ch);

#endif /* FSMCONV_STREAM_H */
