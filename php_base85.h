#ifndef PHP_BASE85_H
#define PHP_BASE85_H 1
#define PHP_BASE85_VERSION "1.0"
#define PHP_BASE85_EXTNAME "base85"

PHP_FUNCTION(base85_encode);
PHP_FUNCTION(base85_decode);

extern zend_module_entry base85_module_entry;
#define phpext_base85_ptr &base85_module_entry

#endif