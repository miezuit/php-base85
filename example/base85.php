<?php

$str = "The quick brown fox jumps over the lazy dog\n";

$encoded = base85_encode($str);
printf("base85-encoded: %s\n", $encoded);

$decoded = base85_decode($encoded);
printf("base85-decoded: %s", $decoded);

$str = base85_encode(pack("N", 203));
printf("binary string base85-encoded: %s\n", $str);

$str = unpack("N", base85_decode($str));
printf("binary string base85-decoded: %s\n", $str[1]);
