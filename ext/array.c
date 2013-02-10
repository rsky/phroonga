/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "array.h"

/* {{{ globals */

PRN_LOCAL int le_grn_array = -1, le_grn_array_cursor = -1;
PRN_DECLARE_RSRC_NAME(prn_array_rsrc_name, grn_array);
PRN_DECLARE_RSRC_NAME(prn_array_cursor_rsrc_name, grn_array_cursor);

/* }}} */
/* {{{ prn_get_le_array() */

PHPAPI int prn_get_le_array(void)
{
	return le_grn_array;
}

/* }}} */
/* {{{ prn_get_le_array_cursor() */

PHPAPI int prn_get_le_array_cursor(void)
{
	return le_grn_array_cursor;
}

/* }}} */
/* {{{ _prn_array_fetch() */

PHPAPI grn_array *_prn_array_fetch(zval *zv TSRMLS_DC)
{
	prn_array *parray = prn_array_fetch_internal(zv TSRMLS_CC);

	if (parray) {
		return parray->array;
	}

	return NULL;
}

/* }}} */
/* {{{ _prn_array_cursor_fetch() */

PHPAPI grn_array_cursor *_prn_array_cursor_fetch(zval *zv TSRMLS_DC)
{
	prn_array_cursor *pcursor = prn_array_cursor_fetch_internal(zv TSRMLS_CC);

	if (pcursor) {
		return pcursor->cursor;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_register_array() */

PRN_LOCAL int prn_register_array(INIT_FUNC_ARGS)
{
	int resource_type1, resource_type2;
	resource_type1 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_array_rsrc_name, module_number);
	resource_type2 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_array_cursor_rsrc_name, module_number);
	if (resource_type1 == FAILURE || resource_type2 == FAILURE) {
		return FAILURE;
	}
	le_grn_array = resource_type1;
	le_grn_array_cursor = resource_type2;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_array_cursor_close() */

PRN_LOCAL grn_rc prn_array_cursor_close(grn_ctx *ctx, grn_array_cursor *cursor)
{
	grn_array_cursor_close(ctx, cursor);

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
