--TEST--
grn_ctx_get_command_version() function returns the stable version
--INI--
groonga.default_command_version = GRN_COMMAND_VERSION_MAX
--FILE--
<?php
$ctx = grn_ctx_open();
$version = grn_ctx_get_command_version($ctx);
if ($version === GRN_COMMAND_VERSION_STABLE) {
    echo 'OK';
} else {
    echo "NG: {$version}";
}
?>
--EXPECT--
OK
