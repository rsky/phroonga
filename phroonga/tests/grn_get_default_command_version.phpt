--TEST--
grn_get_default_command_version() function
--INI--
phroonga.default_command_version = GRN_COMMAND_VERSION_2
--FILE--
<?php
$version = grn_get_default_command_version();
if ($version === GRN_COMMAND_VERSION_2) {
    echo 'OK';
} else {
    echo "NG: {$version}";
}
?>
--EXPECT--
OK
