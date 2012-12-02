<?php

function phroonga_test_setup_directory_for($test_filename)
{
    $dir = realpath(__DIR__ . '/../work')
        . '/'. pathinfo($test_filename, PATHINFO_FILENAME);
    if (is_dir($dir)) {
        exec('rm -rf ' . escapeshellarg($dir));
    }
    mkdir($dir);

    return $dir;
}
