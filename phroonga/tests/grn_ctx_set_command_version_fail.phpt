--TEST--
grn_ctx_set_command_version() function will fail
--INI--
phroonga.default_command_version = GRN_COMMAND_VERSION_1
--FILE--
<?php
$ctx = grn_ctx_open();
$result = @grn_ctx_set_command_version($ctx, GRN_COMMAND_VERSION_2);
if ($result === false) {
    echo 'FAILURE';
} else {
    echo 'SUCCESS';
}
?>
--EXPECT--
FAILURE
