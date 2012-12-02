/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "obj.h"

/* {{{ globals */

static int le_grn_obj;

/* }}} */
/* {{{ function prototypes*/

static void prn_free_obj(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* }}} */
/* {{{ prn_get_le_obj() */

PHPAPI int prn_get_le_obj(void)
{
	return le_grn_obj;
}

/* }}} */
/* {{{ prn_fetch_obj() */

PHPAPI grn_obj *prn_fetch_obj(zval *zv TSRMLS_DC)
{
	prn_obj *pobj = prn_fetch_obj_internal(zv TSRMLS_CC);

	if (pobj) {
		return pobj->obj;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_fetch_obj_internal() */

PRN_LOCAL prn_obj *prn_fetch_obj_internal(zval *zv TSRMLS_DC)
{
	prn_obj *pobj;

	ZEND_FETCH_RESOURCE_NO_RETURN(pobj, prn_obj *, &zv, -1, "grn_obj", le_grn_obj);

	return pobj;
}

/* }}} */
/* {{{ prn_register_obj() */

PRN_LOCAL int prn_register_obj(INIT_FUNC_ARGS)
{
	int resource_id = zend_register_list_destructors_ex(
		prn_free_obj, NULL, "grn_obj", module_number);
	if (resource_id == FAILURE) {
		return FAILURE;
	}
	le_grn_obj = resource_id;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_free_obj() */

static void prn_free_obj(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	prn_obj *pobj = (prn_obj *)rsrc->ptr;

	/* delete from object counter table */
	if (sizeof(ulong) == sizeof(uintptr_t)) {
		uintptr_t h = (uintptr_t)pobj->obj;
		zend_hash_index_del(&PRNG(objects_ht), (ulong)h);
	} else {
		char arKey[32];
		uint nKeyLength = (uint)snprintf(arKey, 32, "%p", pobj->obj);
		zend_hash_del(&PRNG(objects_ht), arKey, nKeyLength);
	}

	grn_obj_unlink(pobj->ctx, pobj->obj);
	zend_list_delete(pobj->ctx_id);
	efree(rsrc->ptr);
}

/* }}} */
/* {{{ prn_obj_zval() */

PRN_LOCAL zval *prn_obj_zval(int ctx_id, grn_ctx *ctx, grn_obj *obj, zval *zv TSRMLS_DC)
{
	prn_obj *pobj;
	zval *retval;
	int obj_id = 0;
	zend_bool addref = 0;

	HashTable *ht = &PRNG(objects_ht);
	char arKey[32] = {'\0'};
	uint nKeyLength = 0;
	ulong h = 0UL;
	int *pData = NULL;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	/* find from object counter table */
	if (sizeof(ulong) == sizeof(uintptr_t)) {
		h = (ulong)((uintptr_t)obj);
	} else {
		nKeyLength = (uint)snprintf(arKey, 32, "%p", obj);
	}
	if (zend_hash_quick_find(ht, arKey, nKeyLength, h, (void **)&pData) == SUCCESS) {
		obj_id = *pData;
		addref = 1;
	}

	/* increment object reference count */
	if (addref) {
		zend_list_addref(obj_id);
		ZVAL_RESOURCE(retval, (long)obj_id);

		return retval;
	}

	pobj = (prn_obj *)emalloc(sizeof(prn_obj));
	pobj->ctx_id = ctx_id;
	pobj->ctx = ctx;
	pobj->obj = obj;

	ZEND_REGISTER_RESOURCE(retval, pobj, le_grn_obj);
	zend_list_addref(ctx_id);

	/* add to object counter table */
	obj_id = (int)Z_LVAL_P(retval);
	pData = &obj_id;
	zend_hash_quick_update(ht, arKey, nKeyLength, h, (void *)pData, sizeof(int), NULL);

	return retval;
}

/* }}} */
/* {{{ prn_obj_type_name */

PHPAPI const char *prn_obj_type_name(grn_obj *obj)
{
	switch (obj->header.type) {
		case GRN_VOID: return "void";
		case GRN_BULK: return "bulk";
		case GRN_PTR: return "ptr";
		case GRN_UVECTOR: return "uvector";
		case GRN_PVECTOR: return "pvector";
		case GRN_VECTOR: return "vector";
		case GRN_MSG: return "msg";
		case GRN_QUERY: return "query";
		case GRN_ACCESSOR: return "accessor";
		case GRN_ACCESSOR_VIEW: return "accessor_view";
		case GRN_SNIP: return "snip";
		case GRN_PATSNIP: return "patsnip";
		case GRN_STRING: return "string";
		case GRN_CURSOR_TABLE_HASH_KEY: return "cursor_table_hash_key";
		case GRN_CURSOR_TABLE_PAT_KEY: return "cursor_table_pat_key";
		case GRN_CURSOR_TABLE_DAT_KEY: return "cursor_table_dat_key";
		case GRN_CURSOR_TABLE_NO_KEY: return "cursor_table_no_key";
		case GRN_CURSOR_TABLE_VIEW: return "cursor_table_view";
		case GRN_CURSOR_COLUMN_INDEX: return "cursor_column_index";
		case GRN_CURSOR_COLUMN_GEO_INDEX: return "cursor_column_geo_index";
		case GRN_TYPE: return "type";
		case GRN_PROC: return "proc";
		case GRN_EXPR: return "expr";
		case GRN_TABLE_HASH_KEY: return "table_hash_key";
		case GRN_TABLE_PAT_KEY: return "table_pat_key";
		case GRN_TABLE_DAT_KEY: return "table_dat_key";
		case GRN_TABLE_NO_KEY: return "Table_no_key";
		case GRN_TABLE_VIEW: return "table_view";
		case GRN_DB: return "db";
		case GRN_COLUMN_FIX_SIZE: return "column_fix_size";
		case GRN_COLUMN_VAR_SIZE: return "column_var_size";
		case GRN_COLUMN_INDEX: return "column_index";
	}

	return NULL;
}

/* }}} */
/* {{{ grn_db_open() */

PRN_FUNCTION(grn_db_open)
{
	zval *zctx = NULL;
	const char *path = NULL;
	int path_len = 0;
	/*zval *zoptions = NULL;*/
	grn_ctx *ctx;
	grn_obj *db;
	grn_db_create_optarg *optarg = NULL;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs!",
			&zctx, &path, &path_len) == FAILURE
	) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	if (path_len == 0) {
		path = NULL;
	} else if (strlen(path) != path_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "given path contains NUL byte");
		return;
	}

	GRN_DB_OPEN_OR_CREATE(ctx, path, optarg, db);
	if (!db) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to open database: %s %s", prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(db, ctx, zctx);
}

/* }}} */
/* {{{ grn_db_touch() */

PRN_FUNCTION(grn_db_touch)
{
	zval *zdb = NULL;
	prn_obj *pobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb) == FAILURE) {
		return;
	}

	pobj = prn_fetch_obj_internal(zdb TSRMLS_CC);
	if (!pobj->obj || pobj->obj->header.type != GRN_DB) {
		return;
	}

	grn_db_touch(pobj->ctx, pobj->obj);
}

/* }}} */
/* {{{ grn_ctx_use() */

PRN_FUNCTION(grn_ctx_use)
{
	zval *zctx = NULL;
	zval *zdb = NULL;
	grn_ctx *ctx;
	prn_obj *pobj = NULL;
	grn_obj *db = NULL;
	grn_rc rc;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr!", &zctx, &zdb) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}
	if (ctx->flags & GRN_CTX_PER_DB) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"don't use with context that has GRN_CTX_PER_DB flag");
		return;
	}

	if (zdb) {
		pobj = prn_fetch_obj_internal(zdb TSRMLS_CC);
		if (!pobj) {
			return;
		}
		db = pobj->obj;
	}

	rc = grn_ctx_use(ctx, db);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to use database: %s %s", prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	if (pobj) {
		int ctx_id = (int)Z_LVAL_P(zctx);
		if (ctx_id != pobj->ctx_id) {
			zend_list_delete(pobj->ctx_id);
			zend_list_addref(ctx_id);
			pobj->ctx_id = ctx_id;
			pobj->ctx = ctx;
		}
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ grn_ctx_db() */

PRN_FUNCTION(grn_ctx_db)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	grn_obj *db;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	db = grn_ctx_db(ctx);
	if (!db) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(db, ctx, zctx);
}

/* }}} */
/* {{{ grn_ctx_get() */

PRN_FUNCTION(grn_ctx_get)
{
	zval *zctx = NULL;
	const char *name = NULL;
	int name_len = 0;
	grn_ctx *ctx;
	grn_obj *obj;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
			&zctx, &name, &name_len) == FAILURE
	) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	obj = grn_ctx_get(ctx, name, (unsigned int)name_len);
	if (!obj) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(obj, ctx, zctx);
}

/* }}} */
/* {{{ grn_ctx_at() */

PRN_FUNCTION(grn_ctx_at)
{
	zval *zctx = NULL;
	long id = 0L;
	grn_ctx *ctx;
	grn_obj *obj;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &id) == FAILURE) {
		return;
	}

	ctx = prn_fetch_ctx(zctx TSRMLS_CC);
	if (!ctx) {
		return;
	}

	obj = grn_ctx_at(ctx, (grn_id)id);
	if (!obj) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(obj, ctx, zctx);
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
