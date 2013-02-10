<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class ColumnTest extends TestCase
{
    public function testInit()
    {
        $column = new Column(new Table(new Database), 'test');
        $this->assertInstanceOf('Groonga\\Column', $column);
    }
}
