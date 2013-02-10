<?php
namespace Groonga;

class Database extends Object
{
    const BUILTIN_TYPE_VOID            = \GRN_DB_VOID;
    const BUILTIN_TYPE_DB              = \GRN_DB_DB;
    const BUILTIN_TYPE_OBJECT          = \GRN_DB_OBJECT;
    const BUILTIN_TYPE_BOOL            = \GRN_DB_BOOL;
    const BUILTIN_TYPE_INT8            = \GRN_DB_INT8;
    const BUILTIN_TYPE_UINT8           = \GRN_DB_UINT8;
    const BUILTIN_TYPE_INT16           = \GRN_DB_INT16;
    const BUILTIN_TYPE_UINT16          = \GRN_DB_UINT16;
    const BUILTIN_TYPE_INT32           = \GRN_DB_INT32;
    const BUILTIN_TYPE_UINT32          = \GRN_DB_UINT32;
    const BUILTIN_TYPE_INT64           = \GRN_DB_INT64;
    const BUILTIN_TYPE_UINT64          = \GRN_DB_UINT64;
    const BUILTIN_TYPE_FLOAT           = \GRN_DB_FLOAT;
    const BUILTIN_TYPE_TIME            = \GRN_DB_TIME;
    const BUILTIN_TYPE_SHORT_TEXT      = \GRN_DB_SHORT_TEXT;
    const BUILTIN_TYPE_TEXT            = \GRN_DB_TEXT;
    const BUILTIN_TYPE_LONG_TEXT       = \GRN_DB_LONG_TEXT;
    const BUILTIN_TYPE_TOKYO_GEO_POINT = \GRN_DB_TOKYO_GEO_POINT;
    const BUILTIN_TYPE_WGS84_GEO_POINT = \GRN_DB_WGS84_GEO_POINT;

    const BUILDIN_TOKENIZER_MECAB   = \GRN_DB_MECAB;
    const BUILDIN_TOKENIZER_DELIMIT = \GRN_DB_DELIMIT;
    const BUILDIN_TOKENIZER_UNIGRAM = \GRN_DB_UNIGRAM;
    const BUILDIN_TOKENIZER_BIGRAM  = \GRN_DB_BIGRAM;
    const BUILDIN_TOKENIZER_TRIGRAM = \GRN_DB_TRIGRAM;

    public function __construct($path = null, Context $ctx = null)
    {
        if ($this->isObjectResource($path, self::TYPE_DB)) {
            $db = $path;
        } else {
            $previous = ini_set('track_errors', '1');
            $db = @grn_db_open_or_create($ctx ? $ctx->getResource() : null, $path);
            ini_set('track_errors', $previous);

            if (!$db) {
                throw new Exception($php_errormsg);
            }
        }

        parent::__construct($db);

        $this->activate();
    }

    public function touch()
    {
        grn_db_touch($this->obj);
    }

    public function activate()
    {
        grn_db_use($this->obj);
    }

    public function table($name, array $options = array())
    {
        return new Table($this, $name, $options);
    }
}
