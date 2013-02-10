<?php
namespace Groonga;

class Database extends Object
{
    public function __construct($path = null, Context $ctx = null)
    {
        if ($this->isObject($path, GRN_DB)) {
            $db = $path;
        } else {
            $previous = ini_set('track_errors', '1');
            $db = @grn_db_open($ctx ? $ctx->getResource() : null, $path);
            ini_set('track_errors', $previous);
            if (!$db) {
                throw new Exception($php_errormsg);
            }
        }
        $this->obj = $db;
        $this->activate();
    }

    public function touch()
    {
        grn_db_touch($this->db);
    }

    public function activate()
    {
        grn_db_use($this->obj);
    }

    public function openTable($name = null, array $options = array())
    {
        return new Table($this, $name, $options);
    }
}
