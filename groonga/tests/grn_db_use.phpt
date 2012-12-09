--TEST--
grn_db_use() function
--FILE--
<?php
error_reporting(E_ALL);
$ctx = grn_ctx_open();
$db1 = grn_db_open($ctx, null);
$db2 = grn_db_open($ctx, null);
if (grn_db_use($db1) && grn_ctx_db($ctx) === $db1) {
    echo "1:OK\n";
} else {
    echo "1:NG\n";
}
if (grn_db_use($db2) && grn_ctx_db($ctx) === $db2) {
    echo "2:OK\n";
} else {
    echo "2:NG\n";
}
?>
--EXPECT--
1:OK
2:OK
