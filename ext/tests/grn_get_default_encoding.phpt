--TEST--
grn_get_default_encoding() function
--INI--
groonga.default_encoding = GRN_ENC_LATIN1
--FILE--
<?php
$encoding = grn_get_default_encoding();
if ($encoding === GRN_ENC_LATIN1) {
    echo 'OK';
} else {
    echo "NG: {$encoding}";
}
?>
--EXPECT--
OK
