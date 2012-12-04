/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "prn_resource.h"

#ifndef PHROONGA_DAT_H
#define PHROONGA_DAT_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_register_dat(INIT_FUNC_ARGS);
PRN_LOCAL grn_rc prn_dat_cursor_close(grn_ctx *ctx, grn_dat_cursor *cursor);

/* }}} */
/* {{{ PHP function prototypes */

/* }}} */
/* {{{ inline functions and utility macros */

static inline prn_dat *prn_dat_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_dat *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_dat_rsrc_name, NULL, 1, le_grn_dat);
}

static inline prn_dat_cursor *prn_dat_cursor_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_dat_cursor *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_dat_cursor_rsrc_name, NULL, 1, le_grn_dat_cursor);
}

static inline zval *prn_dat_zval(zval *zv, grn_dat *dat,
	int ctx_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, dat, le_grn_dat,
		ctx_id, ctx, (prn_resource_dtor)grn_dat_close TSRMLS_CC);
}

static inline zval *prn_dat_cursor_zval(zval *zv, grn_dat_cursor *cursor,
	int dat_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, cursor, le_grn_dat_cursor,
		dat_id, ctx, (prn_resource_dtor)prn_dat_cursor_close TSRMLS_CC);
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
