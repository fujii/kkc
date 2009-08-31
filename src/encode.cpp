#include "encode.hpp"
#include <string.h>
#include <stdlib.h>
#include <iconv.h>
#include <errno.h>

int convert_text(iconv_t cd,
		 char *inbuf, size_t inbytes,
		 char **outbuf, size_t *outbytes)
{
    const size_t BUF_SIZE = 256;
    char *src_buf = inbuf;
    size_t src_len = inbytes;
    char *dst_buf;
    size_t dst_len;
    char *buf, *buf_new;
    size_t buf_len = BUF_SIZE;

    *outbuf = 0;
    *outbytes = 0;

    buf = (char *)malloc(buf_len);
    if (!buf)
	return -1;
    dst_buf = buf;
    dst_len = buf_len;
    size_t n;
    while (src_len) {
	n = iconv(cd, &src_buf, &src_len, &dst_buf, &dst_len);
	if (n == -1) {
	    switch (errno) {
	    case E2BIG:
		buf_new = (char *)realloc(buf, buf_len + BUF_SIZE);
		if (!buf_new) {
		    free(buf);
		    return -1;
		}
		dst_buf = dst_buf - buf + buf_new;
		dst_len += BUF_SIZE;
		buf = buf_new;
		buf_len += BUF_SIZE;
		break;
	    default:
		free(buf);
		return -2;
	    }
	}
    }
    *outbuf = buf;
    *outbytes = dst_buf - buf;
    return 0;
}

int convert_utf8_to_ucs4(const char *str, char **outbuf, size_t *outlen)
{
    iconv_t cd = iconv_open("UCS-4LE", "UTF-8");
    if (cd == iconv_t(-1))
	return -1;
    int err = convert_text(cd, const_cast<char*>(str), strlen(str), outbuf, outlen);
    iconv_close(cd);
    return err;
}

int convert_eucjp_to_ucs4(const char *str, char **outbuf, size_t *outlen)
{
    iconv_t cd = iconv_open("UCS-4LE", "EUC-JP");
    if (cd == iconv_t(-1))
	return -1;
    int err = convert_text(cd, const_cast<char*>(str), strlen(str), outbuf, outlen);
    iconv_close(cd);
    return err;
}

int convert_ucs4_to_utf8(const char *str, size_t str_len, char **outbuf, size_t *outlen)
{
    iconv_t cd = iconv_open("UTF-8", "UCS-4LE");
    if (cd == iconv_t(-1))
	return -1;
    int err = convert_text(cd, const_cast<char*>(str), str_len, outbuf, outlen);
    iconv_close(cd);
    return err;
}

