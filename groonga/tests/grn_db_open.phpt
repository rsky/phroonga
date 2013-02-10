--TEST--
grn_db_open() function
--FILE--
<?php
require_once __DIR__ . '/test_helper.inc';
$dir = phroonga_test_setup_directory_for(__FILE__);
$path = $dir . '/test.db';

$ctx = grn_ctx_open();

$db1 = @grn_db_open($ctx, $path);
if ($db1) {
    printf("%s/%s\n", get_resource_type($db1), grn_obj_type_name($db1));
} else {
    echo "NG\n";
}

$db2 = grn_db_open_or_create($ctx, $path);
if ($db2) {
    printf("%s/%s\n", get_resource_type($db2), grn_obj_type_name($db2));
} else {
    echo "NG\n";
}

$db3 = grn_db_open($ctx, $path);
if ($db3) {
    printf("%s/%s\n", get_resource_type($db3), grn_obj_type_name($db3));
} else {
    echo "NG\n";
}
?>
--EXPECT--
NG
grn_obj/db
grn_obj/db
