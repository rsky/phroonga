--TEST--
grn_obj_id() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
$obj = grn_ctx_at($ctx, GRN_DB_TEXT);
if ($obj) {
    $id = grn_obj_id($obj);
    if ($id === GRN_DB_TEXT) {
        echo 'OK';
    } else {
        echo "NG: {$id}";
    }
} else {
    echo 'NG';
}
?>
--EXPECT--
OK
