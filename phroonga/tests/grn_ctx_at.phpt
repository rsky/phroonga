--TEST--
grn_ctx_at() function
--SKIPIF--
SKIP incomplete
--FILE--
<?php
$ctx = grn_ctx_open();
$obj = grn_ctx_at($ctx, 1);
echo get_resource_type($obj);
?>
--EXPECT--
grn_obj
