--TEST--
grn_ctx_set_match_escalation_threshold() function will fail
--INI--
phroonga.default_match_escalation_threshold = 1000
--FILE--
<?php
$ctx = grn_ctx_open();
$result = @grn_ctx_set_match_escalation_threshold($ctx, 100);
if ($result === false) {
    echo 'FAILURE';
} else {
    echo 'SUCCESS';
}
?>
--EXPECT--
FAILURE
