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

PRN_LOCAL int le_grn_obj = -1;
PRN_DECLARE_RSRC_NAME(prn_obj_rsrc_name, grn_obj);

/* }}} */
/* {{{ prn_get_le_obj() */

PHPAPI int prn_get_le_obj(void)
{
	return le_grn_obj;
}

/* }}} */
/* {{{ _prn_obj_fetch() */

PHPAPI grn_obj *_prn_obj_fetch(zval *zv TSRMLS_DC)
{
	prn_obj *pobj = prn_obj_fetch_internal(zv TSRMLS_CC);

	if (pobj) {
		return pobj->obj;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_register_obj() */

PRN_LOCAL int prn_register_obj(INIT_FUNC_ARGS)
{
	int resource_type = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_obj_rsrc_name, module_number);
	if (resource_type == FAILURE) {
		return FAILURE;
	}
	le_grn_obj = resource_type;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_obj_unlink() */

PRN_LOCAL grn_rc prn_obj_unlink(grn_ctx *ctx, grn_obj *obj)
{
	grn_obj_unlink(ctx, obj);

	return GRN_SUCCESS;
}

/* }}} */
/* {{{ prn_obj_type_name() */

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
		case GRN_SNIP: return "snip";
		case GRN_PATSNIP: return "patsnip";
		case GRN_STRING: return "string";
		case GRN_CURSOR_TABLE_HASH_KEY: return "cursor_table_hash_key";
		case GRN_CURSOR_TABLE_PAT_KEY: return "cursor_table_pat_key";
		case GRN_CURSOR_TABLE_DAT_KEY: return "cursor_table_dat_key";
		case GRN_CURSOR_TABLE_NO_KEY: return "cursor_table_no_key";
		case GRN_CURSOR_COLUMN_INDEX: return "cursor_column_index";
		case GRN_CURSOR_COLUMN_GEO_INDEX: return "cursor_column_geo_index";
		case GRN_TYPE: return "type";
		case GRN_PROC: return "proc";
		case GRN_EXPR: return "expr";
		case GRN_TABLE_HASH_KEY: return "table_hash_key";
		case GRN_TABLE_PAT_KEY: return "table_pat_key";
		case GRN_TABLE_DAT_KEY: return "table_dat_key";
		case GRN_TABLE_NO_KEY: return "table_no_key";
		case GRN_DB: return "db";
		case GRN_COLUMN_FIX_SIZE: return "column_fix_size";
		case GRN_COLUMN_VAR_SIZE: return "column_var_size";
		case GRN_COLUMN_INDEX: return "column_index";
	}

	return NULL;
}

/* }}} */
/* {{{ grn_obj_type() */

PRN_FUNCTION(grn_obj_type)
{
	zval *zobj = NULL;
	grn_obj *obj;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zobj) == FAILURE) {
		return;
	}

	obj = prn_obj_fetch(zobj);
	if (!obj) {
		return;
	}

	RETURN_LONG((long)obj->header.type);
}

/* }}} */
/* {{{ grn_obj_type_name() */

PRN_FUNCTION(grn_obj_type_name)
{
	PHP_FN(grn_obj_type)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (Z_TYPE_P(return_value) == IS_LONG) {
		grn_obj obj = { .header.type = (unsigned char)Z_LVAL_P(return_value) };
		const char *name = prn_obj_type_name(&obj);
		if (name) {
			RETURN_STRING(name, 1);
		}
	}
}

/* }}} */
/* {{{ grn_db_open() */

PRN_FUNCTION(grn_db_open)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	int ctx_id = 0;
	const char *path = NULL;
	int path_len = 0;
	/*zval *zoptions = NULL;*/
	grn_db_create_optarg *optarg = NULL;
	grn_obj *db;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!s!",
			&zctx, &path, &path_len) == FAILURE
	) {
		return;
	}

	ctx = prn_ctx_fetch_ex(zctx, &ctx_id);
	if (!ctx) {
		return;
	}

	if (path_len == 0) {
		path = NULL;
	} else if (!prn_check_path(path, path_len)) {
		return;
	}

	GRN_DB_OPEN_OR_CREATE(ctx, path, optarg, db);
	if (!db) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to open database: %s %s", prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(db, ctx, ctx_id);
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

	pobj = prn_obj_fetch_internal(zdb TSRMLS_CC);
	if (!pobj->obj || pobj->obj->header.type != GRN_DB) {
		return;
	}

	grn_db_touch(pobj->ctx, pobj->obj);
}

/* }}} */
/* {{{ grn_db_use() */

PRN_FUNCTION(grn_db_use)
{
	zval *zdb = NULL;
	prn_obj *pobj = NULL;
	grn_obj *db = NULL;
	grn_rc rc;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb) == FAILURE) {
		return;
	}

	pobj = prn_obj_fetch_internal(zdb TSRMLS_CC);
	if (!pobj) {
		return;
	}

	rc = grn_ctx_use(pobj->ctx, pobj->obj);
	if (rc != GRN_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to use database: %s %s",
			prn_errstr(pobj->ctx->rc), pobj->ctx->errbuf);
		return;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ grn_ctx_db() */

PRN_FUNCTION(grn_ctx_db)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	int ctx_id = 0;
	grn_obj *db;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!", &zctx) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_ex(zctx, &ctx_id);
	if (!ctx) {
		return;
	}

	db = grn_ctx_db(ctx);
	if (!db) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(db, ctx, ctx_id);
}

/* }}} */
/* {{{ grn_ctx_get() */

PRN_FUNCTION(grn_ctx_get)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	int ctx_id = 0;
	const char *name = NULL;
	int name_len = 0;
	grn_obj *obj;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!s",
			&zctx, &name, &name_len) == FAILURE
	) {
		return;
	}

	ctx = prn_ctx_fetch_ex(zctx, &ctx_id);
	if (!ctx) {
		return;
	}

	obj = grn_ctx_get(ctx, name, (unsigned int)name_len);
	if (!obj) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(obj, ctx, ctx_id);
}

/* }}} */
/* {{{ grn_ctx_at() */

PRN_FUNCTION(grn_ctx_at)
{
	zval *zctx = NULL;
	grn_ctx *ctx;
	int ctx_id = 0;
	long id = 0L;
	grn_obj *obj;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!l", &zctx, &id) == FAILURE) {
		return;
	}

	ctx = prn_ctx_fetch_ex(zctx, &ctx_id);
	if (!ctx) {
		return;
	}

	obj = grn_ctx_at(ctx, (grn_id)id);
	if (!obj) {
		return;
	}

	PRN_RETVAL_GRN_OBJ(obj, ctx, ctx_id);
}

/* }}} */
/* {{{ grn_plugin_get_system_plugins_dir() */

PRN_FUNCTION(grn_plugin_get_system_plugins_dir)
{
	const char *path;

	if (ZEND_NUM_ARGS() != 0) {
		RETVAL_FALSE;
		WRONG_PARAM_COUNT;
	}

	path = grn_plugin_get_system_plugins_dir();
	RETURN_STRING(path, 1);
}

/* }}} */
/* {{{ grn_plugin_get_suffix() */

PRN_FUNCTION(grn_plugin_get_suffix)
{
	const char *path;

	if (ZEND_NUM_ARGS() != 0) {
		RETVAL_FALSE;
		WRONG_PARAM_COUNT;
	}

	path = grn_plugin_get_suffix();
	RETURN_STRING(path, 1);
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
