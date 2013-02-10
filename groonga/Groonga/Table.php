<?php
namespace Groonga;

class Table extends Object
{
    public function __construct(Database $db, $name = null, array $options = array())
    {
        $db->activate();

        $path = null;
        $flags = 0;
        $keyType = null;
        $valueType = null;

        foreach (array('path', 'flags', 'keyType', 'valueType') as $option) {
            if (isset($option[$option])) {
                $$option = $option[$option];
            }
        }

        $previous = ini_set('track_errors', '1');
        $table = @grn_table_open(
            $db->getContext()->getResource(),
            $name,
            $path,
            $flags,
            $keyType,
            $valueType
        );
        ini_set('track_errors', $previous);
        if (!$table) {
            throw new Exception($php_errormsg);
        }
        $this->obj = $table;
    }

    public function getResource()
    {
        return $this->obj;
    }

    public function openColumn($name, array $options = array())
    {
        return new Column($this, $name, $options);
    }
}
