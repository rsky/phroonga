--TEST--
grn_ctx_set_match_escalation_threshold() function
--INI--
phroonga.default_match_escalation_threshold = 1000
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);
grn_ctx_set_match_escalation_threshold($ctx, 100);
echo grn_ctx_get_match_escalation_threshold($ctx);
?>
--EXPECT--
100
