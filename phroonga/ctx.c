/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "ctx.h"

/* {{{ globals */

static int le_grn_ctx;

/* }}} */
/* {{{ function prototypes*/

static void prn_free_ctx(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* }}} */
/* {{{ prn_get_le_ctx() */

PHPAPI int prn_get_le_ctx(void)
{
	return le_grn_ctx;
}

/* }}} */
/* {{{ prn_fetch_ctx() */

PHPAPI grn_ctx *prn_fetch_ctx(zval *zv TSRMLS_DC)
{
	grn_ctx *ctx;

	ZEND_FETCH_RESOURCE_NO_RETURN(ctx, grn_ctx *, &zv, -1, "grn_ctx", le_grn_ctx);

	return ctx;
}

/* }}} */
/* {{{ prn_register_ctx() */

PRN_LOCAL int prn_register_ctx(INIT_FUNC_ARGS)
{
	int resource_id = zend_register_list_destructors_ex(
		prn_free_ctx, NULL, "grn_ctx", module_number);

	if (resource_id == FAILURE) {
		return FAILURE;
	}

	le_grn_ctx = resource_id;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_free_ctx() */

static void prn_free_ctx(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	grn_ctx_close((grn_ctx *)rsrc->ptr);
}

/* }}} */
/* {{{ */

PRN_LOCAL zval *prn_ctx_zval(grn_ctx *ctx, zval *zv TSRMLS_DC)
{
	zval *retval;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	ZEND_REGISTER_RESOURCE(retval, ctx, le_grn_ctx);

	return zv;
}

/* }}} */
/* {{{ grn_ctx_open() */

PRN_FUNCTION(grn_ctx_open)
{
	long flags = 0;
	grn_ctx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		return;
	}

	ctx = grn_ctx_open((int)flags);
	if (!ctx) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open context");
		RETURN_NULL();
	}

	prn_ctx_zval(ctx, return_value);
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
