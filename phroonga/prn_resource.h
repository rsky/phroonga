/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "private.h"

#ifndef PHROONGA_RESOURCE_H
#define PHROONGA_RESOURCE_H

BEGIN_EXTERN_C()

/* {{{ macros */

#if PHP_VERSION_ID < 50400
#define PRN_DECLARE_RSRC_NAME(name, type) PRN_LOCAL char * const name = #type
#define PRN_EXTERN_RSRC_NAME(name) extern PRN_LOCAL char * const name
#else
#define PRN_DECLARE_RSRC_NAME(name, type) PRN_LOCAL const char * const name = #type
#define PRN_EXTERN_RSRC_NAME(name) extern PRN_LOCAL const char * const name
#endif

/* }}} */
/* {{{ external globals */

extern PRN_LOCAL int le_grn_ctx;
extern PRN_LOCAL int le_grn_obj;
extern PRN_LOCAL int le_grn_snip;
extern PRN_LOCAL int le_grn_hash, le_grn_hash_cursor;
extern PRN_LOCAL int le_grn_array, le_grn_array_cursor;
extern PRN_LOCAL int le_grn_pat, le_grn_pat_cursor;
extern PRN_LOCAL int le_grn_dat, le_grn_dat_cursor;

PRN_EXTERN_RSRC_NAME(prn_ctx_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_obj_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_snip_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_hash_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_hash_cursor_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_array_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_array_cursor_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_pat_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_pat_cursor_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_dat_rsrc_name);
PRN_EXTERN_RSRC_NAME(prn_dat_cursor_rsrc_name);

/* }}} */
/* {{{ type definitions */

typedef grn_rc (*prn_resource_dtor)(grn_ctx *ctx, void *ptr);

typedef struct {
	int type;
	int owner_id;
	grn_ctx *ctx;
	union {
		void *ptr;
		grn_obj *obj;
		grn_snip *snip;
		grn_hash *hash;
		grn_hash_cursor *hash_cursor;
		grn_array *array;
		grn_array_cursor *array_cursor;
		grn_pat *pat;
		grn_pat_cursor *pat_cursor;
		grn_dat *dat;
		grn_dat_cursor *dat_cursor;
	} resource;
	prn_resource_dtor dtor;
} prn_resource;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_obj *obj;
	grn_rc (*dtor)(grn_ctx *ctx, grn_obj *obj);
} prn_obj;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_snip *snip;
	grn_rc (*dtor)(grn_ctx *ctx, grn_snip *snip);
} prn_snip;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_hash *hash;
	grn_rc (*dtor)(grn_ctx *ctx, grn_hash *hash);
} prn_hash;

typedef struct {
	int type;
	int hash_id;
	grn_ctx *ctx;
	grn_hash_cursor *cursor;
	grn_rc (*dtor)(grn_ctx *ctx, grn_hash_cursor *cursor);
} prn_hash_cursor;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_array *array;
	grn_rc (*dtor)(grn_ctx *ctx, grn_array *array);
} prn_array;

typedef struct {
	int type;
	int array_id;
	grn_ctx *ctx;
	grn_array_cursor *cursor;
	grn_rc (*dtor)(grn_ctx *ctx, grn_array_cursor *cursor);
} prn_array_cursor;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_pat *pat;
	grn_rc (*dtor)(grn_ctx *ctx, grn_pat *pat);
} prn_pat;

typedef struct {
	int type;
	int pat_id;
	grn_ctx *ctx;
	grn_pat_cursor *cursor;
	grn_rc (*dtor)(grn_ctx *ctx, grn_pat_cursor *cursor);
} prn_pat_cursor;

typedef struct {
	int type;
	int ctx_id;
	grn_ctx *ctx;
	grn_dat *dat;
	grn_rc (*dtor)(grn_ctx *ctx, grn_dat *dat);
} prn_dat;

typedef struct {
	int type;
	int dat_id;
	grn_ctx *ctx;
	grn_dat_cursor *cursor;
	grn_rc (*dtor)(grn_ctx *ctx, grn_dat_cursor *cursor);
} prn_dat_cursor;

/* }}} */
/* {{{ internal function prototypes */

PRN_LOCAL int prn_register_types(INIT_FUNC_ARGS);

PRN_LOCAL ZEND_RSRC_DTOR_FUNC(prn_resource_destroy);
PRN_LOCAL void prn_resource_free(prn_resource *prsrc TSRMLS_DC);

PRN_LOCAL int prn_resource_register(void *ptr, int type,
	int owner_id, grn_ctx *ctx, prn_resource_dtor dtor TSRMLS_DC);
PRN_LOCAL zval *prn_resource_zval(zval *zv, void *ptr, int type,
	int owner_id, grn_ctx *ctx, prn_resource_dtor dtor TSRMLS_DC);

PRN_LOCAL void prn_resource_ctx_zval(INTERNAL_FUNCTION_PARAMETERS);
PRN_LOCAL void prn_cursor_owner_zval(INTERNAL_FUNCTION_PARAMETERS);

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
