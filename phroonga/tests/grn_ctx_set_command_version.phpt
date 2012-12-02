--TEST--
grn_ctx_set_command_version() function
--INI--
phroonga.default_command_version = GRN_COMMAND_VERSION_1
--FILE--
<?php
$ctx = grn_ctx_open();
grn_ctx_set_command_version($ctx, GRN_COMMAND_VERSION_2);
$version = grn_ctx_get_command_version($ctx);
if ($version === GRN_COMMAND_VERSION_2) {
    echo 'OK';
} else {
    echo "NG: {$version}";
}
?>
--EXPECT--
OK
