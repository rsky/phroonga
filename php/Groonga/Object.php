<?php
namespace Groonga;

abstract class Object
{
    protected $obj;

    public function getResource()
    {
        return $this->obj;
    }

    public function getContext()
    {
        return new Context(grn_obj_ctx($this->obj));
    }

    protected function isObject($value, $type = null)
    {
        if (is_resource($value) && get_resource_type($value) === 'grn_obj') {
            if (is_null($type)) {
                return true;
            }
            $objType = grn_obj_type($value);
            if (is_array($type)) {
                return in_array($objType, $type, true);
            }
            return $objType === $type;
        }
        return false;
    }
}
