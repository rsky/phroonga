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

static int le_grn_obj;

/* }}} */
/* {{{ function prototypes*/

static void prn_free_obj(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* }}} */
/* {{{ prn_get_le_obj() */

PHPAPI int prn_get_le_obj(void)
{
	return le_grn_obj;
}

/* }}} */
/* {{{ prn_fetch_obj() */

PHPAPI grn_obj *prn_fetch_obj(zval *zv TSRMLS_DC)
{
	prn_obj *pobj;

	ZEND_FETCH_RESOURCE_NO_RETURN(pobj, prn_obj *, &zv, -1, "grn_obj", le_grn_obj);

	return pobj->obj;
}

/* }}} */
/* {{{ prn_register_obj() */

PRN_LOCAL int prn_register_obj(INIT_FUNC_ARGS)
{
	int resource_id = zend_register_list_destructors_ex(
		prn_free_obj, NULL, "grn_obj", module_number);

	if (resource_id == FAILURE) {
		return FAILURE;
	}

	le_grn_obj = resource_id;

	return SUCCESS;
}

/* }}} */
/* {{{ prn_free_obj() */

static void prn_free_obj(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	prn_obj *pobj = (prn_obj *)rsrc->ptr;

	grn_obj_unlink(pobj->ctx, pobj->obj);
	efree(rsrc->ptr);
}

/* }}} */
/* {{{ prn_obj_zval() */

PRN_LOCAL zval *prn_obj_zval(grn_ctx *ctx, grn_obj *obj, zval *zv TSRMLS_DC)
{
	prn_obj *pobj;
	zval *retval;

	pobj = (prn_obj *)emalloc(sizeof(prn_obj));
	pobj->ctx = ctx;
	pobj->obj = obj;

	if (zv) {
		zval_dtor(zv);
		retval = zv;
	} else {
		MAKE_STD_ZVAL(retval);
	}

	ZEND_REGISTER_RESOURCE(retval, pobj, le_grn_obj);

	return retval;
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
