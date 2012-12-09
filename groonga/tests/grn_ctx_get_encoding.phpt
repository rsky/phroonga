--TEST--
grn_ctx_get_encoding() function
--INI--
groonga.default_encoding = GRN_ENC_LATIN1
--FILE--
<?php
$ctx = grn_ctx_open();
$encoding = grn_ctx_get_encoding($ctx);
if ($encoding === GRN_ENC_LATIN1) {
    echo 'OK';
} else {
    echo "NG: {$encoding}";
}
?>
--EXPECT--
OK
