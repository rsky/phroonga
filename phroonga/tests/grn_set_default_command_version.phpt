--TEST--
grn_set_default_command_version() function
--FILE--
<?php
$rc = grn_set_default_command_version(GRN_COMMAND_VERSION_2);
if ($rc !== GRN_SUCCESS) {
    echo "NG: rc = {$rc}";
} else {
    echo grn_get_default_command_version();
}
?>
--EXPECT--
2
