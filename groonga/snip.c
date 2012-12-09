/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "snip.h"

/* {{{ globals */

PRN_LOCAL int le_grn_snip = -1;
PRN_DECLARE_RSRC_NAME(prn_snip_rsrc_name, grn_snip);

/* }}} */
/* {{{ prn_get_le_snip() */

PHPAPI int prn_get_le_snip(void)
{
	return le_grn_snip;
}

/* }}} */
/* {{{ _prn_snip_fetch() */

PHPAPI grn_snip *_prn_snip_fetch(zval *zv TSRMLS_DC)
{
	prn_snip *psnip = prn_snip_fetch_internal(zv TSRMLS_CC);

	if (psnip) {
		return psnip->snip;
	}

	return NULL;
}

/* }}} */
/* {{{ prn_register_snip() */

PRN_LOCAL int prn_register_snip(INIT_FUNC_ARGS)
{
	int resource_type = zend_register_list_destructors_ex(
		prn_resource_destroy, NULL, prn_snip_rsrc_name, module_number);
	if (resource_type == FAILURE) {
		return FAILURE;
	}
	le_grn_snip = resource_type;

	return SUCCESS;
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
