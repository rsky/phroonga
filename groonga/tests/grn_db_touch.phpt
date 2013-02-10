--TEST--
grn_db_touch() function
--FILE--
<?php
error_reporting(E_ALL);
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
if ($db) {
    grn_db_touch($db);
}
?>
--EXPECT--
