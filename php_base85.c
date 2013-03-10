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
  string base85_encode(string $data) - encodes a string using RFC1924 base85
  string base85_decode(string $data) - decodes a string encoded with RFC1924 base85
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

static const char ZERO_BYTE = 0;
static const char DECODING_ERROR = -1;

/************************
* function declarations *
************************/
int encode_85(char *buf, const unsigned char *data, int bytes);
int decode_85(char *dst, const char *buffer, int len);
void _copy_with_padding(char *dest, const char *src, int len, int padding_size, char padding_char);

/* {{{ proto string base85_encode(string data)
 */
PHP_FUNCTION(base85_encode)
{
    char *data, *encoded;
    int data_len, encoded_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
        RETURN_NULL();
    }
    
    /* base85 encodes 4 bytes into 5 bytes;
       therefore the required buffer size is: ceil(data_len * 5/4) + 1 (for the last null byte)
    */
    encoded = emalloc(( (int) (data_len / 4) * 5 + 6) * sizeof(char));
    
    encoded_len = encode_85(encoded, data, data_len);
    
    RETURN_STRINGL(encoded, encoded_len, 1);
    
    efree(encoded);
}
/* }}} */

/* {{{ proto string base85_dencode(string data)
 */
PHP_FUNCTION(base85_decode)
{
    char *data, *decoded;
    int data_len, decoded_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
        RETURN_NULL();
    }    
    
    decoded = emalloc(data_len * sizeof(char));
    
    decoded_len = decode_85(decoded, data, data_len);
    
    if(decoded_len != DECODING_ERROR) {
        RETVAL_STRINGL(decoded, decoded_len, 1);
    } else {
        RETURN_NULL();
    }

    efree(decoded);
}
/* }}} */

static const char base85_alphabet[] = {
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

int encode_85(char* encoded, const unsigned char *data, int len)
{
    /* Base85 encodes 4-byte chunks at a time.
       Which means the lenght of the byte sequence must be divisible by 4.
       If the last block of source bytes contains fewer than 4 bytes,
       the block is padded with up to three null bytes before encoding.
     */
    unsigned char *buffer;
    int buffer_len, padding_size = 0;
    
    int remainder = len % 4;
    if (remainder) {
        padding_size = 4 - remainder;
    }
    
    buffer_len = len + padding_size;
    buffer = emalloc(buffer_len * sizeof(char));

    // copy input data into buffer
    _copy_with_padding(buffer, data, len, padding_size, ZERO_BYTE);
    
    int i;
    /* encode 4-byte chunks */
    for (i = 0; i < buffer_len; i += 4) {
        unsigned acc = 0;
        int cnt;
        
        /* string to int */
        for (cnt = 0; cnt < 4; cnt++) {
            unsigned ch = buffer[i + cnt];
            acc |= ch << ((3 - cnt) * 8);
        }
        /* int to base85 */
        for (cnt = 4; cnt >= 0; cnt--) {
            int val = acc % 85;
            acc /= 85;
            encoded[cnt] = base85_alphabet[val];
        }
        encoded += 5;
    }

    // as much padding added before encoding is removed after encoding
    encoded[-padding_size] = ZERO_BYTE;
    
    // cleanup
    efree(buffer);
    
    return (buffer_len / 4) * 5 - padding_size;
}

int decode_85(char *decoded, const char *data, int len)
{
    /* prapare base85 decoding */
    char base85_ords[256];
    int buffer_len, padding_size = 0;
    char *buffer;
    
    int i;
    for (i = 0; i < 85; i++) {
        int ch = base85_alphabet[i];
        base85_ords[ch] = i + 1;
    }    
    
   /* We want 5-tuple chunks, so pad with as many 84 characters
    * to satisfy the length.
    */    
    int remainder = len % 5;
    if (remainder) {
        padding_size = 5 - remainder;
    }
    
    buffer_len = len + padding_size;
    buffer = emalloc(buffer_len * sizeof(char));

    // copy input data into buffer
    _copy_with_padding(buffer, data, len, padding_size, base85_alphabet[84]); // 84 = "u"(ASCII85); "~"(RFC1924)

    /* decode 5-byte chunks */
    for (i = 0; i < buffer_len; i += 5) {
        unsigned acc = 0;
        int de, cnt;
        unsigned char ch;
        
        /* base85 to int */
        for (cnt = 0; cnt < 5; cnt++) {
            ch = buffer[i + cnt];
            de = base85_ords[ch];
            if (--de < 0) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid base85 alphabet %c at offset %d", ch, i + cnt);
                return DECODING_ERROR;
            }
            acc = acc * 85 + de;
        }
        /* Detect overflow. Groups of characters that decode to a value greater than 2**32 − 1
         * will cause a decoding error.
         */
        if (acc >= 0xffffffff) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid base85 sequence %.5s", buffer - 5);
            return DECODING_ERROR;
        }
        /* int to string */
        for (cnt = 0; cnt < 4; cnt++) {
            /* shift forward the cnt-th char */
            acc = (acc << 8) | (acc >> 24);
            *decoded++ = acc;
        }
    }

    // as much padding added before encoding is removed after encoding
    decoded[-padding_size] = 0;

    // cleanup
    efree(buffer);
    
    return (buffer_len / 5) * 4 - padding_size;
}

void _copy_with_padding(char *dest, const char *src, int len, int padding_size, char padding_char)
{
    // copy input data into buffer
    int i;
    for (i = 0; i < len; i++) {
        dest[i] = src[i];
    }
    // add padding
    for (i = len; i < len + padding_size; i++) {
        dest[i] = padding_char;
    }
}
