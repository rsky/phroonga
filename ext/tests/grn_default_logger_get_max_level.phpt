--TEST--
grn_default_logger_get_max_level() function
--INI--
groonga.default_logger_max_level = GRN_LOG_DEBUG
--FILE--
<?php
$level = grn_default_logger_get_max_level();
if ($level === GRN_LOG_DEBUG) {
    echo 'OK';
} else {
    echo "NG: {$level}";
}
?>
--EXPECT--
OK
