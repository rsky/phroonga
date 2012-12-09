--TEST--
grn_set_default_ctx() function
--FILE--
<?php
$ctx1 = grn_ctx_open();
if (grn_get_default_ctx() === $ctx1) {
    echo "1:OK\n";
} else {
    echo "2:NG\n";
}
$ctx2 = grn_ctx_open();
grn_set_default_ctx($ctx2);
if (grn_get_default_ctx() === $ctx2) {
    echo "2:OK\n";
} else {
    echo "2:NG\n";
}
?>
--EXPECT--
1:OK
2:OK
