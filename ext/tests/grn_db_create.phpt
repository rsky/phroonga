--TEST--
grn_db_open() function
--FILE--
<?php
require_once __DIR__ . '/test_helper.inc';
$dir = phroonga_test_setup_directory_for(__FILE__);
$path = $dir . '/test.db';

$ctx = grn_ctx_open();
$db = grn_db_create($ctx, $path);
if ($db) {
    printf('%s/%s', get_resource_type($db), grn_obj_type_name($db));
}
?>
--EXPECT--
grn_obj/db
