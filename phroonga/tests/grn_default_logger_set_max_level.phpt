--TEST--
grn_default_logger_set_max_level() function
--FILE--
<?php
grn_default_logger_set_max_level(GRN_LOG_DUMP);
$level = grn_default_logger_get_max_level();
if ($level === GRN_LOG_DUMP) {
    echo 'OK: GRN_LOG_DUMP';
} else {
    echo "NG: {$level}";
}
?>
--EXPECT--
OK: GRN_LOG_DUMP
