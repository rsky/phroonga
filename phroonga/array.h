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

#ifndef PHROONGA_ARRAY_H
#define PHROONGA_ARRAY_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_array_startup(INIT_FUNC_ARGS);
PRN_LOCAL grn_rc prn_array_cursor_close(grn_ctx *ctx, grn_array_cursor *cursor);

/* }}} */
/* {{{ PHP function prototypes */

/* }}} */
/* {{{ inline functions and utility macros */

static inline prn_array *prn_array_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_array *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_array_rsrc_name, NULL, 1, le_grn_array);
}

static inline prn_array_cursor *prn_array_cursor_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_array_cursor *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_array_cursor_rsrc_name, NULL, 1, le_grn_array_cursor);
}

static inline zval *prn_array_zval(zval *zv, grn_array *array,
	int ctx_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, array, le_grn_array,
		ctx_id, ctx, (prn_resource_dtor)grn_array_close TSRMLS_CC);
}

static inline zval *prn_array_cursor_zval(zval *zv, grn_array_cursor *cursor,
	int array_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, cursor, le_grn_array_cursor,
		array_id, ctx, (prn_resource_dtor)prn_array_cursor_close TSRMLS_CC);
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
