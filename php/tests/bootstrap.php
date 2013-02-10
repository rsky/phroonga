<?php
spl_autoload_register(function ($name) {
    if (strpos($name, 'Groonga\\') === 0) {
        if (strpos($name, 'Test') === false) {
            include dirname(__DIR__)
                . DIRECTORY_SEPARATOR
                . str_replace('\\', DIRECTORY_SEPARATOR, $name)
                . '.php';
            return true;
        }
    }
});
error_reporting(E_ALL);
