<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class ContextTest extends TestCase
{
    private $testDir;

    public static function setUpBeforeClass()
    {
        TestHelper::setUpTestDirectory(__CLASS__);
    }

    public function setUp()
    {
        $this->testDir = TestHelper::getTestDirectory($this);
    }

    public function testInit()
    {
        $ctx = Context::getDefaultContext();
        $this->assertInstanceOf('Groonga\\Context', $ctx);
    }
}
