<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class DatabaseTest extends TestCase
{
    public function testInit()
    {
        $db = new Database;
        $this->assertInstanceOf('Groonga\\Database', $db);
    }
}
