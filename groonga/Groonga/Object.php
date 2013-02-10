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
}
