<?php
namespace Groonga;

class Context
{
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

    public function openDatabase($path = null)
    {
        return new Database($path, $this);
    }

    public function getActiveDatabase()
    {
        $db = grn_ctx_db($this->ctx);
        if ($db) {
            return new Database($db);
        }
        return new Database(null, $this);
    }
}
