--TEST--
grn_obj_ctx() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);
$result = null;
if ($db) {
    $result = grn_obj_ctx($db);
}
if ($result === $ctx) {
    echo 'OK';
} else {
    var_dump($result);
}
?>
--EXPECT--
OK
