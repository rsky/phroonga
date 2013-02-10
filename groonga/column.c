/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "column.h"

/* {{{ prn_column_open_or_create() */

static void prn_column_open_or_create(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *ztable = NULL, *ztype = NULL;
	grn_ctx *ctx = NULL;
	prn_obj *intern = NULL;
	grn_obj *table, *type = NULL;
	const char *path = NULL, *name = NULL;
	int path_len = 0, name_len = 0;
	long flags = 0L;
	grn_obj *column = NULL;
	const char *arg_format = NULL;
	const char * const arg_format_open_or_create = "rs|s!lr!";
	const char * const arg_format_open = "rs";

	RETVAL_NULL();

	if (mode == PRN_RESOURCE_OPEN) {
		arg_format = arg_format_open;
	} else {
		arg_format = arg_format_open_or_create;
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, arg_format,
			&ztable, &name, &name_len, &path, &path_len,
			&flags, &ztype) == FAILURE
	) {
		return;
	}

	intern = prn_obj_fetch_internal(ztable TSRMLS_CC);
	if (!intern) {
		return;
	}
	table = intern->obj;
	ctx = intern->ctx;
	if (ztype) {
		type = prn_obj_fetch(ztype);
	}

	if (path_len == 0) {
		path = NULL;
	} else if (!prn_check_path(path, path_len)) {
		return;
	}

	if (mode & PRN_RESOURCE_OPEN) {
		column = grn_obj_column(ctx, table, name, name_len);
		if (column) {
			switch (column->header.type) {
				case GRN_COLUMN_FIX_SIZE:
				case GRN_COLUMN_VAR_SIZE:
				case GRN_COLUMN_INDEX:
					/* OK */
					break;
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
						"'%s' is not a kind of column, but %s",
						name, prn_obj_type_name(column));
					return;
			}
		}
	}

	if (!column && (mode & PRN_RESOURCE_CREATE)) {
		column = grn_column_create(ctx, table, name, name_len,
			path, (grn_obj_flags)flags, type);
	}

	if (!column) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to %s column: %s %s",
			(mode & PRN_RESOURCE_CREATE) ? "create" : "open",
			prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(column, ctx, intern->ctx_id);
}

/* }}} */
/* {{{ grn_column_create() */

PRN_FUNCTION(grn_column_create)
{
	prn_column_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_CREATE);
}

/* }}} */
/* {{{ grn_column_open() */

PRN_FUNCTION(grn_column_open)
{
	prn_column_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_OPEN);
}

/* }}} */
/* {{{ grn_column_open_or_create() */

PRN_FUNCTION(grn_column_open_or_create)
{
	prn_column_open_or_create(INTERNAL_FUNCTION_PARAM_PASSTHRU, PRN_RESOURCE_OPEN_OR_CREATE);
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
