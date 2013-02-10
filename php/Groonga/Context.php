<?php
namespace Groonga;

class Context
{
    const GRN_ENC_DEFAULT = \GRN_ENC_DEFAULT;
    const GRN_ENC_NONE    = \GRN_ENC_NONE;
    const GRN_ENC_EUC_JP  = \GRN_ENC_EUC_JP;
    const GRN_ENC_UTF8    = \GRN_ENC_UTF8;
    const GRN_ENC_SJIS    = \GRN_ENC_SJIS;
    const GRN_ENC_LATIN1  = \GRN_ENC_LATIN1;
    const GRN_ENC_KOI8R   = \GRN_ENC_KOI8R;

    const COMMAND_VERSION_DEFAULT = \GRN_COMMAND_VERSION_DEFAULT;
    const COMMAND_VERSION_1       = \GRN_COMMAND_VERSION_1;
    const COMMAND_VERSION_2       = \GRN_COMMAND_VERSION_2;
    const COMMAND_VERSION_MIN     = \GRN_COMMAND_VERSION_MIN;
    const COMMAND_VERSION_STABLE  = \GRN_COMMAND_VERSION_STABLE;
    const COMMAND_VERSION_MAX     = \GRN_COMMAND_VERSION_MAX;

    private $ctx;

    public static function getDefaultContext()
    {
        return new Context(grn_get_default_ctx());
    }

    public function __construct($flags = 0)
    {
        if (is_resource($flags) && get_resource_type($flags) === 'grn_ctx') {
            $this->ctx = $flags;
        } else {
            $previous = ini_set('track_errors', '1');
            $ctx = @grn_ctx_open($flags);
            ini_set('track_errors', $previous);

            if (!$ctx) {
                throw new Exception($php_errormsg);
            }
            $this->ctx = $ctx;
        }
    }

    public function getResource()
    {
        return $this->ctx;
    }

    public function setDefault()
    {
        grn_set_default_ctx($this->ctx);
    }

    public function getEncoding()
    {
        return grn_ctx_get_encoding($this->ctx);
    }

    public function setEncoding($encoding)
    {
        grn_ctx_set_encoding($this->ctx, $encoding);
    }

    public function getCommandVersion($version)
    {
        return grn_ctx_get_command_version($this->ctx);
    }

    public function setCommandVersion($version)
    {
        grn_ctx_set_command_version($this->ctx, $version);
    }

    public function getMatchEscalationThreshold()
    {
        return grn_ctx_get_match_escalation_threshold($this->ctx);
    }

    public function setMatchEscalationThreshold($threshold)
    {
        grn_ctx_set_match_escalation_threshold($this->ctx, $threshold);
    }

    public function database($path)
    {
        return new Database($path, $this);
    }

    public function getActiveDatabase()
    {
        $db = grn_ctx_db($this->ctx);
        if ($db) {
            return new Database($db);
        }
        return null;
    }

    public function getObject($name)
    {
        $obj = grn_ctx_get($this->ctx, $name);
        if ($obj) {
            return new Object($obj);
        }
        return null;
    }

    public function getObjectById($id)
    {
        $obj = grn_ctx_at($this->ctx, $id);
        if ($obj) {
            return new Object($obj);
        }
        return null;
    }

}
