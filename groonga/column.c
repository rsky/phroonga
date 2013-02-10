/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "column.h"

/* {{{ grn_column_open() */

PRN_FUNCTION(grn_column_open)
{
	zval *ztable = NULL, *ztype = NULL;
	grn_ctx *ctx;
	prn_obj *intern;
	grn_obj *table, *type;
	const char *path = NULL, *name = NULL;
	int path_len = 0, name_len = 0;
	long flags = 0L;
	grn_obj *column;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s!lr!",
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

	GRN_COLUMN_OPEN_OR_CREATE(
		ctx, table, name, name_len, path,
		(grn_obj_flags)flags, type, column);
	if (!column) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to open column: %s %s", prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(column, ctx, intern->ctx_id);
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
