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
/* {{{ prn_ctx_zval() */

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

	return retval;
}

/* }}} */
/* {{{ grn_ctx_open() */

PRN_FUNCTION(grn_ctx_open)
{
	long flags = 0L;
	int actual_flags;
	grn_ctx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		return;
	}

	/* set GRN_CTX_USE_QL so as to invoke grn_ctx_impl_init() */
	actual_flags = (int)flags;
	ctx = grn_ctx_open(actual_flags | GRN_CTX_USE_QL);
	/* ctx = grn_ctx_open((int)flags); */
	if (!ctx) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open context");
		RETURN_NULL();
	}
	ctx->flags = actual_flags;

	prn_ctx_zval(ctx, return_value TSRMLS_CC);
}

/* }}} */
/* {{{ grn_ctx_get_encoding() */

PRN_FUNCTION(grn_ctx_get_encoding)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	grn_encoding encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	encoding = GRN_CTX_GET_ENCODING(ctx);

	RETURN_LONG((long)encoding);
}

/* }}} */
/* {{{ grn_ctx_set_encoding() */

PRN_FUNCTION(grn_ctx_set_encoding)
{
	zval *zctx = NULL;
	long encoding = 0L;
	grn_ctx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &encoding) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	if (zend_ts_hash_index_exists(PRNG(encodings_ht), (ulong)encoding)) {
		GRN_CTX_SET_ENCODING(ctx, (grn_encoding)encoding);
		RETURN_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid encoding: %ld", encoding);
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ grn_ctx_get_command_version() */

PRN_FUNCTION(grn_ctx_get_command_version)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	grn_command_version version;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	version = grn_ctx_get_command_version(ctx);

	RETURN_LONG((long)version);
}

/* }}} */
/* {{{ grn_ctx_set_command_version() */

PRN_FUNCTION(grn_ctx_set_command_version)
{
	zval *zctx = NULL;
	long version = 0L;
	grn_ctx *ctx;
	grn_rc rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &version) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	rc = grn_ctx_set_command_version(ctx, (grn_command_version)version);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set command version: %d: %s", rc, ctx->errbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ grn_ctx_get_match_escalation_threshold() */

PRN_FUNCTION(grn_ctx_get_match_escalation_threshold)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	long long int threshold;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	threshold = grn_ctx_get_match_escalation_threshold(ctx);

	if (LONG_MIN <= threshold && threshold <= LONG_MAX) {
		RETURN_LONG((long)threshold);
	} else {
		RETURN_DOUBLE((double)threshold);
	}
}

/* }}} */
/* {{{ grn_ctx_set_match_escalation_threshold() */

PRN_FUNCTION(grn_ctx_set_match_escalation_threshold)
{
	zval *zctx = NULL;
	long threshold = 0L;
	grn_ctx *ctx;
	grn_rc rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &threshold) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	rc = grn_ctx_set_match_escalation_threshold(ctx, (long long int)threshold);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set threshold: %d: %s", rc, ctx->errbuf);
		RETURN_FALSE;
	}

	RETURN_TRUE;
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
