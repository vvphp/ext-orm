PHP_ARG_ENABLE(swoole_orm, whether to enable swoole_orm support, [ --enable-swoole_orm Enable Cat Client support])

swoole_orm_source_file="swoole_orm.c \
				   src/memory.c \
				   src/operator.c \
				   src/string.c \
				   src/array.c"

if test "$PHP_SWOOLE_ORM" = "yes"; then
    AC_DEFINE(HAVE_SWOOLE_ORM, 1, [Whether you have Swoole_orm])
    PHP_NEW_EXTENSION(swoole_orm, $swoole_orm_source_file, $ext_shared)
fi
