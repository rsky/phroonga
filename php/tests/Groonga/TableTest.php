<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class TableTest extends TestCase
{
    private $testDir;
    private $db;

    public static function setUpBeforeClass()
    {
        TestHelper::setUpTestDirectory(__CLASS__);
    }

    public function setUp()
    {
        $this->testDir = TestHelper::getTestDirectory($this);
        $this->db = new Database($this->testDir . '/test.db');
    }

    public function testInit()
    {
        $table = $this->db->table(null, array(
            'flags' => Object::TYPE_TABLE_NO_KEY,
        ));
        $this->assertInstanceOf('Groonga\\Table', $table);
        $this->assertEquals(Object::TYPE_TABLE_NO_KEY, $table->getType());
    }
}
