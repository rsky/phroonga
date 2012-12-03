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

PRN_LOCAL int le_grn_ctx = -1;
PRN_DECLARE_RSRC_NAME(prn_ctx_rsrc_name, grn_ctx);

/* }}} */
/* {{{ function prototypes*/

static ZEND_RSRC_DTOR_FUNC(prn_ctx_destroy);

/* }}} */
/* {{{ prn_get_le_ctx() */

PHPAPI int prn_get_le_ctx(void)
{
	return le_grn_ctx;
}

/* }}} */
/* {{{ prn_ctx_fetch_internal() */

static inline grn_ctx *prn_ctx_fetch_internal(zval *zv TSRMLS_DC)
{
	return (grn_ctx *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_ctx_rsrc_name, NULL, 1, le_grn_ctx);
}

/* }}} */
/* {{{ prn_ctx_fetch() */

PHPAPI grn_ctx *prn_ctx_fetch(zval *zv TSRMLS_DC)
{
	return prn_ctx_fetch_internal(zv TSRMLS_CC);
}

/* }}} */
/* {{{ prn_ctx_startup() */

PRN_LOCAL int prn_ctx_startup(INIT_FUNC_ARGS)
{
	int resource_type = zend_register_list_destructors_ex(
		prn_ctx_destroy, NULL, prn_ctx_rsrc_name, module_number);

	if (resource_type == FAILURE) {
		return FAILURE;
	}

	le_grn_ctx = resource_type;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_ctx_destroy() */

static ZEND_RSRC_DTOR_FUNC(prn_ctx_destroy)
{
	grn_ctx_close((grn_ctx *)rsrc->ptr);
}

/* }}} */
/* {{{ prn_ctx_register() */

PRN_LOCAL int prn_ctx_register(grn_ctx *ctx TSRMLS_DC)
{
	return zend_list_insert(ctx, le_grn_ctx TSRMLS_CC);
}

/* }}} */
/* {{{ prn_ctx_zval() */

PRN_LOCAL zval *prn_ctx_zval(zval *zv, grn_ctx *ctx TSRMLS_DC)
{
	zval *retval;
	int ctx_id;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	ctx_id = prn_ctx_register(ctx TSRMLS_CC);
	ZVAL_RESOURCE(retval, (long)ctx_id);

	return retval;
}

/* }}} */
/* {{{ prn_ctx_check_impl() */

PRN_LOCAL zend_bool prn_ctx_check_impl(grn_ctx *ctx TSRMLS_DC)
{
	if (!ctx->impl) {
#if PHP_VERSION_ID < 50400
		char *space = NULL;
#else
		const char *space = NULL;
#endif
		const char *class_name = get_active_class_name(&space TSRMLS_CC);
		const char *function_name = get_active_function_name(TSRMLS_C);

		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"context is not prepared to use %s%s%s()",
			class_name, space, function_name);

		return 0;
	}

	return 1;
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

	ctx = grn_ctx_open((int)flags);
	if (!ctx) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open context");
		RETURN_NULL();
	}

	prn_ctx_zval(return_value, ctx TSRMLS_CC);
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	if (!prn_ctx_check_impl(ctx TSRMLS_CC)) {
		RETURN_FALSE;
	}

	rc = grn_ctx_set_command_version(ctx, (grn_command_version)version);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set command version: %s", prn_errstr(rc));
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
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

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		RETURN_FALSE;
	}

	if (!prn_ctx_check_impl(ctx TSRMLS_CC)) {
		RETURN_FALSE;
	}

	rc = grn_ctx_set_match_escalation_threshold(ctx, (long long int)threshold);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set threshold: %s", prn_errstr(rc));
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
