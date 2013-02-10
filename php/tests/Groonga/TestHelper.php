<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class TestHelper
{
    public static function getTestDirectory($class)
    {
        if (is_object($class)) {
            $class = get_class($class);
        }

        $dir = realpath(__DIR__ . '/../../../work');
        if (!is_string($dir) || !is_dir($dir)) {
            $dir = getcwd();
        }

        return $dir . DIRECTORY_SEPARATOR . preg_replace('/\\W/', '_', $class);
    }

    public static function setUpTestDirectory($class)
    {
        $dir = self::getTestDirectory($class);
        if (is_dir($dir)) {
            exec('rm -rf ' . escapeshellarg($dir));
        }
        mkdir($dir);

        return $dir;
    }
}
