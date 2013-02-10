--TEST--
grn_ctx_get_command_version() function
--INI--
groonga.default_command_version = GRN_COMMAND_VERSION_2
--FILE--
<?php
$ctx = grn_ctx_open();
$db = grn_db_create($ctx, null);
$version = grn_ctx_get_command_version($ctx);
if ($version === GRN_COMMAND_VERSION_2) {
    echo 'OK';
} else {
    echo "NG: {$version}";
}
?>
--EXPECT--
OK
