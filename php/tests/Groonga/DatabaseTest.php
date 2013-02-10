<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class DatabaseTest extends TestCase
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
        $db = $ctx->database($this->testDir . '/test.db');
        $this->assertInstanceOf('Groonga\\Database', $db);
        $this->assertEquals(Object::TYPE_DB, $db->getType());
    }
}
