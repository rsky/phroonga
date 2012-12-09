/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "prn_resource.h"
#include "array.h"
#include "ctx.h"
#include "dat.h"
#include "expr.h"
#include "geo.h"
#include "hash.h"
#include "log.h"
#include "obj.h"
#include "pat.h"
#include "snip.h"

/* {{{ prn_resource_destroy() */

PRN_LOCAL ZEND_RSRC_DTOR_FUNC(prn_resource_destroy)
{
	prn_resource_free((prn_resource *)rsrc->ptr TSRMLS_CC);
}

/* }}} */
/* {{{ prn_resource_free() */

PRN_LOCAL void prn_resource_free(prn_resource *prsrc TSRMLS_DC)
{
	grn_ctx *ctx = prsrc->ctx;
	void *ptr = prsrc->resource.ptr;

	/* delete from address-id map */
	if (sizeof(ulong) >= sizeof(uintptr_t)) {
		zend_hash_index_del(&PRNG(addr_id_map), (ulong)((uintptr_t)ptr));
	} else {
		char arKey[40] = {'\0'};
		uint nKeyLength = (uint)snprintf(arKey, sizeof(arKey), "%p", ptr);
		zend_hash_del(&PRNG(addr_id_map), arKey, nKeyLength);
	}

	/* deactivate databse */
	if (prsrc->type == le_grn_obj) {
		grn_obj *obj = prsrc->resource.obj;
		if (obj->header.type == GRN_DB) {
			if (!(ctx->flags & GRN_CTX_PER_DB)) {
				grn_ctx_use(ctx, NULL);
			}
		}
	}

	prsrc->dtor(ctx, ptr);
	zend_list_delete(prsrc->owner_id);
	efree(prsrc);
}

/* }}} */
/* {{{ prn_resource_register() */

PRN_LOCAL int prn_resource_register(void *ptr, int type,
	int owner_id, grn_ctx *ctx, prn_resource_dtor dtor TSRMLS_DC)
{
	prn_resource *prsrc;
	int object_id;

	HashTable *ht = &PRNG(addr_id_map);
	char arKey[40] = {'\0'};
	uint nKeyLength = 0;
	ulong h = 0UL;
	int *pData = NULL;

	/* find from address-id map */
	if (sizeof(ulong) >= sizeof(uintptr_t)) {
		h = (ulong)((uintptr_t)ptr);
	} else {
		nKeyLength = (uint)snprintf(arKey, sizeof(arKey), "%p", ptr);
	}

	if (zend_hash_quick_find(ht, arKey, nKeyLength, h, (void **)&pData) == SUCCESS) {
		/* increment reference count and return */
		object_id = *pData;
		zend_list_addref(object_id);

		return object_id;
	}

	/* create new resource and register it */
	prsrc = (prn_resource *)emalloc(sizeof(prn_resource));
	prsrc->type = type;
	prsrc->owner_id = owner_id;
	prsrc->ctx = ctx;
	prsrc->resource.ptr = ptr;
	prsrc->dtor = dtor;

#if PHP_VERSION_ID >= 50400
	object_id = zend_list_insert(prsrc, type TSRMLS_CC);
#else
	object_id = zend_list_insert(prsrc, type);
#endif
	zend_list_addref(owner_id);

	/* add to address-id map */
	pData = &object_id;
	zend_hash_quick_update(ht, arKey, nKeyLength, h, (void *)pData, sizeof(int), NULL);

	return object_id;
}

/* }}} */
/* {{{ prn_resource_zval() */

PRN_LOCAL zval *prn_resource_zval(zval *zv, void *ptr, int type,
	int owner_id, grn_ctx *ctx, prn_resource_dtor dtor TSRMLS_DC)
{
	zval *retval;
	int object_id;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	object_id = prn_resource_register(ptr, type, owner_id, ctx, dtor TSRMLS_CC);
	ZVAL_RESOURCE(retval, (long)object_id);

	return retval;
}

/* }}} */
/* {{{ prn_resource_ctx_zval() */

PRN_LOCAL void prn_resource_ctx_zval(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zobj = NULL;
	void *rsrc = NULL;
	int resource_type = 0;
	int ctx_id = 0;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zobj) == FAILURE) {
		return;
	}

	rsrc = zend_fetch_resource(&zobj TSRMLS_CC,
		-1, "grn_*", &resource_type, 11,
		le_grn_ctx, le_grn_obj, le_grn_snip,
		le_grn_hash, le_grn_hash_cursor,
		le_grn_array, le_grn_array_cursor,
		le_grn_pat, le_grn_pat_cursor,
		le_grn_dat, le_grn_dat_cursor);
	if (!rsrc) {
		return;
	}

	if (resource_type == le_grn_ctx) {
		ctx_id = (int)Z_LVAL_P(zobj);
	} else if (
		   resource_type == le_grn_hash_cursor
		|| resource_type == le_grn_array_cursor
		|| resource_type == le_grn_pat_cursor
		|| resource_type == le_grn_dat_cursor
	) {
		int owner_id = ((prn_resource *)rsrc)->owner_id;
		int owner_type = 0;
		prn_resource *owner = (prn_resource *)zend_list_find(owner_id, &owner_type);
		if (!owner
			|| (resource_type == le_grn_hash_cursor  && owner_type != le_grn_hash)
			|| (resource_type == le_grn_array_cursor && owner_type != le_grn_array)
			|| (resource_type == le_grn_pat_cursor   && owner_type != le_grn_dat)
			|| (resource_type == le_grn_dat_cursor   && owner_type != le_grn_pat)
		) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "cursor owner is already closed");
			return;
		}

		ctx_id = owner->owner_id;
	} else {
		ctx_id = ((prn_resource *)rsrc)->owner_id;
	}

	if (resource_type != le_grn_ctx) {
		int type = 0;
		void *ctx = zend_list_find(ctx_id, &type);
		if (!ctx || type != le_grn_ctx) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "context is already closed");
			return;
		}
	}

	zend_list_addref(ctx_id);

	RETURN_RESOURCE((long)ctx_id);
}

/* }}} */
/* {{{ prn_cursor_owner_zval() */

PRN_LOCAL void prn_cursor_owner_zval(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zcursor = NULL;
	prn_resource *prsrc = NULL;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcursor) == FAILURE) {
		return;
	}

	prsrc = (prn_resource *)zend_fetch_resource(&zcursor TSRMLS_CC,
		-1, "grn_*_cursor", NULL, 4,
		le_grn_hash_cursor, le_grn_array_cursor,
		le_grn_pat_cursor, le_grn_dat_cursor);
	if (!prsrc) {
		return;
	}

	zend_list_addref(prsrc->owner_id);

	RETURN_RESOURCE((long)prsrc->owner_id);
}

/* }}} */
/* {{{ prn_register_types() */

PRN_LOCAL int prn_register_types(INIT_FUNC_ARGS)
{
	if (prn_register_ctx(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
	if (prn_register_obj(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
/*
	if (prn_register_geo(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
*/
	if (prn_register_snip(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
/*
	if (prn_register_log(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
*/
/*
	if (prn_register_expr(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
*/
	if (prn_register_hash(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
	if (prn_register_array(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
	if (prn_register_pat(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}
	if (prn_register_dat(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
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
