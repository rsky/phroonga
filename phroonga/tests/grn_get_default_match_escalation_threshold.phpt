--TEST--
grn_get_default_match_escalation_threshold() function
--FILE--
<?php
echo grn_get_default_match_escalation_threshold();
?>
--EXPECTREGEX--
^(0|[1-9][0-9]*)$
