/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"

#ifndef PHROONGA_OBJ_H
#define PHROONGA_OBJ_H

/* {{{ type definitions */

typedef struct {
	 int ctx_id;
	 grn_ctx *ctx;
	 grn_obj *obj;
} prn_obj;

/* }}} */
/* {{{ internal function prototypes */

PRN_LOCAL int prn_register_obj(INIT_FUNC_ARGS);
PRN_LOCAL prn_obj *prn_fetch_obj_internal(zval *zv TSRMLS_DC);
PRN_LOCAL zval *prn_obj_zval(int ctx_id, grn_ctx *ctx, grn_obj *obj, zval *zv TSRMLS_DC);

/* }}} */
/* {{{ inline functions and utility macros */

#define PRN_RETVAL_GRN_OBJ(obj, ctx, zctx) \
	prn_obj_zval((int)Z_LVAL_P(zctx), (ctx), (obj), return_value TSRMLS_CC)

#define RPN_RETURN_GRN_OBJ(obj, ctx, zctx) { \
	PRN_RETVAL_GRN_OBJ(obj, ctx, zctx); \
	return; \
} 

/* }}} */
/* {{{ PHP function prototypes */

PRN_FUNCTION(grn_db_open);
PRN_FUNCTION(grn_db_touch);
PRN_FUNCTION(grn_ctx_use);
PRN_FUNCTION(grn_ctx_db);
PRN_FUNCTION(grn_ctx_get);
PRN_FUNCTION(grn_ctx_at);

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
