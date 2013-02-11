--TEST--
grn_ctx_get() function
--FILE--
<?php
require_once __DIR__ . '/test_helper.inc';
$dir = phroonga_test_setup_directory_for(__FILE__);
$path = $dir . '/test.db';

$ctx = grn_ctx_open();
$db = grn_db_create($ctx, $path);
$table = grn_table_create($ctx, 'test', null, GRN_OBJ_TABLE_NO_KEY);
$obj = grn_ctx_get($ctx, 'test');
if ($obj) {
    printf(
        '%s/%s/%s',
        get_resource_type($obj),
        grn_obj_type_name($obj),
        (grn_obj_id($obj) === grn_obj_id($table)) ? 'OK' : 'NG'
    );
} else {
    echo 'NG';
}
?>
--EXPECT--
grn_obj/table_no_key/OK
