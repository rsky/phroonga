--TEST--
grn_ctx_db() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);
$current_db = grn_ctx_db($ctx);
if ($current_db === $db) {
    echo 'OK';
} else {
    echo 'NG';
}
?>
--EXPECT--
OK
