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

/* {{{ macros */

#define PRN_INI_HASH_KEY_BUF_SIZE 32
#define PRN_INI_HASH_KEY_MAX_LENGTH (PRN_INI_HASH_KEY_BUF_SIZE - 1)

#define PRN_INI_HASH_ADD(ht, value) \
	prn_ini_non_ts_hash_add(TS_HASH(ht), #value, value, 1)

#define PRN_INI_HASH_ADD_ALIAS(ht, value, alias) \
	prn_ini_non_ts_hash_add(TS_HASH(ht), alias, value, 0)

/* }}} */
/* {{{ function prototypes */

static void prn_ini_non_ts_hash_add(HashTable *ht, const char *key, int value, int index_add);
static zend_bool prn_ini_ts_hash_find(TsHashTable *ht, const char *key, uint length, int *pValue);

/* }}} */
/* {{{ prn_update_default_encoding() */

PRN_LOCAL PHP_INI_MH(prn_update_default_encoding)
{
	TsHashTable *ht = PRNG(encodings_ht);
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

	return SUCCESS;
}

/* }}} */
/* {{{ prn_update_default_command_version() */

PRN_LOCAL PHP_INI_MH(prn_update_default_command_version)
{
	TsHashTable *ht = PRNG(command_versions_ht);
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

	return SUCCESS;
}

/* }}} */
/* {{{ prn_update_default_match_escalation_threshold() */

PRN_LOCAL PHP_INI_MH(prn_update_default_match_escalation_threshold)
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

	return SUCCESS;
}

/* }}} */
/* {{{ prn_update_default_logger_max_level() */

PRN_LOCAL PHP_INI_MH(prn_update_default_logger_max_level)
{
	TsHashTable *ht = PRNG(log_levels_ht);
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

	return SUCCESS;
}

/* }}} */
/* {{{ prn_init_encodings_ht() */

PRN_LOCAL void prn_init_encodings_ht(TsHashTable *ht)
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
	/*
	fprintf(stderr, "%s: %u/%u\n", __FUNCTION__,
		ht->hash.nNumOfElements, ht->hash.nTableSize);
	*/
}

/* }}} */
/* {{{ prn_init_command_versions_ht() */

PRN_LOCAL void prn_init_command_versions_ht(TsHashTable *ht)
{
	zend_ts_hash_init(ht, 16, NULL, NULL, 1);

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
	/*
	fprintf(stderr, "%s: %u/%u\n", __FUNCTION__,
		ht->hash.nNumOfElements, ht->hash.nTableSize);
	*/
}

/* }}} */
/* {{{ prn_init_log_levels_ht() */

PRN_LOCAL void prn_init_log_levels_ht(TsHashTable *ht)
{
	zend_ts_hash_init(ht, 32, NULL, NULL, 1);

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
	/*
	fprintf(stderr, "%s: %u/%u\n", __FUNCTION__,
		ht->hash.nNumOfElements, ht->hash.nTableSize);
	*/
}

/* }}} */
/* {{{ prn_ini_non_ts_hash_add() */

static void prn_ini_non_ts_hash_add(HashTable *ht, const char *key, int value, int index_add)
{
	char buf[PRN_INI_HASH_KEY_BUF_SIZE];
	const char *arKey;
	uint nKeyLength = (uint)strlen(key);
	void *pData;

	/* check for developing */
	if (nKeyLength > PRN_INI_HASH_KEY_MAX_LENGTH) {
		zend_error(E_ERROR, "too long key: %s", key);
		return;
	}

	arKey = zend_str_tolower_copy(buf, key, nKeyLength);
	pData = (void *)&value;

	if (index_add) {
		zend_hash_index_update(ht, (ulong)value, pData, sizeof(int), NULL);
	}
	zend_hash_update(ht, arKey, nKeyLength, pData, sizeof(int), NULL);
}

/* }}} */
/* {{{ prn_ini_ts_hash_find() */

static zend_bool prn_ini_ts_hash_find(TsHashTable *ht, const char *key, uint length, int *pValue)
{
	char *arKey;
	int *pData = NULL;
	zend_bool found = 0;

	if (length > PRN_INI_HASH_KEY_MAX_LENGTH) {
		return 0;
	}

	arKey = zend_str_tolower_dup(key, length);

	if (zend_ts_hash_find(ht, arKey, length, (void **)&pData) == SUCCESS) {
		found = 1;
		if (pValue) {
			*pValue = *pData;
		}
	}

	efree(arKey);

	return found;
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
