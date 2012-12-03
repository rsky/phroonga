--TEST--
grn_ctx_db() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_open($ctx, null);

$current_db = grn_ctx_db($ctx);
printf(
    "%s/%s\n",
    get_resource_type($current_db),
    grn_obj_type_name($current_db)
);

grn_ctx_use($ctx, null);
$current_db = grn_ctx_db($ctx);
printf("%s\n", gettype($current_db));
?>
--EXPECT--
grn_obj/db
NULL
