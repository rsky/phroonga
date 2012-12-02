--TEST--
grn_ctx_get_command_version() function returns 1
--INI--
phroonga.default_command_version = GRN_COMMAND_VERSION_2
--FILE--
<?php
$ctx = grn_ctx_open();
$version = grn_ctx_get_command_version($ctx);
if ($version === GRN_COMMAND_VERSION_1) {
    echo 'OK';
} else {
    echo "NG: {$version}";
}
?>
--EXPECT--
OK
