--TEST--
grn_ctx_use() function
--FILE--
<?php
error_reporting(E_ALL);
$ctx = grn_ctx_open();
$db1 = grn_db_open($ctx, null);
$db2 = grn_db_open($ctx, null);
if (grn_ctx_use($ctx, $db1)) {
    echo "1\n";
}
if (grn_ctx_use($ctx, $db1)) {
    echo "2\n";
}
if (grn_ctx_use($ctx, null)) {
    echo "0\n";
}
?>
--EXPECT--
1
2
0
