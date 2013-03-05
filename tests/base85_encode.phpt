--TEST--
base85_encode() function - encode a string with base85
--FILE--
<?php
$string = "The short red fox ran quickly through the green field and jumped over the tall brown bear\n";

$result = base85_encode($string);

echo $result . "\n";
?>
===DONE===
<?php return(0); ?>
--EXPECT--
RA^-&b7*gJbRcqNWFTg5cp!3NZXj`WX=7__c_4IXa&L8KXdrZGWgusAWo2$4W@%+?WFTQ~WFTsFZE$5|Aa8bMav*eQWgv86Y-}K6a&LETAYx@<atZ(d
===DONE===