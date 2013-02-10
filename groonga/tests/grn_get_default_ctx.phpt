--TEST--
grn_get_default_ctx() function
--FILE--
<?php
$db = grn_db_create(null, null);
$ctx = grn_get_default_ctx();
if ($ctx === grn_obj_ctx($db)) {
    echo 'OK';
} else {
    echo 'NG';
}
?>
--EXPECT--
OK
