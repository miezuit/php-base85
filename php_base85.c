/*
  +----------------------------------------------------------------------+
  | BASE85                                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Radu Gasler <rgasler@streamwide.com>                        |
  +----------------------------------------------------------------------+

  This extension provides RFC1924 Base85 encoding and decoding functions:
  see http://tools.ietf.org/html/rfc1924

  Where should you use base85?
  ----------------------------
  Base85-encoding is used to compactly represent binary data in 7-bit ASCII.
  It is, therefore, 7-bit MIME-safe but not safe to use in URLs, SGML, HTTP
  cookies, and other similar places. Example scenarios where Base85 encoding
  can be put to use are Adobe PDF documents, Adobe PostScript format, binary
  diffs (patches), efficiently storing RSA keys, etc.

  The ASCII85 character set-based encoding is mostly used by Adobe PDF and
  PostScript formats. It may also be used to store RSA keys or binary data
  with a lot of zero byte sequences. The RFC1924 character set-based encoding,
  however, may be used to compactly represent 128-bit unsigned integers (like
  IPv6 addresses) or binary diffs. Encoding based on RFC1924 does not compact
  zero byte sequences, so this form of encoding is less space-efficient than
  the ASCII85 version which compacts redundant zero byte sequences.

  About base85 efficency
  ----------------------
  Base-85 represents 4 bytes as 5 ASCII characters. This is a 7% improvement
  over base-64, which translates to a size increase of ~25% over plain
  binary data for base-85 versus that of ~37% for base-64.
  
  Functions
  ---------
  string base85_encode(string $data) - encodes a string using base85
  string base85_decode(string $data) - decodes a string encoded with base85
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_base85.h"

static zend_function_entry base85_functions[] = {
    PHP_FE(base85_encode, NULL)
    PHP_FE(base85_decode, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry base85_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_BASE85_EXTNAME,
    base85_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_BASE85_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BASE85
ZEND_GET_MODULE(base85)
#endif

/************************
* function declarations *
************************/
void encode_85(char *buf, const unsigned char *data, int bytes);
int decode_85(char *dst, const char *buffer, int len);

/* {{{ proto string base85_encode(string data)
 */
PHP_FUNCTION(base85_encode)
{
    char *data, *buf;
    int data_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
        RETURN_NULL();
    }
    
    /* base85 encodes 4 bytes into 5 bytes;
       therefore the required buffer size is: ceil(data_len * 5/4) + 1 (for the last null byte) 
    */
    buf = emalloc(((int) (data_len * 1.25) + 2) * sizeof(char));
    
    encode_85(buf, data, data_len);
    
    RETURN_STRING(buf, 1);
    
    efree(buf);
}
/* }}} */

/* {{{ proto string base85_dencode(string data)
 */
PHP_FUNCTION(base85_decode)
{
    char *data, *buf;
    int data_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
        RETURN_NULL();
    }    
    
    buf = emalloc((data_len + 1) * sizeof(char));
    
    if(0 == decode_85(buf, data, data_len)) {
        RETURN_STRING(buf, 1);
    } else {
        RETURN_NULL();
    }

    efree(buf);    
}
/* }}} */

static const char en85[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z',
    '!', '#', '$', '%', '&', '(', ')', '*', '+', '-',
    ';', '<', '=', '>', '?', '@', '^', '_', '`', '{',
    '|', '}', '~'
};

void encode_85(char *buffer, const unsigned char *data, int len)
{
    while (len) {
        unsigned acc = 0;
        int cnt;
        /* Pack a 4-character block */
        for (cnt = 24; cnt >= 0; cnt -= 8) {
            unsigned ch = *data++;
            acc |= ch << cnt;
            if (--len == 0)
                break;
        }
        for (cnt = 4; cnt >= 0; cnt--) {
            int val = acc % 85;
            acc /= 85;
            buffer[cnt] = en85[val];
        }
        buffer += 5;
    }

    *buffer = 0;
}

int decode_85(char *buffer, const char *data, int len)
{
    /* prapare base85 decoding */
    char de85[256];
    int offset = 0;
    
    int i;
    for (i = 0; i < 85; i++) {
        int ch = en85[i];
        de85[ch] = i + 1;
    }

    /* start decoding */
    while (len > 0) {
        unsigned acc = 0;
        int de, cnt;
        unsigned char ch;
        /* Unpack a 5-character block */
        for (cnt = 4; cnt >= 0; cnt--) {
            ch = *data++;
            offset++;
            de = de85[ch];
            if (--de < 0) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid base85 alphabet %c at offset %d", ch, offset);
                return 1;
            }
            if (cnt) {
                /* except for the last char */
                acc = acc * 85 + de;
            }
        }
        /* Detect overflow. */
        if (0xffffffff / 85 < acc ||
            0xffffffff - de < (acc *= 85)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid base85 sequence %.5s", data - 5);
            return 1;
        }
        acc += de;

        cnt = (len < 5) ? len : 4;
        len -= 5;
        do {
            acc = (acc << 8) | (acc >> 24);
            *buffer++ = acc;
        } while (--cnt);
    }

    *buffer = 0;
    
    return 0;
}
