--TEST--
grn_obj_domain() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
if ($db) {
    echo grn_obj_domain($db);
} else {
    echo 'NG';
}
?>
--EXPECT--
0
