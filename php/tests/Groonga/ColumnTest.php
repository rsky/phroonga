<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class ColumnTest extends TestCase
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
        $table = $this->db->table('init');
        $column = $table->column('col1', array(
            'type' => Database::BUILTIN_TYPE_TEXT,
        ));
        $this->assertInstanceOf('Groonga\\Column', $column);
        $this->assertEquals(Object::TYPE_COLUMN_VAR_SIZE, $column->getType());
    }
}
