/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "prn_resource.h"

/* {{{ prn_resource_destroy() */

PRN_LOCAL ZEND_RSRC_DTOR_FUNC(prn_resource_destroy)
{
	prn_resource_free((prn_resource *)rsrc->ptr TSRMLS_CC);
}

/* }}} */
/* {{{ prn_resource_free() */

PRN_LOCAL void prn_resource_free(prn_resource *prsrc TSRMLS_DC)
{
	void *ptr = prsrc->resource.ptr;

	/* delete from object counter table */
	if (sizeof(ulong) >= sizeof(uintptr_t)) {
		zend_hash_index_del(PRNG(resources_ht), (ulong)((uintptr_t)ptr));
	} else {
		char arKey[40] = {'\0'};
		uint nKeyLength = (uint)snprintf(arKey, sizeof(arKey), "%p", ptr);
		zend_hash_del(PRNG(resources_ht), arKey, nKeyLength);
	}

	prsrc->dtor(prsrc->ctx, ptr);
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

	HashTable *ht = PRNG(resources_ht);
	char arKey[40] = {'\0'};
	uint nKeyLength = 0;
	ulong h = 0UL;
	int *pData = NULL;

	/* find from object counter table */
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

	object_id = zend_list_insert(prsrc, type TSRMLS_CC);
	zend_list_addref(owner_id);

	/* add to object counter table */
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
		zval *zv;

		MAKE_STD_ZVAL(zv);
		ZVAL_RESOURCE(zv, (long)((prn_resource *)rsrc)->owner_id);

		rsrc = zend_fetch_resource(&zv TSRMLS_CC,
			-1, "grn_*", NULL, 4,
			le_grn_hash, le_grn_array, le_grn_pat, le_grn_dat);

		zval_ptr_dtor(&zv);
		if (!rsrc) {
			return;
		}

		ctx_id = ((prn_resource *)rsrc)->owner_id;
	} else {
		ctx_id = ((prn_resource *)rsrc)->owner_id;
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
	int owner_id = 0;
	int le_ctx = prn_get_le_ctx();

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
