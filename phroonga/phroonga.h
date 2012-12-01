/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#ifndef PHROONGA_H
#define PHROONGA_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
 
#include <php.h>
#include <groonga.h>

#define PHROONGA_VERSION "0.0.1"

#if defined(__GNUC__) && __GNUC__ >= 4
#define PRN_LOCAL __attribute__((visibility("hidden")))
#else
#define PRN_LOCAL
#endif
#define PRN_FUNCTION(name) PRN_LOCAL PHP_FUNCTION(name)

BEGIN_EXTERN_C()

ZEND_BEGIN_MODULE_GLOBALS(phroonga)
	grn_encoding        default_encoding;
	grn_command_version default_command_version;
	long long int       default_match_escalation_threshold;
	grn_log_level       default_logger_max_level;
	TsHashTable         encodings_ht;
	TsHashTable         command_versions_ht;
	TsHashTable         log_levels_ht;
#ifdef ZTS
	MUTEX_T mutexp;
#endif
ZEND_END_MODULE_GLOBALS(phroonga)

ZEND_EXTERN_MODULE_GLOBALS(phroonga)

#ifdef ZTS
#define PRNG(v) TSRMG(phroonga_globals_id, zend_phroonga_globals *, v)
#define PRN_MUTEX_LOCK() tsrm_mutex_lock(PRNG(mutexp))
#define PRN_MUTEX_UNLOCK() tsrm_mutex_unlock(PRNG(mutexp))
#else
#define PRNG(v) (phroonga_globals.v)
#define PRN_MUTEX_LOCK()
#define PRN_MUTEX_UNLOCK()
#endif

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
