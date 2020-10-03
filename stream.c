#include "stream.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

int stream_init(struct stream * stream, const char * pathname)
{
	if(! (stream->file = fopen(pathname, "r")))
		return -1;
	stream->location.line = 0;
	stream->location.column = 0;
	stream->name = (char *) pathname;
	stream->char_stack_pos = 0;
	stream->char_stack[0] = '\0';
	return 0;
}

void stream_destroy(struct stream * stream)
{
	if(stream->file) {
		fclose(stream->file);
		stream->file = NULL;
	}
}

int stream_getchar(struct stream * stream)
{
	if(stream->char_stack_pos > 0)
		return (int) stream->char_stack[--stream->char_stack_pos];
	int ch = fgetc(stream->file);
	switch(ch)
	{
		case '\n':
			{
				stream->location.line++;
				stream->location.column = 0;
			}
			break;
		case '\t':
			{
				stream->location.column = TAB_SIZE - (stream->location.column - 1) % TAB_SIZE;
			}
			break;
	}
	if(ch != EOF)
		++stream->location.column;
	return ch;
}

void stream_ungetchar(struct stream * stream, int ch)
{
	if(stream->char_stack_pos >= STREAM_CHAR_STACK_SIZE) {
		fprintf(stderr, "error: char stack is overflow!\n");
		exit(EXIT_FAILURE);
	}
	if(ch != EOF)
		stream->char_stack[stream->char_stack_pos++] = (char) ch;
}
