php-base85
==========

A PHP extension to encode/decode data with base85.

Overview
--------
This PHP extension provides [RFC1924 Base85](http://tools.ietf.org/html/rfc1924)
encoding and decoding functions.

Example
-------
```php
$str = "The quick brown fox jumps over the lazy dog\n";

$enc = base85_encode($str);
printf("base85-encoded: %s\n", $enc);

$str = base85_decode($enc);
printf("base85-decoded: %s", $str);
```

The above example will output:

```
base85-encoded: RA^-&adl~9Yan8BZ+C7WW^Z^PYISXJb0BYaWpW^NXk{R5VS0HWWN&8*
base85-decoded: The quick brown fox jumps over the lazy dog
```

Where should you use base85?
----------------------------
Base85-encoding is a [binary-to-text encoding](http://en.wikipedia.org/wiki/Binary-to-text_encoding)
used to compactly represent binary data in 7-bit ASCII.
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

About base85 efficiency
-----------------------
Base-85 represents 4 bytes as 5 ASCII characters. This is a 7% improvement
over base-64, which translates to a size increase of ~25% over plain
binary data for base-85 versus that of ~37% for base-64.
  
PHP Functions
-------------
The extension provides two PHP functions:

```
string base85_encode(string $data) - encodes data with base85
string base85_decode(string $data) - decodes data encoded with base85
```

Installation
------------

1. Download the source code
2. Install php build environment (php-devel)
3. Compile and install:

   ```
   $ phpize
   $ ./configure
   $ make install
   ```
4. Optionally, you can run the tests:

   ```
   $ make test
   ```
5. Add the line `extension=base85.so` to your php.ini to trigger it to load on startup.

Alternatives
------------

There is also a pure PHP library [php-base85](https://github.com/scottchiefbaker/php-base85).
That library is easier to install as it does not require the ability to load extensions.

