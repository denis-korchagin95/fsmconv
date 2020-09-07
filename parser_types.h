#ifndef NFA2DFACONV_PARSER_TYPES_H
#define NFA2DFACONV_PARSER_TYPES_H 1

struct token
{
    int type;
    /* TODO: location in the source */
    union {
        int code;
	char identifier[256];
    } content;
};

#endif /* NFA2DFACONV_PARSER_TYPES_H */
