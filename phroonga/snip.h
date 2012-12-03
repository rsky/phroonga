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

#ifndef PHROONGA_SNIP_H
#define PHROONGA_SNIP_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_snip_startup(INIT_FUNC_ARGS);

/* }}} */
/* {{{ PHP function prototypes */

/* }}} */
/* {{{ inline functions and utility macros */

static inline prn_snip *prn_snip_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_snip *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_snip_rsrc_name, NULL, 1, le_grn_snip);
}

static inline zval *prn_snip_zval(zval *zv, grn_snip *snip,
	int ctx_id, grn_ctx *ctx TSRMLS_DC)
{
	return prn_resource_zval(zv, snip, le_grn_snip,
		ctx_id, ctx, (prn_resource_dtor)grn_snip_close TSRMLS_CC);
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
