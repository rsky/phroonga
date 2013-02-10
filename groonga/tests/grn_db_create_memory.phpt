--TEST--
grn_db_create() function (in memory database)
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
if ($db) {
    printf('%s/%s', get_resource_type($db), grn_obj_type_name($db));
}
?>
--EXPECT--
grn_obj/db
