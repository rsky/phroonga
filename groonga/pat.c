/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "pat.h"

/* {{{ globals */

PRN_LOCAL int le_grn_pat = -1, le_grn_pat_cursor = -1;
PRN_DECLARE_RSRC_NAME(prn_pat_rsrc_name, grn_pat);
PRN_DECLARE_RSRC_NAME(prn_pat_cursor_rsrc_name, grn_pat_cursor);

/* }}} */
/* {{{ prn_get_le_pat() */

PHPAPI int prn_get_le_pat(void)
{
	return le_grn_pat;
}

/* }}} */
/* {{{ prn_get_le_pat_cursor() */

PHPAPI int prn_get_le_pat_cursor(void)
{
	return le_grn_pat_cursor;
}

/* }}} */
/* {{{ prn_pat_fetch() */

PHPAPI grn_pat *prn_pat_fetch(zval *zv TSRMLS_DC)
{
	prn_pat *ppat = prn_pat_fetch_internal(zv TSRMLS_CC);

	if (ppat) {
		return ppat->pat;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_pat_cursor_fetch() */

PHPAPI grn_pat_cursor *prn_pat_cursor_fetch(zval *zv TSRMLS_DC)
{
	prn_pat_cursor *pcursor = prn_pat_cursor_fetch_internal(zv TSRMLS_CC);

	if (pcursor) {
		return pcursor->cursor;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_register_pat() */

PRN_LOCAL int prn_register_pat(INIT_FUNC_ARGS)
{
	int resource_type1, resource_type2;
	resource_type1 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_pat_rsrc_name, module_number);
	resource_type2 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_pat_cursor_rsrc_name, module_number);
	if (resource_type1 == FAILURE || resource_type2 == FAILURE) {
		return FAILURE;
	}
	le_grn_pat = resource_type1;
	le_grn_pat_cursor = resource_type2;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_pat_cursor_close() */

PRN_LOCAL grn_rc prn_pat_cursor_close(grn_ctx *ctx, grn_pat_cursor *cursor)
{
	grn_pat_cursor_close(ctx, cursor);

	return GRN_SUCCESS;
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
