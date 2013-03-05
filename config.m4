PHP_ARG_ENABLE(base85, whether to enable
base85 support,
[ --enable-base85   Enable base85 support])
if test "$PHP_BASE85" = "yes"; then
  AC_DEFINE(HAVE_BASE85, 1, [Whether you have base85])
  PHP_NEW_EXTENSION(base85, php_base85.c, $ext_shared)
fi
