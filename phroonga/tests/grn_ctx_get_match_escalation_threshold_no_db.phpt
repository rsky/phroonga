--TEST--
grn_ctx_get_match_escalation_threshold() function returns 0
--INI--
phroonga.default_match_escalation_threshold = 1000
--FILE--
<?php
$ctx = grn_ctx_open();
echo grn_ctx_get_match_escalation_threshold($ctx);
?>
--EXPECT--
0
