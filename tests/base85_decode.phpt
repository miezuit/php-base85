--TEST--
base85_decode() function - decode a string encoded with base85
--FILE--
<?php
echo base85_decode("") . "\n";
echo base85_decode("F#") . "\n";
echo base85_decode("F){") . "\n";
echo base85_decode("F)}j") . "\n";
echo base85_decode("F)}kW") . "\n";
echo base85_decode("RA^-&b7*gJbRcqNWFTg5cp!3NZXj`WX=7__c_4IXa&L8KXdrZGWgusAWo2$4W@%+?WFTQ~WFTsFZE$5|Aa8bMav*eQWgv86Y-}K6a&LETAYx@<as") . "\n";
$unpacked = unpack('N', base85_decode("0002X"));
echo $unpacked[1] . "\n";
?>
===DONE===
<?php return(0); ?>
--EXPECT--

1
12
123
1234
The short red fox ran quickly through the green field and jumped over the tall brown bear
203
===DONE===
