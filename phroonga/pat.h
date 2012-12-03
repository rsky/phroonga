/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"
#include "prn_resource.h"

#ifndef PHROONGA_PAT_H
#define PHROONGA_PAT_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_pat_startup(INIT_FUNC_ARGS);
PRN_LOCAL grn_rc prn_pat_cursor_close(grn_ctx *ctx, grn_pat_cursor *cursor);

/* }}} */
/* {{{ PHP function prototypes */

/* }}} */
/* {{{ inline functions and utility macros */

static inline prn_pat *prn_pat_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_pat *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_pat_rsrc_name, NULL, 1, le_grn_pat);
}

static inline prn_pat_cursor *prn_pat_cursor_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_pat_cursor *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_pat_cursor_rsrc_name, NULL, 1, le_grn_pat_cursor);
}

static inline zval *prn_pat_zval(zval *zv, grn_pat *pat,
	int ctx_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, pat, le_grn_pat,
		ctx_id, ctx, (prn_resource_dtor)grn_pat_close TSRMLS_CC);
}

static inline zval *prn_pat_cursor_zval(zval *zv, grn_pat_cursor *cursor,
	int pat_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, cursor, le_grn_pat_cursor,
		pat_id, ctx, (prn_resource_dtor)prn_pat_cursor_close TSRMLS_CC);
}

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
