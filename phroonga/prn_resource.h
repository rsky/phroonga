/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"

#ifndef PHROONGA_RESOURCE_H
#define PHROONGA_RESOURCE_H

BEGIN_EXTERN_C()

/* {{{ type definitions */

typedef grn_rc (*prn_object_dtor)(grn_ctx *ctx, void *ptr);

typedef struct {
	int resource_id;
	int ctx_id;
	grn_ctx *ctx;
	union {
		void *ptr;
		grn_obj *obj;
		grn_snip *snip;
		grn_hash *hash;
		grn_array *array;
		grn_pat *pat;
		grn_dat *dat;
	} value;
	prn_object_dtor dtor;
} prn_resource;

typedef struct {
	int resource_id;
	int ctx_id;
	grn_ctx *ctx;
	grn_obj *obj;
	grn_rc (*dtor)(grn_ctx *ctx, grn_obj *obj);
} prn_obj;

/* }}} */
/* {{{ internal function prototypes */

PRN_LOCAL ZEND_RSRC_DTOR_FUNC(prn_resource_dtor);
PRN_LOCAL void prn_resource_free(prn_resource *prsrc TSRMLS_DC);

PRN_LOCAL int prn_resource_register(int ctx_id, grn_ctx *ctx,
	int resource_id, void *ptr, prn_object_dtor dtor TSRMLS_DC);
PRN_LOCAL zval *prn_resource_zval(zval *zv, int ctx_id, grn_ctx *ctx,
	int resource_id, void *ptr, prn_object_dtor dtor TSRMLS_DC);

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
