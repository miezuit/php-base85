--TEST--
base85_dencode() function - decode a base85-encoded string
--FILE--
<?php
$string = 'RA^-&b7*gJbRcqNWFTg5cp!3NZXj`WX=7__c_4IXa&L8KXdrZGWgusAWo2$4W@%+?WFTQ~WFTsFZE$5|Aa8bMav*eQWgv86Y-}K6a&LETAYx@<atZ(d';

$result = base85_decode($string);

echo $result;
?>
===DONE===
<?php return(0); ?>
--EXPECT--
The short red fox ran quickly through the green field and jumped over the tall brown bear
===DONE===