#include <stdio.h>
#include <stdlib.h>

unsigned char sextet2b64ascii(unsigned char sextet)
{
	if (sextet >= 0 && sextet < 26)
		return 'A' + sextet;

	if (sextet >= 26 && sextet < 52)
		return 'a' + sextet - 26;

	if (sextet >= 52 && sextet < 62)
		return '0' + sextet - 52;

	if (sextet == 62)
		return '-';

	if (sextet == 63)
		return '+';

	return 0xFF;
}

unsigned char b64ascii2sextet(unsigned char b64ascii)
{
	if (b64ascii >= 'A' && b64ascii <= 'Z')
		return b64ascii - 'A';
	
	if (b64ascii >= 'a' && b64ascii <= 'z')
		return b64ascii - 'a' + 26;
	
	if (b64ascii >= '0' && b64ascii <= '9')
		return b64ascii - '0' + 52;
	
	if (b64ascii == '+')
		return 62;
	
	if (b64ascii == '-')
		return 63;
	
	return 0xFF;
}

int b64_decodable(unsigned char b64ascii)
{
	return b64ascii == '=' || b64ascii2sextet(b64ascii) != 0xFF;
}

void to_quadruplet(size_t triplet_size, unsigned char *triplet, unsigned char *quadruplet)
{
	for (size_t i = triplet_size; i < 3; i++)
		triplet[i] = 0;
	
	quadruplet[0] = sextet2b64ascii(triplet[0] >> 2);
	quadruplet[1] = sextet2b64ascii(((triplet[0] & 0x03) << 4) | (triplet[1] >> 4));
	quadruplet[2] = sextet2b64ascii(((triplet[1] & 0x0F) << 2) | (triplet[2] >> 6));
	quadruplet[3] = sextet2b64ascii(triplet[2] & 0x3F);

	if (triplet_size == 1)
	{
		quadruplet[2] = '=';
		quadruplet[3] = '=';
	}
	
	if (triplet_size == 2)
		quadruplet[3] = '=';
}

void dump_b64encode(FILE *f)
{
	unsigned char triplet_buf[3];
	unsigned char quadruplet_buf[4];
	size_t count;
	
	while ((count = fread(triplet_buf, sizeof(unsigned char), 3, f)) > 0)
	{
		to_quadruplet(count, triplet_buf, quadruplet_buf);
		fwrite(quadruplet_buf, sizeof(unsigned char), 4, stdout);
	}
}

size_t to_triplet(unsigned char *quadruplet, unsigned char *triplet)
{
	size_t pad_count = 0;

	for (size_t i = 0; i < 4; i++)
	{
		if (quadruplet[i] == '=')
			pad_count++;
	}

	if (pad_count > 2)
		return 0;

	for (size_t i = 0; i < 4 - pad_count; i++)
		quadruplet[i] = b64ascii2sextet(quadruplet[i]);

	triplet[0] = (quadruplet[0] << 2) | ((quadruplet[1] & 0x30) >> 4);
	triplet[1] = (quadruplet[1] << 4) | ((quadruplet[2] & 0x3C) >> 2);
	triplet[2] = (quadruplet[2] << 6) | quadruplet[3];

	return 3 - pad_count;
}

void dump_b64decode(FILE *f)
{
	unsigned char triplet_buf[3];
	unsigned char quadruplet_buf[4];
	int trailing_flag = 0;
	int unexpected_flag = 0;

	size_t count;
	while ((count = fread(quadruplet_buf, sizeof(unsigned char), 4, f)) > 0)
	{
		if (count < 4)
			trailing_flag = 1;
		
		for (size_t i = 0; i < 4; i++)
		{
			if (!b64_decodable(quadruplet_buf[i]))
				unexpected_flag = 1;
		}

		size_t triplet_size = to_triplet(quadruplet_buf, triplet_buf);
		if (!triplet_size)
			unexpected_flag = 1;

		fwrite(triplet_buf, sizeof(unsigned char), triplet_size, stdout);
	}
	
	if (trailing_flag)
		fprintf(stderr, "input file contains trailing characters (possibly a newline)\n");
	else if (unexpected_flag)
		fprintf(stderr, "input file contains unexptected characters\n");
}
