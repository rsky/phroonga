/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "table.h"

/* {{{ prn_table_open_or_create() */

static void prn_table_open_or_create(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *zctx = NULL, *zkey_type = NULL, *zvalue_type = NULL;
	grn_ctx *ctx = NULL;
	grn_obj *key_type = NULL, *value_type = NULL;
	int ctx_id = 0;
	const char *path = NULL, *name = NULL;
	int path_len = 0, name_len = 0;
	long flags = 0L;
	grn_obj *table = NULL;
	const char *arg_format = NULL;
	const char * const arg_format_open_or_create = "rs|s!lr!r!";
	const char * const arg_format_create = "rs!|s!lr!r!";
	const char * const arg_format_open = "rs";

	RETVAL_NULL();

	if (mode == PRN_RESOURCE_CREATE) {
		arg_format = arg_format_create;
	} else if (mode == PRN_RESOURCE_OPEN) {
		arg_format = arg_format_open;
	} else {
		arg_format = arg_format_open_or_create;
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, arg_format,
			&zctx, &name, &name_len, &path, &path_len,
			&flags, &zkey_type, &zvalue_type) == FAILURE
	) {
		return;
	}

	ctx = prn_ctx_fetch_ex(zctx, &ctx_id);
	if (!ctx) {
		return;
	}
	if (zkey_type) {
		key_type = prn_obj_fetch(zkey_type);
	}
	if (zvalue_type) {
		value_type = prn_obj_fetch(zvalue_type);
	}

	if (name_len == 0) {
		name = NULL;
	}
	if (path_len == 0) {
		path = NULL;
	} else if (!prn_check_path(path, path_len)) {
		return;
	}

	if (mode & PRN_RESOURCE_OPEN) {
		table = grn_ctx_get(ctx, name, name_len);
		if (table) {
			switch (table->header.type) {
				case GRN_TABLE_HASH_KEY:
				case GRN_TABLE_PAT_KEY:
				case GRN_TABLE_DAT_KEY:
				case GRN_TABLE_NO_KEY:
					/* OK */
					break;
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
						"'%s' is not a kind of table, but %s",
						name, prn_obj_type_name(table));
					return;
			}
		}
	}

	if (!table && (mode & PRN_RESOURCE_CREATE)) {
		table = grn_table_create(ctx, name, name_len, path,
			(grn_obj_flags)flags, key_type, value_type);
	}

	if (!table) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to %s table: %s %s",
			(mode & PRN_RESOURCE_CREATE) ? "create" : "open",
			prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(table, ctx, ctx_id);
}

/* }}} */
/* {{{ grn_table_create() */

PRN_FUNCTION(grn_table_create)
{
	prn_table_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_CREATE);
}

/* }}} */
/* {{{ grn_table_open() */

PRN_FUNCTION(grn_table_open)
{
	prn_table_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_OPEN);
}

/* }}} */
/* {{{ grn_table_open_or_create() */

PRN_FUNCTION(grn_table_open_or_create)
{
	prn_table_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_OPEN_OR_CREATE);
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
