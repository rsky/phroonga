/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"

#ifndef PHROONGA_OBJ_H
#define PHROONGA_OBJ_H

typedef struct {
	 grn_ctx *ctx;
	 grn_obj *obj;
} prn_obj;

PRN_LOCAL int prn_register_obj(INIT_FUNC_ARGS);
PRN_LOCAL zval *prn_obj_zval(grn_ctx *ctx, grn_obj *obj, zval *zv TSRMLS_DC);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
