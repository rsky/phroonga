--TEST--
grn_get_default_match_escalation_threshold() function
--INI--
phroonga.default_match_escalation_threshold = 1000
--FILE--
<?php
echo grn_get_default_match_escalation_threshold();
?>
--EXPECT--
1000
