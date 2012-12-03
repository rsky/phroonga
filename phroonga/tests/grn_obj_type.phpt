--TEST--
grn_obj_type() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);
$type = grn_obj_type($db);
if ($type === GRN_DB) {
    echo 'OK';
} else {
    echo "NG: {$type}";
}
?>
--EXPECT--
OK
