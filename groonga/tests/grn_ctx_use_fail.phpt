--TEST--
grn_ctx_use() function will fail
--FILE--
<?php
$ctx1 = grn_ctx_open();
$ctx2 = grn_ctx_open();
$db = grn_db_open($ctx1, null);
if (@grn_ctx_use($ctx2, $db)) {
    echo 'OK';
} else {
    echo 'NG';
}
?>
--EXPECT--
NG
