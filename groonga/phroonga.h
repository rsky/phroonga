/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#ifndef PHROONGA_H
#define PHROONGA_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdint.h>
#include <groonga.h>

#define PHROONGA_VERSION "0.0.1"

BEGIN_EXTERN_C()

/* {{{ module globals */

ZEND_BEGIN_MODULE_GLOBALS(groonga)
	HashTable addr_id_map;
	int default_context_id;
ZEND_END_MODULE_GLOBALS(groonga)

ZEND_EXTERN_MODULE_GLOBALS(groonga)

#ifdef ZTS
#define PRNG(v) TSRMG(groonga_globals_id, zend_groonga_globals *, v)
#else
#define PRNG(v) (groonga_globals.v)
#endif

/* }}} */
/* {{{ resource API */

PHPAPI int prn_get_le_ctx(void);
PHPAPI grn_ctx *_prn_ctx_fetch(zval *zv, int *ctx_id TSRMLS_DC);
#define prn_ctx_fetch(zv) _prn_ctx_fetch((zv), NULL TSRMLS_CC)
#define prn_ctx_fetch_ex(zv, ctx_id) _prn_ctx_fetch((zv), (ctx_id) TSRMLS_CC)

PHPAPI int prn_get_le_obj(void);
PHPAPI grn_obj *_prn_obj_fetch(zval *zv TSRMLS_DC);
#define prn_obj_fetch(zv) _prn_obj_fetch((zv) TSRMLS_CC)

PHPAPI int prn_get_le_snip(void);
PHPAPI grn_snip *_prn_snip_fetch(zval *zv TSRMLS_DC);
#define prn_snip_fetch(zv) _prn_snip_fetch((zv) TSRMLS_CC)

PHPAPI int prn_get_le_hash(void);
PHPAPI int prn_get_le_hash_cursor(void);
PHPAPI grn_hash *_prn_hash_fetch(zval *zv TSRMLS_DC);
PHPAPI grn_hash_cursor *_prn_hash_cursor_fetch(zval *zv TSRMLS_DC);
#define prn_hash_fetch(zv) _prn_hash_fetch((zv) TSRMLS_CC)
#define prn_hash_cursor_fetch(zv) _prn_hash_cursor_fetch((zv) TSRMLS_CC)

PHPAPI int prn_get_le_array(void);
PHPAPI int prn_get_le_array_cursor(void);
PHPAPI grn_array *_prn_array_fetch(zval *zv TSRMLS_DC);
PHPAPI grn_array_cursor *_prn_array_cursor_fetch(zval *zv TSRMLS_DC);
#define prn_array_fetch(zv) _prn_array_fetch((zv) TSRMLS_CC)
#define prn_array_cursor_fetch(zv) _prn_array_cursor_fetch((zv) TSRMLS_CC)

PHPAPI int prn_get_le_pat(void);
PHPAPI int prn_get_le_pat_cursor(void);
PHPAPI grn_pat *_prn_pat_fetch(zval *zv TSRMLS_DC);
PHPAPI grn_pat_cursor *_prn_pat_cursor_fetch(zval *zv TSRMLS_DC);
#define prn_pat_fetch(zv) _prn_pat_fetch((zv) TSRMLS_CC)
#define prn_pat_cursor_fetch(zv) _prn_pat_cursor_fetch((zv) TSRMLS_CC)

PHPAPI int prn_get_le_dat(void);
PHPAPI int prn_get_le_dat_cursor(void);
PHPAPI grn_dat *_prn_dat_fetch(zval *zv TSRMLS_DC);
PHPAPI grn_dat_cursor *_prn_dat_cursor_fetch(zval *zv TSRMLS_DC);
#define prn_dat_fetch(zv) _prn_pat_fetch((zv) TSRMLS_CC)
#define prn_dat_cursor_fetch(zv) _prn_pat_cursor_fetch((zv) TSRMLS_CC)

/* }}} */
/* {{{ miscellanous API */

PHPAPI const char *prn_errstr(grn_rc rc);
PHPAPI const char *prn_obj_type_name(grn_obj *obj);

/* }}} */

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
