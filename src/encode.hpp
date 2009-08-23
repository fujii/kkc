#include <stddef.h>

int convert_utf8_to_ucs4(const char *str, char **outbuf, size_t *outlen);
int convert_eucjp_to_ucs4(const char *str, char **outbuf, size_t *outlen);
int convert_ucs4_to_utf8(const char *str, size_t str_len, char **outbuf, size_t *outlen);

