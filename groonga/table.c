/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "table.h"

/* {{{ grn_table_open() */

PRN_FUNCTION(grn_table_open)
{
	zval *zctx = NULL, *zkey_type = NULL, *zvalue_type = NULL;
	grn_ctx *ctx;
	grn_obj *key_type = NULL, *value_type = NULL;
	int ctx_id = 0;
	const char *path = NULL, *name = NULL;
	int path_len = 0, name_len = 0;
	long flags = 0L;
	grn_obj *table;

	RETVAL_NULL();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s!s!lr!r!",
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

	GRN_TABLE_OPEN_OR_CREATE(
		ctx, name, name_len, path, (grn_obj_flags)flags,
		key_type, value_type, table);
	if (!table) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"failed to open table: %s %s", prn_errstr(ctx->rc), ctx->errbuf);
		return;
	}

	PRN_RETVAL_GRN_OBJ(table, ctx, ctx_id);
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
