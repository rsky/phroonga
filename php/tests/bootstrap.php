<?php
spl_autoload_register(function ($name) {
    if (strpos($name, 'Groonga\\') === 0) {
        if ($name === 'Groonga\\TestHelper') {
            include __DIR__ . '/Groonga/TestHelper.php';
        } elseif (strpos($name, 'Test') === false) {
            include dirname(__DIR__)
                . DIRECTORY_SEPARATOR
                . str_replace('\\', DIRECTORY_SEPARATOR, $name)
                . '.php';
        }
    }
});
error_reporting(E_ALL);
