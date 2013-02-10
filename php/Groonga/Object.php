<?php
namespace Groonga;

class Object
{
    const TYPE_VOID     = \GRN_VOID;
    const TYPE_BULK     = \GRN_BULK;
    const TYPE_PTR      = \GRN_PTR;
    const TYPE_UVECTOR  = \GRN_UVECTOR;
    const TYPE_PVECTOR  = \GRN_PVECTOR;
    const TYPE_VECTOR   = \GRN_VECTOR;
    const TYPE_MSG      = \GRN_MSG;
    const TYPE_QUERY    = \GRN_QUERY;
    const TYPE_ACCESSOR = \GRN_ACCESSOR;
    const TYPE_SNIP     = \GRN_SNIP;
    const TYPE_PATSNIP  = \GRN_PATSNIP;
    const TYPE_STRING   = \GRN_STRING;
    const TYPE_CURSOR_TABLE_HASH_KEY    = \GRN_CURSOR_TABLE_HASH_KEY;
    const TYPE_CURSOR_TABLE_PAT_KEY     = \GRN_CURSOR_TABLE_PAT_KEY;
    const TYPE_CURSOR_TABLE_DAT_KEY     = \GRN_CURSOR_TABLE_DAT_KEY;
    const TYPE_CURSOR_TABLE_NO_KEY      = \GRN_CURSOR_TABLE_NO_KEY;
    const TYPE_CURSOR_COLUMN_INDEX      = \GRN_CURSOR_COLUMN_INDEX;
    const TYPE_CURSOR_COLUMN_GEO_INDEX  = \GRN_CURSOR_COLUMN_GEO_INDEX;
    const TYPE_TYPE             = \GRN_TYPE;
    const TYPE_PROC             = \GRN_PROC;
    const TYPE_EXPR             = \GRN_EXPR;
    const TYPE_TABLE_HASH_KEY   = \GRN_TABLE_HASH_KEY;
    const TYPE_TABLE_PAT_KEY    = \GRN_TABLE_PAT_KEY;
    const TYPE_TABLE_DAT_KEY    = \GRN_TABLE_DAT_KEY;
    const TYPE_TABLE_NO_KEY     = \GRN_TABLE_NO_KEY;
    const TYPE_DB               = \GRN_DB;
    const TYPE_COLUMN_FIX_SIZE  = \GRN_COLUMN_FIX_SIZE;
    const TYPE_COLUMN_VAR_SIZE  = \GRN_COLUMN_VAR_SIZE;
    const TYPE_COLUMN_INDEX     = \GRN_COLUMN_INDEX;

    protected $obj;

    public function __construct($obj)
    {
        if (!$this->isObjectResource($obj)) {
            throw new InvalidArgumentException(
                'Argument #1 must be a grn_obj resource'
            );
        }
        $this->obj = $obj;
    }

    public function getResource()
    {
        return $this->obj;
    }

    public function getContext()
    {
        if ($this->obj) {
            $ctx = grn_obj_ctx($this->obj);
        } else {
            $ctx = grn_get_default_ctx();
        }
        return new Context($ctx);
    }

    public function getType()
    {
        return grn_obj_type($this->obj);
    }

    protected function isObjectResource($value, $type = null)
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

    protected function fetchResource($value, Context $ctx = null)
    {
        if (is_object($value) && $value instanceof Object) {
            return $value->getResource();
        }

        if (is_scalar($value)) {
            if (!$ctx) {
                $ctx = $this->getContext();
            }
            if (is_int($value)) {
                return grn_ctx_at($ctx->getResource(), $value);
            }
            return grn_ctx_get($ctx->getResource(), $value);
        }

        return $value;
    }
}
