<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class ContextTest extends TestCase
{
    public function testInit()
    {
        $ctx = new Context;
        $this->assertInstanceOf('Groonga\\Context', $ctx);
    }
}
