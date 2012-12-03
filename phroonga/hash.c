/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "hash.h"

/* {{{ globals */

PRN_LOCAL int le_grn_hash = -1, le_grn_hash_cursor = -1;
PRN_DECLARE_RSRC_NAME(prn_hash_rsrc_name, grn_hash);
PRN_DECLARE_RSRC_NAME(prn_hash_cursor_rsrc_name, grn_hash_cursor);

/* }}} */
/* {{{ prn_get_le_hash() */

PHPAPI int prn_get_le_hash(void)
{
	return le_grn_hash;
}

/* }}} */
/* {{{ prn_get_le_hash_cursor() */

PHPAPI int prn_get_le_hash_cursor(void)
{
	return le_grn_hash_cursor;
}

/* }}} */
/* {{{ prn_hash_fetch() */

PHPAPI grn_hash *prn_hash_fetch(zval *zv TSRMLS_DC)
{
	prn_hash *phash = prn_hash_fetch_internal(zv TSRMLS_CC);

	if (phash) {
		return phash->hash;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_hash_cursor_fetch() */

PHPAPI grn_hash_cursor *prn_hash_cursor_fetch(zval *zv TSRMLS_DC)
{
	prn_hash_cursor *pcursor = prn_hash_cursor_fetch_internal(zv TSRMLS_CC);

	if (pcursor) {
		return pcursor->cursor;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_hash_startup() */

PRN_LOCAL int prn_hash_startup(INIT_FUNC_ARGS)
{
	int resource_type1, resource_type2;
	resource_type1 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_hash_rsrc_name, module_number);
	resource_type2 = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_hash_cursor_rsrc_name, module_number);
	if (resource_type1 == FAILURE || resource_type2 == FAILURE) {
		return FAILURE;
	}
	le_grn_hash = resource_type1;
	le_grn_hash_cursor = resource_type2;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_hash_cursor_close() */

PRN_LOCAL grn_rc prn_hash_cursor_close(grn_ctx *ctx, grn_hash_cursor *cursor)
{
	grn_hash_cursor_close(ctx, cursor);

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
