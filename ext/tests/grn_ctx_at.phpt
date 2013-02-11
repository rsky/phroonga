--TEST--
grn_ctx_at() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
$obj = grn_ctx_at($ctx, GRN_DB_TEXT);
if (is_resource($obj)) {
    printf('%s/%s', get_resource_type($obj), grn_obj_type_name($obj));
} else {
    echo 'NG';
}
?>
--EXPECT--
grn_obj/type
