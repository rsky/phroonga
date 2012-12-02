--TEST--
grn_db_open() function (in memory database)
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);
if ($db) {
    printf('%s/%s', get_resource_type($db), phroonga_obj_type_name($db));
}
?>
--EXPECT--
grn_obj/db
