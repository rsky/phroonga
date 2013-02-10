--TEST--
grn_ctx_open() function
--FILE--
<?php
$ctx = grn_ctx_open();
echo get_resource_type($ctx);
?>
--EXPECT--
grn_ctx
