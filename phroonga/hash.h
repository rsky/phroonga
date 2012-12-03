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

#ifndef PHROONGA_HASH_H
#define PHROONGA_HASH_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_hash_startup(INIT_FUNC_ARGS);
PRN_LOCAL grn_rc prn_hash_cursor_close(grn_ctx *ctx, grn_hash_cursor *cursor);

/* }}} */
/* {{{ PHP function prototypes */

/* }}} */
/* {{{ inline functions and utility macros */

static inline prn_hash *prn_hash_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_hash *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_hash_rsrc_name, NULL, 1, le_grn_hash);
}

static inline prn_hash_cursor *prn_hash_cursor_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_hash_cursor *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_hash_cursor_rsrc_name, NULL, 1, le_grn_hash_cursor);
}

static inline zval *prn_hash_zval(zval *zv, grn_hash *hash,
	int ctx_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, hash, le_grn_hash,
		ctx_id, ctx, (prn_resource_dtor)grn_hash_close TSRMLS_CC);
}

static inline zval *prn_hash_cursor_zval(zval *zv, grn_hash_cursor *cursor,
	int hash_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, cursor, le_grn_hash_cursor,
		hash_id, ctx, (prn_resource_dtor)prn_hash_cursor_close TSRMLS_CC);
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
