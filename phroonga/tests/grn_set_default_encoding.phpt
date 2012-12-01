--TEST--
grn_set_default_encoding() function
--FILE--
<?php
$rc = grn_set_default_encoding(GRN_ENC_EUC_JP);
if ($rc !== GRN_SUCCESS) {
    echo "NG: rc = {$rc}";
} else {
    echo grn_get_default_encoding();
}
?>
--EXPECT--
2
