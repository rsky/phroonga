/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"

#ifndef PHROONGA_PRIVATE_H
#define PHROONGA_PRIVATE_H

BEGIN_EXTERN_C()

/* {{{ visibility */

#ifdef __clang__
#define PRN_LOCAL __attribute__((visibility("hidden")))
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PRN_LOCAL __attribute__((visibility("hidden")))
#else
#define PRN_LOCAL
#endif
#define PRN_FUNCTION(name) PRN_LOCAL PHP_FUNCTION(name)

/* }}} */
/* {{{ mutex */

#ifdef ZTS
extern PRN_LOCAL MUTEX_T phroonga_mutex;
#define PRN_MUTEX_LOCK() tsrm_mutex_lock(phroonga_mutex)
#define PRN_MUTEX_UNLOCK() tsrm_mutex_unlock(phroonga_mutex)
#else
#define PRN_MUTEX_LOCK()
#define PRN_MUTEX_UNLOCK()
#endif

/* }}} */
/* {{{ miscellaneous private functions */

PRN_LOCAL int prn_is_valid_encoding(long encoding);
PRN_LOCAL int prn_is_valid_command_version(long version);
PRN_LOCAL int prn_is_valid_log_level(long level);

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
