<?php
namespace Groonga;

use PHPUnit_Framework_TestCase as TestCase;

class ObjectTest extends TestCase
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

    /**
     * @expectedException InvalidArgumentException
     */
    public function testInstantiateFailure()
    {
        new Object('error!');
    }
}
