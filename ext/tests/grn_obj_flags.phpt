--TEST--
grn_obj_flags() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
$flags = grn_obj_flags($db);
if ($flags === GRN_OBJ_TEMPORARY) {
    echo 'OK';
} else {
    echo "NG: {$flags}";
}
?>
--EXPECT--
OK
