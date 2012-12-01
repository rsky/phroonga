--TEST--
grn_get_version() function
--FILE--
<?php
echo grn_get_version();
?>
--EXPECTREGEX--
^[0-9]+\.[0-9]+\.[0-9]+$
