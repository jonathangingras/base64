#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <stdlib.h>

int base64_encode(char *out, const char *in, size_t size);
int base64_decode(char* out, const char *in, size_t size);
size_t base64_encoded_size(size_t size);
size_t base64_decoded_size(size_t size);
char *base64_alloc_encode_buffer(size_t input_size);
char *base64_alloc_decode_buffer(size_t input_size);

#endif
