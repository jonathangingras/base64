#include <string.h>
#include <stdlib.h>
#include "base64.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  ((byte) & 0x80 ? 1 : 0), \
  ((byte) & 0x40 ? 1 : 0), \
  ((byte) & 0x20 ? 1 : 0), \
  ((byte) & 0x10 ? 1 : 0), \
  ((byte) & 0x08 ? 1 : 0), \
  ((byte) & 0x04 ? 1 : 0), \
  ((byte) & 0x02 ? 1 : 0), \
  ((byte) & 0x01 ? 1 : 0)

static char base64_encode_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
	'U', 'V', 'W', 'X', 'Y', 'Z', 
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
	'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'+', '/'
};

static char base64_decode_table[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	62, //'+'

	0, 0, 0, 

	63, //'/' 

	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0' to '9'

	0, 0, 0, 0, 0, 0, 0,

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, //'A' to 'Z'

	0, 0, 0, 0, 0, 0,

	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 //'a' to 'z'
};

/* logical right shift */
#define lrs(x, y) ((char)( ( ((x) >> y) | (0b11111111 << (8 - y)) ) ^ (0b11111111 << (8 - y)) ))

static void base64_encode_chunk(char *out, const char *_str) {
	out[0] = base64_encode_table[lrs(_str[0], 2)];
	out[1] = base64_encode_table[lrs(_str[0] << 6, 2) | lrs(_str[1], 4)];
	out[2] = base64_encode_table[lrs(_str[1] << 4, 2) | lrs(_str[2], 6)];
	out[3] = base64_encode_table[lrs(_str[2] << 2, 2)];
}

/* next multiple of y from x */
#define next_multiple(x, y) (x + (y - (x % y)))

static int base64_encode_one(char *out, const char *in) {
  out[0] = base64_encode_table[lrs(in[0], 2)];
  out[1] = base64_encode_table[lrs((in[0] << 6), 2)];
  out[2] = '=';
  out[3] = '=';
  
  return 0;
}

static int base64_encode_two(char *out, const char *in) {
  out[0] = base64_encode_table[lrs(in[0], 2)];
  out[1] = base64_encode_table[lrs((in[0] << 6), 2) | (in[1] >> 4)];
  out[2] = base64_encode_table[lrs((in[1] << 4), 2)];
  out[3] = '=';

  return 0;
}

int base64_encode(char *out, const char *in, size_t size) {
        if(size == 1) {
                return base64_encode_one(out, in);
        }
        if(size == 2) {
                return base64_encode_two(out, in);
        }

	size_t size_ = size;
	int padding = 0;
	if(size % 3) {
		size_ = next_multiple(size, 3);
		padding = next_multiple(size, 3) - size;
	}

	int i; char last_bytes[3] = {0, 0, 0};
	for(i = 0; i < (size_/3) - 1; ++i) {
		base64_encode_chunk(out + (i * 4), in + (i * 3));
	}
	memcpy(last_bytes, in + (3 * ((size_/3) - 1)), 3 - (size_ - size));
	base64_encode_chunk(out + (4 * ((size_/3) - 1)), last_bytes);

	if(padding) {
		out[((size_/3) * 4) - 1] = '=';
		if(padding == 2) {
			out[((size_/3) * 4) - 2] = '=';
		}
	}

	return 0;
}

static void base64_decode_chunk(char* out, const char *in) {
	char i0, i1, i2, i3;

	i0 = base64_decode_table[in[0]];
	i1 = base64_decode_table[in[1]];
	i2 = base64_decode_table[in[2]];
	i3 = base64_decode_table[in[3]];

	out[0] = (i0 << 2) | lrs(i1, 4);
	out[1] = (i1 << 4) | lrs(i2, 2);
	out[2] = (i2 << 6) | i3;
}

int base64_decode(char* out, const char *in, size_t size) {
	if(size % 4) {
		return -1;
	}

	int i;
	for(i = 0; i < (size/4) + 2; ++i) {
		base64_decode_chunk(out + (i * 3), in + (i * 4));
	}

	return 0;
}

size_t base64_encoded_size(size_t size) {
	return (next_multiple(size, 3) / 3) * 4;
}

size_t base64_decoded_size(size_t size) {
	if(size % 4) {
		return 0;
	}

	return (size / 4) * 3;
}

char *base64_alloc_encode_buffer(size_t input_size) {
  size_t encoded_size = base64_encoded_size(input_size);
  char *buffer = malloc(encoded_size + 1);
  buffer[encoded_size] = 0;
  return buffer;
}

char *base64_alloc_decode_buffer(size_t input_size) {
  size_t decoded_size = base64_decoded_size(input_size);
  char *buffer = malloc(decoded_size + 1);
  buffer[decoded_size] = 0;
  return buffer;
}

