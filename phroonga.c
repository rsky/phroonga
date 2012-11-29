/**
 * phroonga: the PHP groonga extension
 *
 * @package     phroonga
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "phroonga.h"

static PHP_MINIT_FUNCTION(phroonga);
static PHP_MSHUTDOWN_FUNCTION(phroonga);
static PHP_RINIT_FUNCTION(phroonga);
static PHP_RSHUTDOWN_FUNCTION(phroonga);
static PHP_MINFO_FUNCTION(phroonga);

static zend_function_entry phroonga_functions[] = {
    { NULL, NULL, NULL }
};

zend_module_entry phroonga_module_entry = {
	STANDARD_MODULE_HEADER,
	"phroonga",
	phroonga_functions,
	PHP_MINIT(phroonga),
	PHP_MSHUTDOWN(phroonga),
	PHP_RINIT(phroonga),
	PHP_RSHUTDOWN(phroonga),
	PHP_MINFO(phroonga),
	PHROONGA_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHROONGA
ZEND_GET_MODULE(phroonga)
#endif

static PHP_MINIT_FUNCTION(phroonga)
{
	if (grn_init() != GRN_SUCCESS) {
		return FAILURE;
	}
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(phroonga)
{
	if (grn_fin() != GRN_SUCCESS) {
		return FAILURE;
	}
	return SUCCESS;
}

static PHP_RINIT_FUNCTION(phroonga)
{
	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(phroonga)
{
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(phroonga)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "phroonga support", "enabled");
	php_info_print_table_row(2, "phroonga extension version", PHROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (compiled)", PRN_GROONGA_VERSION);
	php_info_print_table_row(2, "groonga version (linking)", grn_get_version());
	php_info_print_table_end();
}
