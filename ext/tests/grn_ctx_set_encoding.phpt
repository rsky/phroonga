--TEST--
grn_ctx_set_encoding() function
--INI--
groonga.default_encoding = GRN_ENC_UTF8
--FILE--
<?php
$ctx = grn_ctx_open();
grn_ctx_set_encoding($ctx, GRN_ENC_LATIN1);
$encoding = grn_ctx_get_encoding($ctx);
if ($encoding === GRN_ENC_LATIN1) {
    echo 'OK';
} else {
    echo "NG: {$encoding}";
}
?>
--EXPECT--
OK
