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
static grn_ctx *prn_get_default_ctx(TSRMLS_DC);
static int prn_get_default_ctx_id(TSRMLS_DC);

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
/* {{{ _prn_ctx_fetch() */

PHPAPI grn_ctx *_prn_ctx_fetch(zval *zv, int *ctx_id TSRMLS_DC)
{
	grn_ctx *ctx;
	int rsrc_id;

	if (zv) {
		rsrc_id = (int)Z_LVAL_P(zv);
		ctx = prn_ctx_fetch_internal(zv TSRMLS_CC);
	} else {
		int type = 0;
		rsrc_id = prn_get_default_ctx_id(TSRMLS_CC);
		ctx = (grn_ctx *)zend_list_find(rsrc_id, &type);
		if (type != le_grn_ctx) {
			ctx = NULL;
		}
	}

	if (ctx && ctx_id) {
		*ctx_id = rsrc_id;
	}

	return ctx;
}

/* }}} */
/* {{{ prn_register_ctx() */

PRN_LOCAL int prn_register_ctx(INIT_FUNC_ARGS)
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
#if PHP_VERSION_ID >= 50400
	return zend_list_insert(ctx, le_grn_ctx TSRMLS_CC);
#else
	return zend_list_insert(ctx, le_grn_ctx);
#endif
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
	grn_ctx *ctx;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		return;
	}

	ctx = grn_ctx_open((int)flags);
	if (!ctx) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open context");
		return;
	}

	prn_ctx_zval(return_value, ctx TSRMLS_CC);

	if (!PRNG(default_context_id)) {
		int ctx_id = (int)Z_LVAL_P(return_value);
		PRNG(default_context_id) = ctx_id;
		zend_list_addref(ctx_id);
	}
}

/* }}} */
/* {{{ grn_ctx_get_encoding() */

PRN_FUNCTION(grn_ctx_get_encoding)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	grn_encoding encoding;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
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

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &encoding) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	if (prn_is_valid_encoding(encoding)) {
		GRN_CTX_SET_ENCODING(ctx, (grn_encoding)encoding);
		RETURN_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid encoding: %ld", encoding);
		return;
	}
}

/* }}} */
/* {{{ grn_ctx_get_command_version() */

PRN_FUNCTION(grn_ctx_get_command_version)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	grn_command_version version;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
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

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &version) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	if (!prn_ctx_check_impl(ctx TSRMLS_CC)) {
		return;
	}

	rc = grn_ctx_set_command_version(ctx, (grn_command_version)version);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set command version: %s", prn_errstr(rc));
		return;
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

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
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

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &threshold) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_internal(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	if (!prn_ctx_check_impl(ctx TSRMLS_CC)) {
		return;
	}

	rc = grn_ctx_set_match_escalation_threshold(ctx, (long long int)threshold);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to set threshold: %s", prn_errstr(rc));
		return;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ prn_get_default_ctx_id() */

static int prn_get_default_ctx_id(TSRMLS_DC)
{
	grn_ctx *ctx;
	int ctx_id = PRNG(default_context_id);

	if (ctx_id) {
		int type = 0;
		ctx = (grn_ctx *)zend_list_find(ctx_id, &type);
		if (ctx && type == le_grn_ctx) {
			return ctx_id;
		}
	}

	ctx = grn_ctx_open(0);
	if (!ctx) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to open the default context");
		return 0;
	}

	ctx_id = prn_ctx_register(ctx TSRMLS_CC);
	PRNG(default_context_id) = ctx_id;

	return ctx_id;
}

/* }}} */
/* {{{ grn_get_default_ctx() */

PRN_FUNCTION(grn_get_default_ctx)
{
	int ctx_id;

	RETVAL_NULL();

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	ctx_id = prn_get_default_ctx_id(TSRMLS_CC);
	if (ctx_id) {
		zend_list_addref(ctx_id);
		RETURN_RESOURCE((long)ctx_id);
	}
}

/* }}} */
/* {{{ grn_set_default_ctx() */

PRN_FUNCTION(grn_set_default_ctx)
{
	zval *zctx = NULL;
	int ctx_id;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	if (!prn_ctx_fetch_internal(zctx TSRMLS_CC)) {
		return;
	}

	ctx_id = (int)Z_LVAL_P(zctx);

	if (PRNG(default_context_id) != ctx_id) {
		if (PRNG(default_context_id)) {
			zend_list_delete(PRNG(default_context_id));
		}

		PRNG(default_context_id) = ctx_id;
		zend_list_addref(ctx_id);
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
