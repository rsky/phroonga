<?php
namespace Groonga;

class Column extends Object
{
    public function __construct(Table $table, $name, array $options = array())
    {
        $path = null;
        $flags = 0;
        $type = null;

        foreach (array('path', 'flags', 'type') as $option) {
            if (isset($option[$option])) {
                $$option = $option[$option];
            }
        }

        $previous = ini_set('track_errors', '1');
        $column = @grn_column_open(
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
        $this->obj = $column;
    }
}
