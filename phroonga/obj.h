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

#ifndef PHROONGA_OBJ_H
#define PHROONGA_OBJ_H

BEGIN_EXTERN_C()

/* {{{ internal function prototypes */

PRN_LOCAL int prn_obj_startup(INIT_FUNC_ARGS);

PRN_LOCAL grn_rc prn_obj_unlink(grn_ctx *ctx, grn_obj *obj);

/* }}} */
/* {{{ PHP function prototypes */

PRN_FUNCTION(grn_obj_type);
PRN_FUNCTION(grn_obj_type_name);
PRN_FUNCTION(grn_db_open);
PRN_FUNCTION(grn_db_touch);
PRN_FUNCTION(grn_ctx_use);
PRN_FUNCTION(grn_ctx_db);
PRN_FUNCTION(grn_ctx_get);
PRN_FUNCTION(grn_ctx_at);

/* }}} */
/* {{{ inline functions and utility macros */

PRN_EXTERN_RSRC_NAME(prn_obj_rsrc_name);

static inline prn_obj *prn_obj_fetch_internal(zval *zv TSRMLS_DC)
{
	return (prn_obj *)zend_fetch_resource(&zv TSRMLS_CC,
		-1, prn_obj_rsrc_name, NULL, 1, le_grn_obj);
}

static inline zval *prn_obj_zval(zval *zv, grn_obj *obj,
	int ctx_id, grn_ctx *ctx, grn_rc (*dtor)(grn_ctx *, grn_obj *) TSRMLS_DC)
{
	return prn_resource_zval(zv, obj, le_grn_obj,
		ctx_id, ctx, (prn_resource_dtor)dtor TSRMLS_CC);
}

#define PRN_ZVAL_GRN_OBJ(zv, obj, ctx, zctx) \
	prn_obj_zval((zv), (obj), (int)Z_LVAL_P(zctx), (ctx), prn_obj_unlink TSRMLS_CC)

#define PRN_RETVAL_GRN_OBJ(obj, ctx, zctx) \
	PRN_ZVAL_GRN_OBJ(return_value, (obj), (ctx), (zctx))

#define PRN_RETURN_GRN_OBJ(obj, ctx, zctx) { \
	PRN_RETVAL_GRN_OBJ(obj, ctx, zctx); \
	return; \
}

#define PRN_ZVAL_GRN_OBJ_EX(zv, obj, ctx, zctx, dtor) \
	prn_obj_zval((zv), (obj), (int)Z_LVAL_P(zctx), (ctx), (dtor) TSRMLS_CC)

#define PRN_RETVAL_GRN_OBJ_EX(obj, ctx, zctx, dtor) \
	PRN_ZVAL_GRN_OBJ_EX(return_value, (obj), (ctx), (zctx), (dtor))

#define PRN_RETURN_GRN_OBJ_EX(obj, ctx, zctx, dtor) { \
	PRN_RETVAL_GRN_OBJ_EX((obj), (ctx), (zctx), (dtor)); \
	return; \
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
