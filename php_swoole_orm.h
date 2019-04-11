/*
  +----------------------------------------------------------------------+
  |                            swoole_orm                                |
  +----------------------------------------------------------------------+
  | swoole_orm is a database ORM ext for mysql written in c language     |
  +----------------------------------------------------------------------+
  | LICENSE: https://github.com/caohao0730/swoole_orm/blob/master/LICENSE|
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_SWOOLE_ORM_H
#define PHP_SWOOLE_ORM_H 1

#include "php.h"
#include "php_ini.h"  //获取 php.ini 操作的方法

#include "zend_interfaces.h"
#include "zend_variables.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "swoole_orm.h"
BEGIN_EXTERN_C()

#define PHP_SWOOLE_ORM_VERSION  "1.0.0"

#ifndef ZEND_MOD_END
#define ZEND_MOD_END {NULL,NULL,NULL}
#endif

extern zend_module_entry swoole_orm_module_entry;  //扩展模块实体的定义，(扩展必须)
#define phpext_swoole_orm_ptr &swoole_orm_module_entry  //定义php扩展swoole_orm指针，(扩展必须)

#ifdef PHP_WIN32
#	define PHP_SWOOLE_ORM_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SWOOLE_ORM_API __attribute__ ((visibility("default")))
#else
#	define PHP_SWOOLE_ORM_API
#endif

//函数参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_void, 0, 0, 0)
ZEND_END_ARG_INFO()

//初始化函数
PHP_MINIT_FUNCTION(swoole_orm);
PHP_MSHUTDOWN_FUNCTION(swoole_orm);
PHP_RINIT_FUNCTION(swoole_orm);
PHP_RSHUTDOWN_FUNCTION(swoole_orm);
PHP_MINFO_FUNCTION(swoole_orm);

//普通函数
PHP_FUNCTION(swoole_orm_version);

//全局变量
ZEND_BEGIN_MODULE_GLOBALS(swoole_orm)
ZEND_END_MODULE_GLOBALS(swoole_orm)

extern ZEND_DECLARE_MODULE_GLOBALS(swoole_orm);

#ifdef ZTS
#define SWOOLE_ORM_G(v) TSRMG(swoole_orm_globals_id, zend_swoole_orm_globals *, v)
#else
#define SWOOLE_ORM_G(v) (swoole_orm_globals.v)
#endif

END_EXTERN_C()

#endif
