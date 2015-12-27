#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base64.h"

int main(int argc, char **argv) {
        char *in;
        char *out;
        size_t len;
        char *(* alloc_buffer)(size_t);
        int (* process_base64)(char *, const char*, size_t);

        if(!strcmp(argv[1], "-d")) {
          in = argv[2];
          alloc_buffer = base64_alloc_decode_buffer;
          process_base64 = base64_decode;
        } else {
          if (!strcmp(argv[1], "-e")) {
            in = argv[2];
          } else {
            in = argv[1];
          }
          
          alloc_buffer = base64_alloc_encode_buffer;
          process_base64 = base64_encode;
        }

        len = strlen(in);
        out = alloc_buffer(len);
        process_base64(out, in, len);

	printf("%s\n", out);

	free(out);

	return 0;
}
