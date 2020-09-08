#include "debug.h"
#include "parser.h"


void debug_token(FILE * output, struct token * token)
{
	if(token == NULL || token == &eof_token)
		return;
	switch(token->type)
	{
		case TOKEN_INVALID:
			fprintf(output, "[INVALID %c]", token->content.code);
			break;
		case TOKEN_IDENTIFIER:
			fprintf(output, "[IDENTIFIER %s]", token->content.identifier->name);
			break;
		case TOKEN_CHARACTER:
			fprintf(output, "[CHARACTER %c]", token->content.code);
			break;
		case TOKEN_PUNCTUATOR:
			switch(token->content.code)
			{
				case PUNCTUATOR_SEMICOLON:
					fprintf(output, "[PUNCTUATOR ;]");
					break;
				case PUNCTUATOR_AT:
					fprintf(output, "[PUNCTUATOR @]");
					break;
				case PUNCTUATOR_COMMA:
					fprintf(output, "[PUNCTUATOR ,]");
					break;
				case PUNCTUATOR_HYPHEN_LESS:
					fprintf(output, "[PUNCTUATOR ->]");
					break;
				default:
					fprintf(output, "[PUNCTUATOR <unknown>]");
			}
			break;
		default:
			fprintf(output, "<unknown token>");
	}
}
