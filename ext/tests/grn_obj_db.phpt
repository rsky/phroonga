--TEST--
grn_obj_db() function
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
$table = grn_table_create($ctx, null);
if ($table) {
    $expected = grn_obj_id($db);
    $actual = grn_obj_id(grn_obj_db($table));
    if ($expected === $actual) {
        echo 'OK';
    } else {
        echo "NG: {$expected}/{$actual}";
    }
} else {
    echo 'NG';
}
?>
--EXPECT--
OK
