<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class TableTest extends TestCase
{
    public function testInit()
    {
        $table = new Table(new Database, null);
        $this->assertInstanceOf('Groonga\\Table', $table);
    }
}
