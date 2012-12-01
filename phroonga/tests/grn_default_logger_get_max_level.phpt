--TEST--
grn_default_logger_get_max_level() function
--FILE--
<?php
$level = grn_default_logger_get_max_level();
if ($level === GRN_LOG_DEFAULT_LEVEL) {
    echo 'OK: GRN_LOG_DEFAULT_LEVEL';
} else {
    echo "NG: {$level}";
}
?>
--EXPECT--
OK: GRN_LOG_DEFAULT_LEVEL
