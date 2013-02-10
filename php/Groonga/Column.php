<?php
namespace Groonga;

class Column extends Object
{
    public function __construct(Table $table, $name, array $options = array())
    {
        $columnTypes = array(
            self::TYPE_COLUMN_FIX_SIZE,
            self::TYPE_COLUMN_VAR_SIZE,
            self::TYPE_COLUMN_INDEX,
        );
        if ($this->isObject($name, $columnTypes)) {
            parent::__construct($name);
            return;
        }

        $path = null;
        $flags = 0;
        $type = null;

        foreach (array('path', 'flags', 'type') as $option) {
            if (isset($options[$option])) {
                $$option = $options[$option];
            }
        }

        $previous = ini_set('track_errors', '1');
        $column = @grn_column_open_or_create(
            $table->getResource(),
            $name,
            $path,
            $flags,
            $type
        );
        ini_set('track_errors', $previous);

        if (!$column) {
            throw new Exception($php_errormsg);
        }

        parent::__construct($column);
    }
}
