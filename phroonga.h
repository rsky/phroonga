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
#include <php_ini.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>
#include <groonga.h>

BEGIN_EXTERN_C()

#define PHROONGA_VERSION "0.0.1"

END_EXTERN_C()

#endif
