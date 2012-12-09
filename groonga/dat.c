/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "dat.h"

/* {{{ globals */

PRN_LOCAL int le_grn_dat = -1, le_grn_dat_cursor = -1;
PRN_DECLARE_RSRC_NAME(prn_dat_rsrc_name, grn_dat);
PRN_DECLARE_RSRC_NAME(prn_dat_cursor_rsrc_name, grn_dat_cursor);

/* }}} */
/* {{{ prn_get_le_dat() */

PHPAPI int prn_get_le_dat(void)
{
	return le_grn_dat;
}

/* }}} */
/* {{{ prn_get_le_dat_cursor() */

PHPAPI int prn_get_le_dat_cursor(void)
{
	return le_grn_dat_cursor;
}

/* }}} */
/* {{{ _prn_dat_fetch() */

PHPAPI grn_dat *_prn_dat_fetch(zval *zv TSRMLS_DC)
{
	prn_dat *pdat = prn_dat_fetch_internal(zv TSRMLS_CC);

	if (pdat) {
		return pdat->dat;
	}

	return NULL;
}

/* }}} */
/* {{{ _prn_dat_cursor_fetch() */

PHPAPI grn_dat_cursor *_prn_dat_cursor_fetch(zval *zv TSRMLS_DC)
{
	prn_dat_cursor *pcursor = prn_dat_cursor_fetch_internal(zv TSRMLS_CC);

	if (pcursor) {
		return pcursor->cursor;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_register_dat() */

PRN_LOCAL int prn_register_dat(INIT_FUNC_ARGS)
{
	int resource_type1, resource_type2;
	resource_type1 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_dat_rsrc_name, module_number);
	resource_type2 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_dat_cursor_rsrc_name, module_number);
	if (resource_type1 == FAILURE || resource_type2 == FAILURE) {
		return FAILURE;
	}
	le_grn_dat = resource_type1;
	le_grn_dat_cursor = resource_type2;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_dat_cursor_close() */

PRN_LOCAL grn_rc prn_dat_cursor_close(grn_ctx *ctx, grn_dat_cursor *cursor)
{
	grn_dat_cursor_close(ctx, cursor);

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
