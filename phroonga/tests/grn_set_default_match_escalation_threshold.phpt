--TEST--
grn_set_default_match_escalation_threshold() function
--FILE--
<?php
$rc = grn_set_default_match_escalation_threshold(1234567890);
if ($rc !== GRN_SUCCESS) {
    echo "NG: rc = {$rc}";
} else {
    echo grn_get_default_match_escalation_threshold();
}
?>
--EXPECT--
1234567890
