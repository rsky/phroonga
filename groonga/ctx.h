/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "prn_resource.h"

#ifndef PHROONGA_CTX_H
#define PHROONGA_CTX_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_register_ctx(INIT_FUNC_ARGS);
PRN_LOCAL int prn_ctx_register(grn_ctx *ctx TSRMLS_DC);
PRN_LOCAL zval *prn_ctx_zval(zval *zv, grn_ctx *ctx TSRMLS_DC);
PRN_LOCAL zend_bool prn_ctx_check_impl(grn_ctx *ctx TSRMLS_DC);

/* }}} */
/* {{{ PHP function prototypes */

PRN_FUNCTION(grn_ctx_open);
PRN_FUNCTION(grn_ctx_get_encoding);
PRN_FUNCTION(grn_ctx_set_encoding);
PRN_FUNCTION(grn_ctx_get_command_version);
PRN_FUNCTION(grn_ctx_set_command_version);
PRN_FUNCTION(grn_ctx_get_match_escalation_threshold);
PRN_FUNCTION(grn_ctx_set_match_escalation_threshold);
PRN_FUNCTION(grn_get_default_ctx);
PRN_FUNCTION(grn_set_default_ctx);

/* }}} */

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
