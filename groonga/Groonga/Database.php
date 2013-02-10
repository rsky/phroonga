<?php
namespace Groonga;

class Database extends Object
{
    public function __construct($path = null, Context $ctx = null)
    {
        if (is_resource($path)
            && get_resource_type($path) === 'grn_obj'
            && grn_obj_type($path) === GRN_DB
        ) {
            $db = $path;
        } else {
            $db = grn_db_open($ctx ? $ctx->getResource() : null, $path);
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
