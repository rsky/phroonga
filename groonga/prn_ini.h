/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012-2013 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "private.h"

#ifndef PHROONGA_INI_H
#define PHROONGA_INI_H

BEGIN_EXTERN_C()

/* {{{ ini modification handler prototypes */

PRN_LOCAL PHP_INI_MH(prn_update_default_encoding);
PRN_LOCAL PHP_INI_MH(prn_update_default_command_version);
PRN_LOCAL PHP_INI_MH(prn_update_default_match_escalation_threshold);
PRN_LOCAL PHP_INI_MH(prn_update_default_logger_max_level);

/* }}} */
/* {{{ internal function prototypes */

PRN_LOCAL void prn_init_ini_param_tables(void);
PRN_LOCAL void prn_destroy_ini_param_tables(void);

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
