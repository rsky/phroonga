/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "log.h"

/* {{{ grn_default_logger_get_max_level() */

PRN_FUNCTION(grn_default_logger_get_max_level)
{
	grn_log_level level;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PRN_MUTEX_LOCK();
	level = grn_default_logger_get_max_level();
	PRN_MUTEX_UNLOCK();

	RETURN_LONG((long)level);
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
