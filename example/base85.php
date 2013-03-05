<?php

$str = "The quick brown fox jumps over the lazy dog\n";

$encoded = base85_encode($str);
printf("base85-encoded: %s\n", $encoded);

$decoded = base85_decode($encoded);
printf("base85-decoded: %s", $decoded);
