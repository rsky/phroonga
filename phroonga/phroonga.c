/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"
#include <php_ini.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>
#include "array.h"
#include "ctx.h"
#include "dat.h"
#include "expr.h"
#include "geo.h"
#include "hash.h"
#include "log.h"
#include "obj.h"
#include "pat.h"
#include <php_ini.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>

/* {{{ globals */

PRN_LOCAL ZEND_DECLARE_MODULE_GLOBALS(phroonga)

/* }}} */
/* {{{ miscellaneous macros */

#define PRN_REGISTER_CONSTANT(name) \
	REGISTER_LONG_CONSTANT(#name, name, CONST_PERSISTENT | CONST_CS)

#define PRN_INI_HASH_KEY_MAX_SIZE 32

#define PRN_INI_HASH_ADD(ht, value) \
	prn_ini_non_ts_hash_add(TS_HASH(ht), #value, value)

#define PRN_INI_HASH_ADD_ALIAS(ht, value, alias) \
	prn_ini_non_ts_hash_add(TS_HASH(ht), alias, value)

#define PRN_INI_MH_ARGS_PASSTHRU entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC

/* }}} */
/* {{{ function prototypes (module) */

static PHP_MINIT_FUNCTION(phroonga);
static PHP_MSHUTDOWN_FUNCTION(phroonga);
static PHP_RINIT_FUNCTION(phroonga);
static PHP_RSHUTDOWN_FUNCTION(phroonga);
static PHP_MINFO_FUNCTION(phroonga);
static PHP_GINIT_FUNCTION(phroonga);
static PHP_GSHUTDOWN_FUNCTION(phroonga);

/* }}} */
/* {{{ function prototypes (internal) */

/* functions to register constants */

static void prn_register_constants(INIT_FUNC_ARGS);

static void prn_register_enum_rc(INIT_FUNC_ARGS);
static void prn_register_enum_encoding(INIT_FUNC_ARGS);
static void prn_register_enum_command_version(INIT_FUNC_ARGS);
static void prn_register_enum_log_level(INIT_FUNC_ARGS);
static void prn_register_enum_content_type(INIT_FUNC_ARGS);
static void prn_register_enum_builtin_type(INIT_FUNC_ARGS);
static void prn_register_enum_builtin_tokenizer(INIT_FUNC_ARGS);
static void prn_register_enum_proc_type(INIT_FUNC_ARGS);
static void prn_register_enum_operator(INIT_FUNC_ARGS);
static void prn_register_enum_info_type(INIT_FUNC_ARGS);
static void prn_register_enum_hook_entry(INIT_FUNC_ARGS);

static void prn_register_ctx_init_flags(INIT_FUNC_ARGS);
static void prn_register_table_cursor_open_flags(INIT_FUNC_ARGS);
static void prn_register_table_sort_flags(INIT_FUNC_ARGS);
static void prn_register_table_group_flags(INIT_FUNC_ARGS);
static void prn_register_obj_set_value_flags(INIT_FUNC_ARGS);
static void prn_register_snip_open_flags(INIT_FUNC_ARGS);
static void prn_register_logger_info_flags(INIT_FUNC_ARGS);
static void prn_register_obj_format_flags(INIT_FUNC_ARGS);
static void prn_register_expr_flags(INIT_FUNC_ARGS);
static void prn_register_ctx_cnnect_flags(INIT_FUNC_ARGS);

/* functions to handle ini entries */
static PHP_INI_MH(prn_update_default_encoding);
static PHP_INI_MH(prn_update_default_command_version);
static PHP_INI_MH(prn_update_default_match_escalation_threshold);
static PHP_INI_MH(prn_update_default_logger_set_max_level);
static void prn_init_encodings_ht(TsHashTable *ht TSRMLS_DC);
static void prn_init_command_versions_ht(TsHashTable *ht TSRMLS_DC);
static void prn_init_log_levels_ht(TsHashTable *ht TSRMLS_DC);
static void prn_ini_non_ts_hash_add(HashTable *ht, const char *key, int value);
static zend_bool prn_ini_ts_hash_find(TsHashTable *ht, const char *key, uint length, int *pValue);

/* }}} */
/* {{{ function prototypes (PHP) */

static PHP_FUNCTION(grn_get_version);
static PHP_FUNCTION(grn_get_package);
static PHP_FUNCTION(grn_get_default_encoding);
static PHP_FUNCTION(grn_get_default_command_version);
static PHP_FUNCTION(grn_get_default_match_escalation_threshold);

/* }}} */
/* {{{ argument informations */

/* }}} */
/* {{{ argument informations (log) */

/* }}} */
/* {{{ function entry */

static zend_function_entry phroonga_functions[] = {
	PHP_FE(grn_get_version, NULL)
	PHP_FE(grn_get_package, NULL)
	PHP_FE(grn_get_default_encoding, NULL)
	PHP_FE(grn_get_default_command_version, NULL)
	PHP_FE(grn_get_default_match_escalation_threshold, NULL)
	/* ctx */
	/* obj */
	/* geo */
	/* log */
	PHP_FE(grn_default_logger_get_max_level, NULL)
	/* expr */
	/* hash */
	/* array */
	/* pat */
	/* dat */
	/* terminator */
	{ NULL, NULL, NULL }
};

/* }}} */
/* {{{ module dependencies */

static zend_module_dep phroonga_deps[] = {
	{NULL, NULL, NULL, 0}
};

/* }}} */
/* {{{ ini entries */

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY(
	"phroonga.default_encoding", "", PHP_INI_SYSTEM,
	prn_update_default_encoding, default_encoding,
	zend_phroonga_globals, phroonga_globals)
STD_PHP_INI_ENTRY(
	"phroonga.default_command_version", "", PHP_INI_SYSTEM,
	prn_update_default_command_version, default_command_version,
	zend_phroonga_globals, phroonga_globals)
STD_PHP_INI_ENTRY(
	"phroonga.default_match_escalation_threshold", "0", PHP_INI_SYSTEM,
	prn_update_default_match_escalation_threshold, default_match_escalation_threshold,
	zend_phroonga_globals, phroonga_globals)
STD_PHP_INI_ENTRY(
	"phroonga.default_logger_max_level", "", PHP_INI_SYSTEM,
	prn_update_default_logger_set_max_level, default_logger_max_level,
	zend_phroonga_globals, phroonga_globals)
PHP_INI_END()

/* }}} */
/* {{{ module entry */

zend_module_entry phroonga_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	phroonga_deps,
	"phroonga",
	phroonga_functions,
	PHP_MINIT(phroonga),
	PHP_MSHUTDOWN(phroonga),
	PHP_RINIT(phroonga),
	PHP_RSHUTDOWN(phroonga),
	PHP_MINFO(phroonga),
	PHROONGA_VERSION,
	PHP_MODULE_GLOBALS(phroonga),
	PHP_GINIT(phroonga),
	PHP_GSHUTDOWN(phroonga),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_PHROONGA
ZEND_GET_MODULE(phroonga)
#endif

/* }}} */
/* {{{ PHP_MINIT_FUNCTION */

static PHP_MINIT_FUNCTION(phroonga)
{
	if (grn_init() != GRN_SUCCESS) {
		return FAILURE;
	}

	REGISTER_INI_ENTRIES();

	prn_register_constants(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}

/* }}} */
/* {{{ PHP_MSHUTDOWN_FUNCTION */

static PHP_MSHUTDOWN_FUNCTION(phroonga)
{
	UNREGISTER_INI_ENTRIES();

	if (grn_fin() != GRN_SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
}

/* }}} */
/* {{{ PHP_RINIT_FUNCTION */

static PHP_RINIT_FUNCTION(phroonga)
{
	return SUCCESS;
}

/* }}} */
/* {{{ PHP_RSHUTDOWN_FUNCTION */

static PHP_RSHUTDOWN_FUNCTION(phroonga)
{
	return SUCCESS;
}

/* }}} */
/* {{{ PHP_MINFO_FUNCTION */

static PHP_MINFO_FUNCTION(phroonga)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "phroonga support", "enabled");
	php_info_print_table_row(2, "phroonga extension version", PHROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (compiled)", PRN_GROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (linking)", grn_get_version());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/* }}} */
/* {{{ PHP_GINIT_FUNCTION */

static PHP_GINIT_FUNCTION(phroonga)
{
	memset(phroonga_globals, 0, sizeof(zend_phroonga_globals));

	phroonga_globals->default_encoding = GRN_ENC_DEFAULT;
	phroonga_globals->default_command_version = GRN_COMMAND_VERSION_DEFAULT;
	phroonga_globals->default_match_escalation_threshold = 0LL;
	phroonga_globals->default_logger_max_level = GRN_LOG_DEFAULT_LEVEL;

	prn_init_encodings_ht(&phroonga_globals->encodings_ht TSRMLS_CC);
	prn_init_command_versions_ht(&phroonga_globals->command_versions_ht TSRMLS_CC);
	prn_init_log_levels_ht(&phroonga_globals->log_levels_ht TSRMLS_CC);

#ifdef ZTS
	phroonga_globals->mutexp = tsrm_mutex_alloc();
#endif
}

/* }}} */
/* {{{ PHP_GSHUTDOWN_FUNCTION */

static PHP_GSHUTDOWN_FUNCTION(phroonga)
{
	zend_ts_hash_destroy(&phroonga_globals->encodings_ht);
	zend_ts_hash_destroy(&phroonga_globals->command_versions_ht);
	zend_ts_hash_destroy(&phroonga_globals->log_levels_ht);
#ifdef ZTS
	tsrm_mutex_free(phroonga_globals->mutexp);
#endif
}

/* }}} */
/* {{{ prn_register_constants() family */

static void prn_register_constants(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_ID_NIL);
	PRN_REGISTER_CONSTANT(GRN_ID_MAX);
	PRN_REGISTER_CONSTANT(GRN_TRUE);
	PRN_REGISTER_CONSTANT(GRN_FALSE);

	prn_register_enum_rc(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_encoding(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_command_version(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_log_level(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_content_type(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_builtin_type(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_builtin_tokenizer(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_proc_type(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_operator(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_info_type(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_enum_hook_entry(INIT_FUNC_ARGS_PASSTHRU);

	prn_register_ctx_init_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_table_cursor_open_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_table_sort_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_table_group_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_obj_set_value_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_snip_open_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_logger_info_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_obj_format_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_expr_flags(INIT_FUNC_ARGS_PASSTHRU);
	prn_register_ctx_cnnect_flags(INIT_FUNC_ARGS_PASSTHRU);
}

static void prn_register_enum_rc(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_SUCCESS);
	PRN_REGISTER_CONSTANT(GRN_END_OF_DATA);
	PRN_REGISTER_CONSTANT(GRN_UNKNOWN_ERROR);
	PRN_REGISTER_CONSTANT(GRN_OPERATION_NOT_PERMITTED);
	PRN_REGISTER_CONSTANT(GRN_NO_SUCH_FILE_OR_DIRECTORY);
	PRN_REGISTER_CONSTANT(GRN_NO_SUCH_PROCESS);
	PRN_REGISTER_CONSTANT(GRN_INTERRUPTED_FUNCTION_CALL);
	PRN_REGISTER_CONSTANT(GRN_INPUT_OUTPUT_ERROR);
	PRN_REGISTER_CONSTANT(GRN_NO_SUCH_DEVICE_OR_ADDRESS);
	PRN_REGISTER_CONSTANT(GRN_ARG_LIST_TOO_LONG);
	PRN_REGISTER_CONSTANT(GRN_EXEC_FORMAT_ERROR);
	PRN_REGISTER_CONSTANT(GRN_BAD_FILE_DESCRIPTOR);
	PRN_REGISTER_CONSTANT(GRN_NO_CHILD_PROCESSES);
	PRN_REGISTER_CONSTANT(GRN_RESOURCE_TEMPORARILY_UNAVAILABLE);
	PRN_REGISTER_CONSTANT(GRN_NOT_ENOUGH_SPACE);
	PRN_REGISTER_CONSTANT(GRN_PERMISSION_DENIED);
	PRN_REGISTER_CONSTANT(GRN_BAD_ADDRESS);
	PRN_REGISTER_CONSTANT(GRN_RESOURCE_BUSY);
	PRN_REGISTER_CONSTANT(GRN_FILE_EXISTS);
	PRN_REGISTER_CONSTANT(GRN_IMPROPER_LINK);
	PRN_REGISTER_CONSTANT(GRN_NO_SUCH_DEVICE);
	PRN_REGISTER_CONSTANT(GRN_NOT_A_DIRECTORY);
	PRN_REGISTER_CONSTANT(GRN_IS_A_DIRECTORY);
	PRN_REGISTER_CONSTANT(GRN_INVALID_ARGUMENT);
	PRN_REGISTER_CONSTANT(GRN_TOO_MANY_OPEN_FILES_IN_SYSTEM);
	PRN_REGISTER_CONSTANT(GRN_TOO_MANY_OPEN_FILES);
	PRN_REGISTER_CONSTANT(GRN_INAPPROPRIATE_I_O_CONTROL_OPERATION);
	PRN_REGISTER_CONSTANT(GRN_FILE_TOO_LARGE);
	PRN_REGISTER_CONSTANT(GRN_NO_SPACE_LEFT_ON_DEVICE);
	PRN_REGISTER_CONSTANT(GRN_INVALID_SEEK);
	PRN_REGISTER_CONSTANT(GRN_READ_ONLY_FILE_SYSTEM);
	PRN_REGISTER_CONSTANT(GRN_TOO_MANY_LINKS);
	PRN_REGISTER_CONSTANT(GRN_BROKEN_PIPE);
	PRN_REGISTER_CONSTANT(GRN_DOMAIN_ERROR);
	PRN_REGISTER_CONSTANT(GRN_RESULT_TOO_LARGE);
	PRN_REGISTER_CONSTANT(GRN_RESOURCE_DEADLOCK_AVOIDED);
	PRN_REGISTER_CONSTANT(GRN_NO_MEMORY_AVAILABLE);
	PRN_REGISTER_CONSTANT(GRN_FILENAME_TOO_LONG);
	PRN_REGISTER_CONSTANT(GRN_NO_LOCKS_AVAILABLE);
	PRN_REGISTER_CONSTANT(GRN_FUNCTION_NOT_IMPLEMENTED);
	PRN_REGISTER_CONSTANT(GRN_DIRECTORY_NOT_EMPTY);
	PRN_REGISTER_CONSTANT(GRN_ILLEGAL_BYTE_SEQUENCE);
	PRN_REGISTER_CONSTANT(GRN_SOCKET_NOT_INITIALIZED);
	PRN_REGISTER_CONSTANT(GRN_OPERATION_WOULD_BLOCK);
	PRN_REGISTER_CONSTANT(GRN_ADDRESS_IS_NOT_AVAILABLE);
	PRN_REGISTER_CONSTANT(GRN_NETWORK_IS_DOWN);
	PRN_REGISTER_CONSTANT(GRN_NO_BUFFER);
	PRN_REGISTER_CONSTANT(GRN_SOCKET_IS_ALREADY_CONNECTED);
	PRN_REGISTER_CONSTANT(GRN_SOCKET_IS_NOT_CONNECTED);
	PRN_REGISTER_CONSTANT(GRN_SOCKET_IS_ALREADY_SHUTDOWNED);
	PRN_REGISTER_CONSTANT(GRN_OPERATION_TIMEOUT);
	PRN_REGISTER_CONSTANT(GRN_CONNECTION_REFUSED);
	PRN_REGISTER_CONSTANT(GRN_RANGE_ERROR);
	PRN_REGISTER_CONSTANT(GRN_TOKENIZER_ERROR);
	PRN_REGISTER_CONSTANT(GRN_FILE_CORRUPT);
	PRN_REGISTER_CONSTANT(GRN_INVALID_FORMAT);
	PRN_REGISTER_CONSTANT(GRN_OBJECT_CORRUPT);
	PRN_REGISTER_CONSTANT(GRN_TOO_MANY_SYMBOLIC_LINKS);
	PRN_REGISTER_CONSTANT(GRN_NOT_SOCKET);
	PRN_REGISTER_CONSTANT(GRN_OPERATION_NOT_SUPPORTED);
	PRN_REGISTER_CONSTANT(GRN_ADDRESS_IS_IN_USE);
	PRN_REGISTER_CONSTANT(GRN_ZLIB_ERROR);
	PRN_REGISTER_CONSTANT(GRN_LZO_ERROR);
	PRN_REGISTER_CONSTANT(GRN_STACK_OVER_FLOW);
	PRN_REGISTER_CONSTANT(GRN_SYNTAX_ERROR);
	PRN_REGISTER_CONSTANT(GRN_RETRY_MAX);
	PRN_REGISTER_CONSTANT(GRN_INCOMPATIBLE_FILE_FORMAT);
	PRN_REGISTER_CONSTANT(GRN_UPDATE_NOT_ALLOWED);
	PRN_REGISTER_CONSTANT(GRN_TOO_SMALL_OFFSET);
	PRN_REGISTER_CONSTANT(GRN_TOO_LARGE_OFFSET);
	PRN_REGISTER_CONSTANT(GRN_TOO_SMALL_LIMIT);
	PRN_REGISTER_CONSTANT(GRN_CAS_ERROR);
	PRN_REGISTER_CONSTANT(GRN_UNSUPPORTED_COMMAND_VERSION);
}

static void prn_register_enum_encoding(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_ENC_DEFAULT);
	PRN_REGISTER_CONSTANT(GRN_ENC_NONE);
	PRN_REGISTER_CONSTANT(GRN_ENC_EUC_JP);
	PRN_REGISTER_CONSTANT(GRN_ENC_UTF8);
	PRN_REGISTER_CONSTANT(GRN_ENC_SJIS);
	PRN_REGISTER_CONSTANT(GRN_ENC_LATIN1);
	PRN_REGISTER_CONSTANT(GRN_ENC_KOI8R);
}

static void prn_register_enum_command_version(INIT_FUNC_ARGS)
{
	/* enum members */
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_DEFAULT);
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_1);
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_2);
	/* aliases */
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_MIN);
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_STABLE);
	PRN_REGISTER_CONSTANT(GRN_COMMAND_VERSION_MAX);
}

static void prn_register_enum_log_level(INIT_FUNC_ARGS)
{
	/* enum members */
	PRN_REGISTER_CONSTANT(GRN_LOG_NONE);
	PRN_REGISTER_CONSTANT(GRN_LOG_EMERG);
	PRN_REGISTER_CONSTANT(GRN_LOG_ALERT);
	PRN_REGISTER_CONSTANT(GRN_LOG_CRIT);
	PRN_REGISTER_CONSTANT(GRN_LOG_ERROR);
	PRN_REGISTER_CONSTANT(GRN_LOG_WARNING);
	PRN_REGISTER_CONSTANT(GRN_LOG_NOTICE);
	PRN_REGISTER_CONSTANT(GRN_LOG_INFO);
	PRN_REGISTER_CONSTANT(GRN_LOG_DEBUG);
	PRN_REGISTER_CONSTANT(GRN_LOG_DUMP);
	/* alias */
	PRN_REGISTER_CONSTANT(GRN_LOG_DEFAULT_LEVEL);
}

static void prn_register_enum_content_type(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_CONTENT_NONE);
	PRN_REGISTER_CONSTANT(GRN_CONTENT_TSV);
	PRN_REGISTER_CONSTANT(GRN_CONTENT_JSON);
	PRN_REGISTER_CONSTANT(GRN_CONTENT_XML);
	PRN_REGISTER_CONSTANT(GRN_CONTENT_MSGPACK);
}

static void prn_register_enum_builtin_type(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_DB_VOID);
	PRN_REGISTER_CONSTANT(GRN_DB_DB);
	PRN_REGISTER_CONSTANT(GRN_DB_OBJECT);
	PRN_REGISTER_CONSTANT(GRN_DB_BOOL);
	PRN_REGISTER_CONSTANT(GRN_DB_INT8);
	PRN_REGISTER_CONSTANT(GRN_DB_UINT8);
	PRN_REGISTER_CONSTANT(GRN_DB_INT16);
	PRN_REGISTER_CONSTANT(GRN_DB_UINT16);
	PRN_REGISTER_CONSTANT(GRN_DB_INT32);
	PRN_REGISTER_CONSTANT(GRN_DB_UINT32);
	PRN_REGISTER_CONSTANT(GRN_DB_INT64);
	PRN_REGISTER_CONSTANT(GRN_DB_UINT64);
	PRN_REGISTER_CONSTANT(GRN_DB_FLOAT);
	PRN_REGISTER_CONSTANT(GRN_DB_TIME);
	PRN_REGISTER_CONSTANT(GRN_DB_SHORT_TEXT);
	PRN_REGISTER_CONSTANT(GRN_DB_TEXT);
	PRN_REGISTER_CONSTANT(GRN_DB_LONG_TEXT);
	PRN_REGISTER_CONSTANT(GRN_DB_TOKYO_GEO_POINT);
	PRN_REGISTER_CONSTANT(GRN_DB_WGS84_GEO_POINT);
}

static void prn_register_enum_builtin_tokenizer(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_DB_MECAB);
	PRN_REGISTER_CONSTANT(GRN_DB_DELIMIT);
	PRN_REGISTER_CONSTANT(GRN_DB_UNIGRAM);
	PRN_REGISTER_CONSTANT(GRN_DB_BIGRAM);
	PRN_REGISTER_CONSTANT(GRN_DB_TRIGRAM);
}

static void prn_register_enum_proc_type(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_PROC_TOKENIZER);
	PRN_REGISTER_CONSTANT(GRN_PROC_COMMAND);
	PRN_REGISTER_CONSTANT(GRN_PROC_FUNCTION);
	PRN_REGISTER_CONSTANT(GRN_PROC_HOOK);
}

static void prn_register_enum_operator(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_OP_PUSH);
	PRN_REGISTER_CONSTANT(GRN_OP_POP);
	PRN_REGISTER_CONSTANT(GRN_OP_NOP);
	PRN_REGISTER_CONSTANT(GRN_OP_CALL);
	PRN_REGISTER_CONSTANT(GRN_OP_INTERN);
	PRN_REGISTER_CONSTANT(GRN_OP_GET_REF);
	PRN_REGISTER_CONSTANT(GRN_OP_GET_VALUE);
	PRN_REGISTER_CONSTANT(GRN_OP_AND);
	PRN_REGISTER_CONSTANT(GRN_OP_AND_NOT);
	PRN_REGISTER_CONSTANT(GRN_OP_OR);
	PRN_REGISTER_CONSTANT(GRN_OP_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_STAR_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_SLASH_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_MOD_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_PLUS_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_MINUS_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTL_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTR_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTRR_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_AND_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_XOR_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_OR_ASSIGN);
	PRN_REGISTER_CONSTANT(GRN_OP_JUMP);
	PRN_REGISTER_CONSTANT(GRN_OP_CJUMP);
	PRN_REGISTER_CONSTANT(GRN_OP_COMMA);
	PRN_REGISTER_CONSTANT(GRN_OP_BITWISE_OR);
	PRN_REGISTER_CONSTANT(GRN_OP_BITWISE_XOR);
	PRN_REGISTER_CONSTANT(GRN_OP_BITWISE_AND);
	PRN_REGISTER_CONSTANT(GRN_OP_BITWISE_NOT);
	PRN_REGISTER_CONSTANT(GRN_OP_EQUAL);
	PRN_REGISTER_CONSTANT(GRN_OP_NOT_EQUAL);
	PRN_REGISTER_CONSTANT(GRN_OP_LESS);
	PRN_REGISTER_CONSTANT(GRN_OP_GREATER);
	PRN_REGISTER_CONSTANT(GRN_OP_LESS_EQUAL);
	PRN_REGISTER_CONSTANT(GRN_OP_GREATER_EQUAL);
	PRN_REGISTER_CONSTANT(GRN_OP_IN);
	PRN_REGISTER_CONSTANT(GRN_OP_MATCH);
	PRN_REGISTER_CONSTANT(GRN_OP_NEAR);
	PRN_REGISTER_CONSTANT(GRN_OP_NEAR2);
	PRN_REGISTER_CONSTANT(GRN_OP_SIMILAR);
	PRN_REGISTER_CONSTANT(GRN_OP_TERM_EXTRACT);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTL);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTR);
	PRN_REGISTER_CONSTANT(GRN_OP_SHIFTRR);
	PRN_REGISTER_CONSTANT(GRN_OP_PLUS);
	PRN_REGISTER_CONSTANT(GRN_OP_MINUS);
	PRN_REGISTER_CONSTANT(GRN_OP_STAR);
	PRN_REGISTER_CONSTANT(GRN_OP_SLASH);
	PRN_REGISTER_CONSTANT(GRN_OP_MOD);
	PRN_REGISTER_CONSTANT(GRN_OP_DELETE);
	PRN_REGISTER_CONSTANT(GRN_OP_INCR);
	PRN_REGISTER_CONSTANT(GRN_OP_DECR);
	PRN_REGISTER_CONSTANT(GRN_OP_INCR_POST);
	PRN_REGISTER_CONSTANT(GRN_OP_DECR_POST);
	PRN_REGISTER_CONSTANT(GRN_OP_NOT);
	PRN_REGISTER_CONSTANT(GRN_OP_ADJUST);
	PRN_REGISTER_CONSTANT(GRN_OP_EXACT);
	PRN_REGISTER_CONSTANT(GRN_OP_LCP);
	PRN_REGISTER_CONSTANT(GRN_OP_PARTIAL);
	PRN_REGISTER_CONSTANT(GRN_OP_UNSPLIT);
	PRN_REGISTER_CONSTANT(GRN_OP_PREFIX);
	PRN_REGISTER_CONSTANT(GRN_OP_SUFFIX);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_DISTANCE1);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_DISTANCE2);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_DISTANCE3);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_DISTANCE4);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_WITHINP5);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_WITHINP6);
	PRN_REGISTER_CONSTANT(GRN_OP_GEO_WITHINP8);
	PRN_REGISTER_CONSTANT(GRN_OP_OBJ_SEARCH);
	PRN_REGISTER_CONSTANT(GRN_OP_EXPR_GET_VAR);
	PRN_REGISTER_CONSTANT(GRN_OP_TABLE_CREATE);
	PRN_REGISTER_CONSTANT(GRN_OP_TABLE_SELECT);
	PRN_REGISTER_CONSTANT(GRN_OP_TABLE_SORT);
	PRN_REGISTER_CONSTANT(GRN_OP_TABLE_GROUP);
	PRN_REGISTER_CONSTANT(GRN_OP_JSON_PUT);
	PRN_REGISTER_CONSTANT(GRN_OP_ALL_COLUMNS);
}

static void prn_register_enum_info_type(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_INFO_ENCODING);
	PRN_REGISTER_CONSTANT(GRN_INFO_SOURCE);
	PRN_REGISTER_CONSTANT(GRN_INFO_DEFAULT_TOKENIZER);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_CURR_MAX);
	PRN_REGISTER_CONSTANT(GRN_INFO_MAX_ELEMENT_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_SEG_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_CHUNK_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_MAX_SECTION);
	PRN_REGISTER_CONSTANT(GRN_INFO_HOOK_LOCAL_DATA);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_A);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_CHUNK);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_CHUNK_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_BUFFER_FREE);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_NTERMS);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_NTERMS_VOID);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_SIZE_IN_CHUNK);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_POS_IN_CHUNK);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_SIZE_IN_BUFFER);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_POS_IN_BUFFER);
	PRN_REGISTER_CONSTANT(GRN_INFO_ELEMENT_ESTIMATE_SIZE);
	PRN_REGISTER_CONSTANT(GRN_INFO_NGRAM_UNIT_SIZE);
	/*
	PRN_REGISTER_CONSTANT(GRN_INFO_VERSION);
	PRN_REGISTER_CONSTANT(GRN_INFO_CONFIGURE_OPTIONS);
	PRN_REGISTER_CONSTANT(GRN_INFO_CONFIG_PATH);
	*/
	PRN_REGISTER_CONSTANT(GRN_INFO_PARTIAL_MATCH_THRESHOLD);
	PRN_REGISTER_CONSTANT(GRN_INFO_II_SPLIT_THRESHOLD);
	PRN_REGISTER_CONSTANT(GRN_INFO_SUPPORT_ZLIB);
	PRN_REGISTER_CONSTANT(GRN_INFO_SUPPORT_LZO);
}

static void prn_register_enum_hook_entry(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_HOOK_SET);
	PRN_REGISTER_CONSTANT(GRN_HOOK_GET);
	PRN_REGISTER_CONSTANT(GRN_HOOK_INSERT);
	PRN_REGISTER_CONSTANT(GRN_HOOK_DELETE);
	PRN_REGISTER_CONSTANT(GRN_HOOK_SELECT);
}

static void prn_register_ctx_init_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_CTX_USE_QL);
	PRN_REGISTER_CONSTANT(GRN_CTX_BATCH_MODE);
	PRN_REGISTER_CONSTANT(GRN_CTX_PER_DB);
}

static void prn_register_table_cursor_open_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_CURSOR_ASCENDING);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_DESCENDING);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_GE);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_GT);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_LE);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_LT);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_BY_KEY);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_BY_ID);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_PREFIX);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_SIZE_BY_BIT);
	PRN_REGISTER_CONSTANT(GRN_CURSOR_RK);
}

static void prn_register_table_sort_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_TABLE_SORT_ASC);
	PRN_REGISTER_CONSTANT(GRN_TABLE_SORT_DESC);
}

static void prn_register_table_group_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_TABLE_GROUP_CALC_COUNT);
	PRN_REGISTER_CONSTANT(GRN_TABLE_GROUP_CALC_MAX);
	PRN_REGISTER_CONSTANT(GRN_TABLE_GROUP_CALC_MIN);
	PRN_REGISTER_CONSTANT(GRN_TABLE_GROUP_CALC_SUM);
	PRN_REGISTER_CONSTANT(GRN_TABLE_GROUP_CALC_AVG);
}

static void prn_register_obj_set_value_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_OBJ_SET_MASK);
	PRN_REGISTER_CONSTANT(GRN_OBJ_SET);
	PRN_REGISTER_CONSTANT(GRN_OBJ_INCR);
	PRN_REGISTER_CONSTANT(GRN_OBJ_DECR);
	PRN_REGISTER_CONSTANT(GRN_OBJ_APPEND);
	PRN_REGISTER_CONSTANT(GRN_OBJ_PREPEND);
	PRN_REGISTER_CONSTANT(GRN_OBJ_GET);
	PRN_REGISTER_CONSTANT(GRN_OBJ_COMPARE);
	PRN_REGISTER_CONSTANT(GRN_OBJ_LOCK);
	PRN_REGISTER_CONSTANT(GRN_OBJ_UNLOCK);
}

static void prn_register_snip_open_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_SNIP_NORMALIZE);
	PRN_REGISTER_CONSTANT(GRN_SNIP_COPY_TAG);
	PRN_REGISTER_CONSTANT(GRN_SNIP_SKIP_LEADING_SPACES);
}

static void prn_register_logger_info_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_LOG_TIME);
	PRN_REGISTER_CONSTANT(GRN_LOG_TITLE);
	PRN_REGISTER_CONSTANT(GRN_LOG_MESSAGE);
	PRN_REGISTER_CONSTANT(GRN_LOG_LOCATION);
}

static void prn_register_obj_format_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_OBJ_FORMAT_WITH_COLUMN_NAMES);
	PRN_REGISTER_CONSTANT(GRN_OBJ_FORMAT_ASARRAY);
}

static void prn_register_expr_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_EXPR_SYNTAX_QUERY);
	PRN_REGISTER_CONSTANT(GRN_EXPR_SYNTAX_SCRIPT);
	PRN_REGISTER_CONSTANT(GRN_EXPR_SYNTAX_OUTPUT_COLUMNS);
	PRN_REGISTER_CONSTANT(GRN_EXPR_ALLOW_PRAGMA);
	PRN_REGISTER_CONSTANT(GRN_EXPR_ALLOW_COLUMN);
	PRN_REGISTER_CONSTANT(GRN_EXPR_ALLOW_UPDATE);
	PRN_REGISTER_CONSTANT(GRN_EXPR_ALLOW_LEADING_NOT);
}

static void prn_register_ctx_cnnect_flags(INIT_FUNC_ARGS)
{
	PRN_REGISTER_CONSTANT(GRN_CTX_MORE);
	PRN_REGISTER_CONSTANT(GRN_CTX_TAIL);
	PRN_REGISTER_CONSTANT(GRN_CTX_HEAD);
	PRN_REGISTER_CONSTANT(GRN_CTX_QUIET);
	PRN_REGISTER_CONSTANT(GRN_CTX_QUIT);
}

/* }}} */
/* {{{ ini handlers */


static PHP_INI_MH(prn_update_default_encoding)
{
	TsHashTable *ht = &PRNG(encodings_ht);
	grn_encoding encoding = GRN_ENC_DEFAULT;
	grn_rc rc;

	if (new_value && new_value[0]) {
		int value = 0;
		if (prn_ini_ts_hash_find(ht, new_value, new_value_length, &value)) {
			encoding = (grn_encoding)value;
		} else {
			return FAILURE;
		}
	} else {
		return FAILURE;
	}

	PRN_MUTEX_LOCK();
	rc = grn_set_default_encoding(encoding);
	PRN_MUTEX_UNLOCK();

	if (rc != GRN_SUCCESS) {
		return FAILURE;
	}

	return OnUpdateStringUnempty(PRN_INI_MH_ARGS_PASSTHRU);
}

static PHP_INI_MH(prn_update_default_command_version)
{
	TsHashTable *ht = &PRNG(command_versions_ht);
	grn_command_version version = GRN_COMMAND_VERSION_DEFAULT;
	grn_rc rc;

	if (new_value && new_value[0]) {
		int value = 0;
		if (prn_ini_ts_hash_find(ht, new_value, new_value_length, &value)) {
			version = (grn_command_version)value;
		} else {
			return FAILURE;
		}
	} else {
		return FAILURE;
	}

	PRN_MUTEX_LOCK();
	rc = grn_set_default_command_version(version);
	PRN_MUTEX_UNLOCK();

	if (rc != GRN_SUCCESS) {
		return FAILURE;
	}

	return OnUpdateStringUnempty(PRN_INI_MH_ARGS_PASSTHRU);
}

static PHP_INI_MH(prn_update_default_match_escalation_threshold)
{
	long threshold;
	grn_rc rc;

	threshold = zend_atol(new_value, new_value_length);
	if (threshold < 0L) {
		return FAILURE;
	}

	PRN_MUTEX_LOCK();
	rc = grn_set_default_match_escalation_threshold((long long int)threshold);
	PRN_MUTEX_UNLOCK();

	if (rc != GRN_SUCCESS) {
		return FAILURE;
	}

	return OnUpdateLongGEZero(PRN_INI_MH_ARGS_PASSTHRU);
}

static PHP_INI_MH(prn_update_default_logger_set_max_level)
{
	TsHashTable *ht = &PRNG(log_levels_ht);
	grn_log_level level = GRN_LOG_DEFAULT_LEVEL;

	if (new_value && new_value[0]) {
		int value = 0;
		if (prn_ini_ts_hash_find(ht, new_value, new_value_length, &value)) {
			level = (grn_log_level)value;
		} else {
			return FAILURE;
		}
	} else {
		return FAILURE;
	}

	PRN_MUTEX_LOCK();
	grn_default_logger_set_max_level(level);
	PRN_MUTEX_UNLOCK();

	return OnUpdateStringUnempty(PRN_INI_MH_ARGS_PASSTHRU);
}

/* }}} */
/* {{{ global hash table initializers */

static void prn_init_encodings_ht(TsHashTable *ht TSRMLS_DC)
{
	zend_ts_hash_init(ht, 32, NULL, NULL, 1);

	PRN_INI_HASH_ADD(ht, GRN_ENC_DEFAULT);
	PRN_INI_HASH_ADD(ht, GRN_ENC_NONE);
	PRN_INI_HASH_ADD(ht, GRN_ENC_EUC_JP);
	PRN_INI_HASH_ADD(ht, GRN_ENC_UTF8);
	PRN_INI_HASH_ADD(ht, GRN_ENC_SJIS);
	PRN_INI_HASH_ADD(ht, GRN_ENC_LATIN1);
	PRN_INI_HASH_ADD(ht, GRN_ENC_KOI8R);

	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_DEFAULT, "default");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_NONE, "none");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_EUC_JP, "euc-jp");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_EUC_JP, "eucjp");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_UTF8, "utf8");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_UTF8, "utf-8");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_SJIS, "sjis");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_SJIS, "shift_jis");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_LATIN1, "latin1");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_LATIN1, "latin-1");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_KOI8R, "koi8r");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_ENC_KOI8R, "koi8-r");
}

static void prn_init_command_versions_ht(TsHashTable *ht TSRMLS_DC)
{
	zend_ts_hash_init(ht, 32, NULL, NULL, 1);

	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_DEFAULT);
	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_1);
	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_2);
	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_MIN);
	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_STABLE);
	PRN_INI_HASH_ADD(ht, GRN_COMMAND_VERSION_MAX);

	PRN_INI_HASH_ADD_ALIAS(ht, GRN_COMMAND_VERSION_DEFAULT, "default");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_COMMAND_VERSION_MIN, "min");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_COMMAND_VERSION_STABLE, "stable");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_COMMAND_VERSION_MAX, "max");
}

static void prn_init_log_levels_ht(TsHashTable *ht TSRMLS_DC)
{
	zend_ts_hash_init(ht, 64, NULL, NULL, 1);

	PRN_INI_HASH_ADD(ht, GRN_LOG_NONE);
	PRN_INI_HASH_ADD(ht, GRN_LOG_EMERG);
	PRN_INI_HASH_ADD(ht, GRN_LOG_ALERT);
	PRN_INI_HASH_ADD(ht, GRN_LOG_CRIT);
	PRN_INI_HASH_ADD(ht, GRN_LOG_ERROR);
	PRN_INI_HASH_ADD(ht, GRN_LOG_WARNING);
	PRN_INI_HASH_ADD(ht, GRN_LOG_NOTICE);
	PRN_INI_HASH_ADD(ht, GRN_LOG_INFO);
	PRN_INI_HASH_ADD(ht, GRN_LOG_DEBUG);
	PRN_INI_HASH_ADD(ht, GRN_LOG_DUMP);
	PRN_INI_HASH_ADD(ht, GRN_LOG_DEFAULT_LEVEL);

	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_NONE, "none");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_EMERG, "emerg");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_ALERT, "alert");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_CRIT, "crit");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_ERROR, "error");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_WARNING, "warning");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_NOTICE, "notice");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_INFO, "info");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_DEBUG, "debug");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_DUMP, "dump");
	PRN_INI_HASH_ADD_ALIAS(ht, GRN_LOG_DEFAULT_LEVEL, "default");
}

/* {{{ hash utilities */
/* }}} */

static void prn_ini_non_ts_hash_add(HashTable *ht, const char *key, int value)
{
	char buf[PRN_INI_HASH_KEY_MAX_SIZE];
	const char *arKey;
	uint length;

	zend_hash_index_update(ht, (ulong)value, &value, sizeof(int), NULL);

	length = (uint)strlen(key);
	if (length >= PRN_INI_HASH_KEY_MAX_SIZE) {
		length = PRN_INI_HASH_KEY_MAX_SIZE - 1;
	}
	arKey = zend_str_tolower_copy(buf, key, length);
	zend_hash_update(ht, arKey, length + 1, (void *)&value, sizeof(int), NULL);
}

static zend_bool prn_ini_ts_hash_find(TsHashTable *ht, const char *key, uint length, int *pValue)
{
	char *arKey = zend_str_tolower_dup(key, length);
	zend_bool found = 0;
	int *pData = NULL;

	if (zend_ts_hash_find(ht, arKey, length + 1, (void **)&pData) == SUCCESS) {
		found = 1;
	}

	efree(arKey);

	if (found && pValue) {
		*pValue = *pData;
	}

	return found;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
