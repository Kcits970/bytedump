#include <stdio.h>
#include <stdlib.h>

int hex_decodable(unsigned char c)
{
	if (c >= 'A' && c <= 'F')
		return 1;

	if (c >= 'a' && c <= 'f')
		return 1;

	if (c >= '0' && c <= '9')
		return 1;

	return 0;
}

unsigned char to_nibble(unsigned char c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;

	if (c >= '0' && c <= '9')
		return c - '0';

	return 0xFF;
}

unsigned char to_byte(unsigned char c1, unsigned char c2)
{
	return (to_nibble(c1) << 4) | to_nibble(c2);
}

void dump_hexencode(FILE *f)
{
#define BUFFER_SIZE 1024

	unsigned char buf[BUFFER_SIZE];
	size_t count;

	while ((count = fread(buf, sizeof(unsigned char), BUFFER_SIZE, f)) > 0)
	{
		for (int i = 0; i < count; i++)
			printf("%02x", buf[i]);
	}

#undef BUFFER_SIZE
}

void dump_hexdecode(FILE *f)
{
	unsigned char buf[2];
	unsigned char decoded_byte;
	size_t count;

	int trailing_flag = 0;
	int unexpected_flag = 0;

	while ((count = fread(buf, sizeof(unsigned char), 2, f)) > 0)
	{
		if (count != 2)
			trailing_flag = 1;

		if (!hex_decodable(buf[0]) || !hex_decodable(buf[1]))
			unexpected_flag = 1;

		decoded_byte = to_byte(buf[0], buf[1]);
		fwrite(&decoded_byte, sizeof(unsigned char), 1, stdout);
	}

	if (trailing_flag)
		puts("input file contains trailing characters (possibly a newline)");
	else if (unexpected_flag)
		puts("input file contains unexptected characters");
}
