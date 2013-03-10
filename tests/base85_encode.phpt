--TEST--
base85_encode() function - encode a string with base85
--FILE--
<?php
echo base85_encode("") . "\n";
echo base85_encode("1") . "\n";
echo base85_encode("12") . "\n";
echo base85_encode("123") . "\n";
echo base85_encode("1234") . "\n";
echo base85_encode("The short red fox ran quickly through the green field and jumped over the tall brown bear") . "\n";
echo base85_encode(pack('N', 203)) . "\n";
?>
===DONE===
<?php return(0); ?>
--EXPECT--

F#
F){
F)}j
F)}kW
RA^-&b7*gJbRcqNWFTg5cp!3NZXj`WX=7__c_4IXa&L8KXdrZGWgusAWo2$4W@%+?WFTQ~WFTsFZE$5|Aa8bMav*eQWgv86Y-}K6a&LETAYx@<as
0002X
===DONE===
