--TEST--
grn_ctx_get_match_escalation_threshold() function
--INI--
phroonga.default_match_escalation_threshold = 1000
--FILE--
<?php
$ctx = grn_ctx_open(GRN_CTX_USE_QL);
echo grn_ctx_get_match_escalation_threshold($ctx);
?>
--EXPECT--
1000
