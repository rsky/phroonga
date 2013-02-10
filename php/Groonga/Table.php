<?php
namespace Groonga;

class Table extends Object
{
    public function __construct(Database $db, $name, array $options = array())
    {
        $db->activate();

        $tableTypes = array(
            self::TYPE_TABLE_HASH_KEY,
            self::TYPE_TABLE_PAT_KEY,
            self::TYPE_TABLE_DAT_KEY,
            self::TYPE_TABLE_NO_KEY,
        );
        if ($this->isObject($name, $tableTypes)) {
            parent::__construct($name);
            return;
        }

        $path = null;
        $flags = 0;
        $keyType = null;
        $valueType = null;

        foreach (array('path', 'flags', 'keyType', 'valueType') as $option) {
            if (isset($options[$option])) {
                $$option = $options[$option];
            }
        }

        $previous = ini_set('track_errors', '1');
        $table = @grn_table_open_or_create(
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

        parent::__construct($table);
    }

    public function openColumn($name, array $options = array())
    {
        return new Column($this, $name, $options);
    }
}
