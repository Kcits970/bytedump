#include <stdio.h>
#include <string.h>
#include "dump_functions.h"

void exit_from_bad_usage()
{
}

int main(int argc, char **argv)
{
	if (argc != 3)
		goto bad_usage;

	void (*dump_function)(FILE*);

	if (!strcmp("hexencode", argv[1]))
		dump_function = dump_hexencode;

	else if (!strcmp("hexdecode", argv[1]))
		dump_function = dump_hexdecode;

	else if (!strcmp("b64encode", argv[1]))
		dump_function = dump_b64encode;

	else if (!strcmp("b64decode", argv[1]))
		dump_function = dump_b64decode;

	else
		goto bad_usage;	

	FILE *f = fopen(argv[2], "rb");
	if (!f)
	{
		fprintf(stderr, "failed to open the input file\n");
		return 1;
	}

	dump_function(f);
	fclose(f);
	return 0;

bad_usage:
	fprintf(stderr, "usage: ./bytedump hexencode|hexdecode|b64encode|b64decode <file>\n");
	return 1;
}
