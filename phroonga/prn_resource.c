/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "prn_resource.h"

/* {{{ prn_resource_dtor() */

PRN_LOCAL ZEND_RSRC_DTOR_FUNC(prn_resource_dtor)
{
	prn_resource_free((prn_resource *)rsrc->ptr TSRMLS_CC);
}

/* }}} */
/* {{{ prn_resource_free() */

PRN_LOCAL void prn_resource_free(prn_resource *prsrc TSRMLS_DC)
{
	void *ptr = prsrc->value.ptr;

	/* delete from object counter table */
	if (sizeof(ulong) >= sizeof(uintptr_t)) {
		zend_hash_index_del(PRNG(resources_ht), (ulong)((uintptr_t)ptr));
	} else {
		char arKey[40] = {'\0'};
		uint nKeyLength = (uint)snprintf(arKey, sizeof(arKey), "%p", ptr);
		zend_hash_del(PRNG(resources_ht), arKey, nKeyLength);
	}

	prsrc->dtor(prsrc->ctx, ptr);
	zend_list_delete(prsrc->ctx_id);
	efree(prsrc);
}

/* }}} */
/* {{{ prn_resource_register() */

PRN_LOCAL int prn_resource_register(int ctx_id, grn_ctx *ctx,
	int resource_id, void *ptr, prn_object_dtor dtor TSRMLS_DC)
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
	prsrc->resource_id = resource_id;
	prsrc->ctx_id = ctx_id;
	prsrc->ctx = ctx;
	prsrc->value.ptr = ptr;
	prsrc->dtor = dtor;

	object_id = zend_list_insert(prsrc, resource_id TSRMLS_CC);
	zend_list_addref(ctx_id);

	/* add to object counter table */
	pData = &object_id;
	zend_hash_quick_update(ht, arKey, nKeyLength, h, (void *)pData, sizeof(int), NULL);

	return object_id;
}

/* }}} */
/* {{{ prn_resource_zval() */

PRN_LOCAL zval *prn_resource_zval(zval *zv, int ctx_id, grn_ctx *ctx,
	int resource_id, void *ptr, prn_object_dtor dtor TSRMLS_DC)
{
	zval *retval;
	int object_id;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	object_id = prn_resource_register(ctx_id, ctx,
		resource_id, ptr, dtor TSRMLS_CC);
	ZVAL_RESOURCE(retval, (long)object_id);

	return retval;
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
