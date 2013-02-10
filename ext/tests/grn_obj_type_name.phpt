--TEST--
grn_obj_type_name() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
echo grn_obj_type_name($db);
?>
--EXPECT--
db
