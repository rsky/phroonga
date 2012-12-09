/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>

#include "prn_ini.h"
#include "prn_constants.h"

#include "array.h"
#include "ctx.h"
#include "dat.h"
#include "expr.h"
#include "geo.h"
#include "hash.h"
#include "log.h"
#include "obj.h"
#include "pat.h"
#include "snip.h"

/* {{{ globals */

PRN_LOCAL ZEND_DECLARE_MODULE_GLOBALS(groonga)

#ifdef ZTS
PRN_LOCAL MUTEX_T phroonga_mutex = NULL;
#endif

/* }}} */
/* {{{ module function prototypes */

static PHP_MINIT_FUNCTION(groonga);
static PHP_MSHUTDOWN_FUNCTION(groonga);
static PHP_RINIT_FUNCTION(groonga);
static PHP_RSHUTDOWN_FUNCTION(groonga);
static PHP_MINFO_FUNCTION(groonga);
static PHP_GINIT_FUNCTION(groonga);
static PHP_GSHUTDOWN_FUNCTION(groonga);

/* }}} */
/* {{{ PHP function prototypes */

static PHP_FUNCTION(grn_get_version);
static PHP_FUNCTION(grn_get_package);
static PHP_FUNCTION(grn_get_default_encoding);
static PHP_FUNCTION(grn_get_default_command_version);
static PHP_FUNCTION(grn_get_default_match_escalation_threshold);

/* }}} */
/* {{{ argument informations (ctx) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_common, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, ctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_open, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_set_encoding, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_set_command_version, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, version)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_set_match_escalation_threshold, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, threshold)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ argument informations (obj) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_obj_common, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_db_common, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, db)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_db_open, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, path)
/*	ZEND_ARG_ARRAY_INFO(0, options, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_use, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, db)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_get, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ctx_at, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, ctx)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ argument informations (log) */

/* }}} */
/* {{{ function entry */

#define PRN_INTERNAL_FALIAS(name, alias, arg_info) \
	ZEND_FENTRY(name, alias, arg_info, 0)

static zend_function_entry phroonga_functions[] = {
	PHP_FE(grn_get_version, NULL)
	PHP_FE(grn_get_package, NULL)
	PHP_FE(grn_get_default_encoding, NULL)
	PHP_FE(grn_get_default_command_version, NULL)
	PHP_FE(grn_get_default_match_escalation_threshold, NULL)
	/* ctx */
	PHP_FE(grn_ctx_open, arginfo_ctx_open)
	PHP_FE(grn_ctx_get_encoding, arginfo_ctx_common)
	PHP_FE(grn_ctx_set_encoding, arginfo_ctx_set_encoding)
	PHP_FE(grn_ctx_get_command_version, arginfo_ctx_common)
	PHP_FE(grn_ctx_set_command_version, arginfo_ctx_set_command_version)
	PHP_FE(grn_ctx_get_match_escalation_threshold, arginfo_ctx_common)
	PHP_FE(grn_ctx_set_match_escalation_threshold, arginfo_ctx_set_match_escalation_threshold)
	/* obj */
	PRN_INTERNAL_FALIAS(grn_obj_ctx, prn_resource_ctx_zval, arginfo_obj_common)
	PHP_FE(grn_obj_type, arginfo_obj_common)
	PHP_FE(grn_obj_type_name, arginfo_obj_common)
	PHP_FE(grn_db_open, arginfo_db_open)
	PHP_FE(grn_db_touch, arginfo_db_common)
	PHP_FE(grn_ctx_use, arginfo_ctx_use)
	PHP_FE(grn_ctx_db, arginfo_ctx_common)
	PHP_FE(grn_ctx_get, arginfo_ctx_get)
	PHP_FE(grn_ctx_at, arginfo_ctx_at)
	/* geo */
	/* snip */
	/* log */
	PHP_FE(grn_default_logger_get_max_level, NULL)
	/* expr */
	/* hash */
	/* array */
	/* pat */
	/* dat */
	/* terminator */
	{ NULL, NULL, NULL, 0, 0 }
};

/* }}} */
/* {{{ module dependencies */

static zend_module_dep phroonga_deps[] = {
	{ NULL, NULL, NULL, 0 }
};

/* }}} */
/* {{{ ini entries */

PHP_INI_BEGIN()
PHP_INI_ENTRY("groonga.default_encoding", "",
	PHP_INI_SYSTEM, prn_update_default_encoding)
PHP_INI_ENTRY("groonga.default_command_version", "",
	PHP_INI_SYSTEM, prn_update_default_command_version)
PHP_INI_ENTRY("groonga.default_match_escalation_threshold", "0",
	PHP_INI_SYSTEM, prn_update_default_match_escalation_threshold)
PHP_INI_ENTRY("groonga.default_logger_max_level", "",
	PHP_INI_SYSTEM, prn_update_default_logger_max_level)
PHP_INI_END()

/* }}} */
/* {{{ module entry */

zend_module_entry groonga_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	phroonga_deps,
	"groonga",
	phroonga_functions,
	PHP_MINIT(groonga),
	PHP_MSHUTDOWN(groonga),
	PHP_RINIT(groonga),
	PHP_RSHUTDOWN(groonga),
	PHP_MINFO(groonga),
	PHROONGA_VERSION,
	PHP_MODULE_GLOBALS(groonga),
	PHP_GINIT(groonga),
	PHP_GSHUTDOWN(groonga),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_GROONGA
ZEND_GET_MODULE(groonga)
#endif

/* }}} */
/* {{{ PHP_MINIT_FUNCTION */

static PHP_MINIT_FUNCTION(groonga)
{
	if (grn_init() != GRN_SUCCESS) {
		return FAILURE;
	}

#ifdef ZTS
	phroonga_mutex = tsrm_mutex_alloc();
#endif

	prn_init_ini_param_tables();
	REGISTER_INI_ENTRIES();

	prn_register_constants(INIT_FUNC_ARGS_PASSTHRU);

	if (prn_register_types(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

/* }}} */
/* {{{ PHP_MSHUTDOWN_FUNCTION */

static PHP_MSHUTDOWN_FUNCTION(groonga)
{
	UNREGISTER_INI_ENTRIES();
	prn_destroy_ini_param_tables();

#ifdef ZTS
	tsrm_mutex_free(phroonga_mutex);
#endif

	if (grn_fin() != GRN_SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
}

/* }}} */
/* {{{ PHP_RINIT_FUNCTION */

static PHP_RINIT_FUNCTION(groonga)
{
	zend_hash_clean(&PRNG(addr_id_map));

	return SUCCESS;
}

/* }}} */
/* {{{ PHP_RSHUTDOWN_FUNCTION */

static PHP_RSHUTDOWN_FUNCTION(groonga)
{
	return SUCCESS;
}

/* }}} */
/* {{{ PHP_MINFO_FUNCTION */

static PHP_MINFO_FUNCTION(groonga)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "groonga support", "enabled");
	php_info_print_table_row(2, "groonga extension version", PHROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (compiled)", PRN_GROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (linking)", grn_get_version());
	php_info_print_table_row(2, "system plugins directory", grn_plugin_get_system_plugins_dir());
	php_info_print_table_row(2, "plugin suffix", grn_plugin_get_suffix());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/* }}} */
/* {{{ PHP_GINIT_FUNCTION */

static PHP_GINIT_FUNCTION(groonga)
{
	zend_hash_init(&groonga_globals->addr_id_map, 32, NULL, NULL, 1);
}

/* }}} */
/* {{{ PHP_GSHUTDOWN_FUNCTION */

static PHP_GSHUTDOWN_FUNCTION(groonga)
{
	zend_hash_destroy(&groonga_globals->addr_id_map);
}

/* }}} */
/* {{{ grn_get_version() */

static PHP_FUNCTION(grn_get_version)
{
	const char *version;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	version = grn_get_version();

	RETURN_STRING(version, 1);
}

/* }}} */
/* {{{ grn_get_package() */

static PHP_FUNCTION(grn_get_package)
{
	const char *package;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	package = grn_get_package();

	RETURN_STRING(package, 1);
}

/* }}} */
/* {{{ grn_get_default_encoding() */

static PHP_FUNCTION(grn_get_default_encoding)
{
	grn_encoding encoding;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PRN_MUTEX_LOCK();
	encoding = grn_get_default_encoding();
	PRN_MUTEX_UNLOCK();

	RETURN_LONG((long)encoding);
}

/* }}} */
/* {{{ grn_get_default_command_version() */

static PHP_FUNCTION(grn_get_default_command_version)
{
	grn_command_version version;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PRN_MUTEX_LOCK();
	version = grn_get_default_command_version();
	PRN_MUTEX_UNLOCK();

	RETURN_LONG((long)version);
}

/* }}} */
/* {{{ grn_get_default_match_escalation_threshold() */

static PHP_FUNCTION(grn_get_default_match_escalation_threshold)
{
	long long int threshold;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PRN_MUTEX_LOCK();
	threshold = grn_get_default_match_escalation_threshold();
	PRN_MUTEX_UNLOCK();

	if (LONG_MIN <= threshold && threshold <= LONG_MAX) {
		RETURN_LONG((long)threshold);
	} else {
		RETURN_DOUBLE((double)threshold);
	}
}

/* }}} */
/* {{{ prn_errstr() */

PHPAPI const char *prn_errstr(grn_rc rc)
{
	switch (rc) {
		case GRN_SUCCESS: return "SUCCESS";
		case GRN_END_OF_DATA: return "END_OF_DATA";
		case GRN_UNKNOWN_ERROR: return "UNKNOWN_ERROR";
		case GRN_OPERATION_NOT_PERMITTED: return "OPERATION_NOT_PERMITTED";
		case GRN_NO_SUCH_FILE_OR_DIRECTORY: return "NO_SUCH_FILE_OR_DIRECTORY";
		case GRN_NO_SUCH_PROCESS: return "NO_SUCH_PROCESS";
		case GRN_INTERRUPTED_FUNCTION_CALL: return "INTERRUPTED_FUNCTION_CALL";
		case GRN_INPUT_OUTPUT_ERROR: return "INPUT_OUTPUT_ERROR";
		case GRN_NO_SUCH_DEVICE_OR_ADDRESS: return "NO_SUCH_DEVICE_OR_ADDRESS";
		case GRN_ARG_LIST_TOO_LONG: return "ARG_LIST_TOO_LONG";
		case GRN_EXEC_FORMAT_ERROR: return "EXEC_FORMAT_ERROR";
		case GRN_BAD_FILE_DESCRIPTOR: return "BAD_FILE_DESCRIPTOR";
		case GRN_NO_CHILD_PROCESSES: return "NO_CHILD_PROCESSES";
		case GRN_RESOURCE_TEMPORARILY_UNAVAILABLE: return "RESOURCE_TEMPORARILY_UNAVAILABLE";
		case GRN_NOT_ENOUGH_SPACE: return "NOT_ENOUGH_SPACE";
		case GRN_PERMISSION_DENIED: return "PERMISSION_DENIED";
		case GRN_BAD_ADDRESS: return "BAD_ADDRESS";
		case GRN_RESOURCE_BUSY: return "RESOURCE_BUSY";
		case GRN_FILE_EXISTS: return "FILE_EXISTS";
		case GRN_IMPROPER_LINK: return "IMPROPER_LINK";
		case GRN_NO_SUCH_DEVICE: return "NO_SUCH_DEVICE";
		case GRN_NOT_A_DIRECTORY: return "NOT_A_DIRECTORY";
		case GRN_IS_A_DIRECTORY: return "IS_A_DIRECTORY";
		case GRN_INVALID_ARGUMENT: return "INVALID_ARGUMENT";
		case GRN_TOO_MANY_OPEN_FILES_IN_SYSTEM: return "TOO_MANY_OPEN_FILES_IN_SYSTEM";
		case GRN_TOO_MANY_OPEN_FILES: return "TOO_MANY_OPEN_FILES";
		case GRN_INAPPROPRIATE_I_O_CONTROL_OPERATION: return "INAPPROPRIATE_I_O_CONTROL_OPERATION";
		case GRN_FILE_TOO_LARGE: return "FILE_TOO_LARGE";
		case GRN_NO_SPACE_LEFT_ON_DEVICE: return "NO_SPACE_LEFT_ON_DEVICE";
		case GRN_INVALID_SEEK: return "INVALID_SEEK";
		case GRN_READ_ONLY_FILE_SYSTEM: return "READ_ONLY_FILE_SYSTEM";
		case GRN_TOO_MANY_LINKS: return "TOO_MANY_LINKS";
		case GRN_BROKEN_PIPE: return "BROKEN_PIPE";
		case GRN_DOMAIN_ERROR: return "DOMAIN_ERROR";
		case GRN_RESULT_TOO_LARGE: return "RESULT_TOO_LARGE";
		case GRN_RESOURCE_DEADLOCK_AVOIDED: return "RESOURCE_DEADLOCK_AVOIDED";
		case GRN_NO_MEMORY_AVAILABLE: return "NO_MEMORY_AVAILABLE";
		case GRN_FILENAME_TOO_LONG: return "FILENAME_TOO_LONG";
		case GRN_NO_LOCKS_AVAILABLE: return "NO_LOCKS_AVAILABLE";
		case GRN_FUNCTION_NOT_IMPLEMENTED: return "FUNCTION_NOT_IMPLEMENTED";
		case GRN_DIRECTORY_NOT_EMPTY: return "DIRECTORY_NOT_EMPTY";
		case GRN_ILLEGAL_BYTE_SEQUENCE: return "ILLEGAL_BYTE_SEQUENCE";
		case GRN_SOCKET_NOT_INITIALIZED: return "SOCKET_NOT_INITIALIZED";
		case GRN_OPERATION_WOULD_BLOCK: return "OPERATION_WOULD_BLOCK";
		case GRN_ADDRESS_IS_NOT_AVAILABLE: return "ADDRESS_IS_NOT_AVAILABLE";
		case GRN_NETWORK_IS_DOWN: return "NETWORK_IS_DOWN";
		case GRN_NO_BUFFER: return "NO_BUFFER";
		case GRN_SOCKET_IS_ALREADY_CONNECTED: return "SOCKET_IS_ALREADY_CONNECTED";
		case GRN_SOCKET_IS_NOT_CONNECTED: return "SOCKET_IS_NOT_CONNECTED";
		case GRN_SOCKET_IS_ALREADY_SHUTDOWNED: return "SOCKET_IS_ALREADY_SHUTDOWNED";
		case GRN_OPERATION_TIMEOUT: return "OPERATION_TIMEOUT";
		case GRN_CONNECTION_REFUSED: return "CONNECTION_REFUSED";
		case GRN_RANGE_ERROR: return "RANGE_ERROR";
		case GRN_TOKENIZER_ERROR: return "TOKENIZER_ERROR";
		case GRN_FILE_CORRUPT: return "FILE_CORRUPT";
		case GRN_INVALID_FORMAT: return "INVALID_FORMAT";
		case GRN_OBJECT_CORRUPT: return "OBJECT_CORRUPT";
		case GRN_TOO_MANY_SYMBOLIC_LINKS: return "TOO_MANY_SYMBOLIC_LINKS";
		case GRN_NOT_SOCKET: return "NOT_SOCKET";
		case GRN_OPERATION_NOT_SUPPORTED: return "OPERATION_NOT_SUPPORTED";
		case GRN_ADDRESS_IS_IN_USE: return "ADDRESS_IS_IN_USE";
		case GRN_ZLIB_ERROR: return "ZLIB_ERROR";
		case GRN_LZO_ERROR: return "LZO_ERROR";
		case GRN_STACK_OVER_FLOW: return "STACK_OVER_FLOW";
		case GRN_SYNTAX_ERROR: return "SYNTAX_ERROR";
		case GRN_RETRY_MAX: return "RETRY_MAX";
		case GRN_INCOMPATIBLE_FILE_FORMAT: return "INCOMPATIBLE_FILE_FORMAT";
		case GRN_UPDATE_NOT_ALLOWED: return "UPDATE_NOT_ALLOWED";
		case GRN_TOO_SMALL_OFFSET: return "TOO_SMALL_OFFSET";
		case GRN_TOO_LARGE_OFFSET: return "TOO_LARGE_OFFSET";
		case GRN_TOO_SMALL_LIMIT: return "TOO_SMALL_LIMIT";
		case GRN_CAS_ERROR: return "CAS_ERROR";
		case GRN_UNSUPPORTED_COMMAND_VERSION: return "UNSUPPORTED_COMMAND_VERSION";
	}

	return "(undefined)";
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
