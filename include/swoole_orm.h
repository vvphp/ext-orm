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
#ifndef SWOOLE_ORM_H_
#define SWOOLE_ORM_H_

#if defined(HAVE_CONFIG_H) && !defined(COMPILE_DL_SWOOLE_ORM)
#include "config.h"
#endif

#ifdef SW_ORM_STATIC_COMPILATION
#include "php_config.h"
#endif

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/un.h>

#include "php7_wrapper.h"
#include "swoole_orm_config.h"
#include "error.h"
#include <ext/standard/php_var.h>
#include <ext/standard/php_string.h>

#ifdef _MSC_VER
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#else
#include <inttypes.h>   /* uint32_t */
#endif

/////////////////////// 类声明 ///////////////////////
#define MAX_TABLE_SIZE 48
#define MAX_OPERATOR_SIZE 4

zend_class_entry swoole_orm_ce;
zend_class_entry* swoole_orm_ce_ptr;

PHP_METHOD(swoole_orm, __construct);
PHP_METHOD(swoole_orm, __destruct);
PHP_METHOD(swoole_orm, select);
PHP_METHOD(swoole_orm, insert);
PHP_METHOD(swoole_orm, replace);
PHP_METHOD(swoole_orm, update);
PHP_METHOD(swoole_orm, delete);

//私有函数
char* select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql);
char* handle_join(zval* join, char* table, char** table_query);
char* column_quote(char* string, char* table_column);
char* get_join_type(char* type);
char* column_push(zval* columns, zval* map, char** column_query);
char* where_clause(zval* where, zval* map, char** where_query);
char* where_implode(char* key, zval* data, zval* map, char** where_clause, char* connector);
char* group_by_implode(zval* group, char** group_by_condition);
char* having_implode(zval* having, zval* map, char** having_condition);
char* order_implode(zval* order, char** order_condition);
char* limit_implode(zval* limit, char** limit_condition);
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map);
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector);
zval* add_map(zval* map, zval* value);
void update_error_info(zval* obj, char* code, char* errmsg);
void update_pdo_error(zval* obj, zval* errorcode, zval* errorinfo);
int is_write_type(char* sql);
int is_insert(char* sql);

//preg函数
int preg_join_match(char* key, char* join, char* table, char* alias); //匹配join表
int preg_operator_match(char* key, char* column, char* operator); //匹配列名和操作符
int preg_and_or_match(char* key, char* relation); //匹配是否 AND 或者 OR
int preg_table_match(char* key, char* table, char* alias);  //匹配表名和别名

//object函数
zend_class_entry* get_pdo_ce();

/////////////////////////////// 公共宏定义 //////////////////////////////////
#define SW_ORM_OK                  0
#define SW_ORM_ERR                -1
#define SW_ORM_AGAIN              -2
#define SW_ORM_BUSY               -3
#define SW_ORM_DONE               -4
#define SW_ORM_DECLINED           -5
#define SW_ORM_ABORT              -6

#define MAP_ITOA_INT_SIZE 16

#ifndef ulong
#define ulong unsigned long
typedef unsigned long ulong_t;
#endif

//内存分配大小
#define ALIGNED_SIZE_32 32
#define MM_ALIGNED_SIZE_16(size) ((size + Z_L(16) - Z_L(1)) & (~(Z_L(16) - Z_L(1)))) //16位内存对齐
#define MM_ALIGNED_SIZE_32(size) ((size + Z_L(32) - Z_L(1)) & (~(Z_L(32) - Z_L(1)))) //32位内存对齐
#define MM_REAL_SIZE(len) (MM_ALIGNED_SIZE_32(len + 1 + sizeof(size_t)))

/////////////////////// 基础函数 ///////////////////////
#define sw_orm_php_fatal_error(level, fmt_str, ...)   php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
static sw_orm_inline void zval_debug(char* name, zval* p) {
    php_printf("----------%s----------\n", name);
    php_debug_zval_dump(p, 1);
}
static sw_orm_inline void string_debug(char* name, char* str) {
    php_printf("%s：[%s] (string)\n", name, str);
}
static sw_orm_inline void int_debug(char* name, int l) {
    php_printf("%s：[%d] (int)\n", name, l);
}
static sw_orm_inline void float_debug(char* name, float l) {
    php_printf("%s：[%f] (float)\n", name, l);
}
static sw_orm_inline void x_debug(char* name, void* l) {
    php_printf("%s：[%x] (point)\n", name, l);
}
static sw_orm_inline void info_debug(char* name) {
    php_printf("============%s============\n", name);
}
static sw_orm_inline void term_debug(char* term_info) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, term_info);
}

//////////////////// memory.c ///////////////////////////
static sw_orm_inline void sw_orm_emalloc_and_init(void **tmp, size_t size) {
    *tmp = emalloc(size);    //分配空间，并初始化为 0
    memset(*tmp, 0, size);
}

static sw_orm_inline zval* sw_orm_zval_copy(zval * source) {
	zval *copy;
	SW_ORM_MAKE_STD_ZVAL(copy);
	*copy = *source;
	zval_copy_ctor(copy);
	return copy;
}

extern char* sw_orm_multi_memcpy(char * source, int n_str, ...);

//以32位对齐分配内存
static sw_orm_inline void sw_orm_string_emalloc_32(char **tmp, int len) {
    size_t real_size = MM_REAL_SIZE(len);
    *tmp = emalloc(real_size);
    memset(*tmp, 0, real_size);
    memcpy(*tmp, &real_size, sizeof(size_t));
    *tmp = *tmp + sizeof(size_t);
}

static sw_orm_inline void sw_orm_string_efree_32(char *tmp) {
    efree(tmp - sizeof(size_t));
}

extern char* sw_orm_multi_memcpy_auto_realloc(char** source, int n_str, ...); //自动扩容

//////////////////// string.c ///////////////////////////
#define sw_orm_is_space(p) (p != '\0' && (isspace(p) || (p) == '\n' || (p) == '\r' || (p) == '\t'))
#define sw_orm_is_string_not_empty(p) (p != NULL && p[0] != '\0')
#define sw_orm_is_string_empty(p) !sw_orm_is_string_not_empty(p)
#define sw_orm_init_stack_string(p, str)  SW_ORM_MAKE_STD_ZVAL(p); SW_ORM_ZVAL_STRING(p, str, 1);

#if defined(SW_ORM_USE_JEMALLOC) || defined(SW_ORM_USE_TCMALLOC)
extern sw_orm_inline char* sw_orm_strdup(const char *s);
extern sw_orm_inline char* sw_orm_strndup(const char *s, size_t n);
#else
#define sw_orm_strdup              strdup
#define sw_orm_strndup             strndup
#endif

extern zval* sw_orm_preg_match(const char* regex_str, char* subject_str);
extern int sw_orm_strpos(const char *haystack,const char *needle);
extern char* rtrim(char *str);
extern char* ltrim(char *str);
extern char* trim(char *str);
extern char* ltrim_str(char *str, char *remove);
extern char* rtrim_str(char *str, char *remove);
extern char* sw_orm_itoa(long num,char* str);
extern char* sw_orm_escape(char* sql);
extern char* strreplace(char* original, char const * const pattern, char const * const replacement);
extern void sw_orm_serialize(zval *return_value, zval *var TSRMLS_DC);
extern void sw_orm_unserialize(zval *return_value, zval *var TSRMLS_DC);

/////////////////// array.c ////////////////////////////
extern char* sw_orm_get_string_from_array_index(zval *array, ulong index);
extern HashTable* sw_orm_get_array_keys(zval* p);
extern char* sw_orm_get_array_key_index(zval* p, uint32_t index);
extern zval * sw_orm_get_element_by_hashtable_index(HashTable *ht, int index);
extern zval * php_sw_orm_array_get_value(HashTable *ht, char *key);
static sw_orm_inline void sw_orm_free_hash(HashTable *ht) {
    zend_hash_destroy(ht);    //释放hash
    FREE_HASHTABLE(ht);
}
extern void sw_orm_destroy_hash(HashTable *ht);
extern void sw_orm_destroy_array(zval **array);
extern void sw_orm_clean_hash(HashTable *ht);
extern void sw_orm_clean_array(zval **array);
extern int is_set_array_index(HashTable *ht, int index); //数组下标 index 是否为空
extern char* sw_orm_get_string_from_hashtable_index(HashTable *ht, int index);
extern char* sw_orm_get_string_from_hashtable(HashTable *ht, char* key);
extern char* sw_orm_implode(zval *arr, const char *delim_str, char** result);
extern void sw_orm_array_single_columns(zval** return_single_column_result, zval* data);

/////////////////// operator.c ////////////////////////////
extern int sw_orm_compare_strict_bool(zval *op1, zend_bool op2 TSRMLS_DC);

/////////////////// net.c ////////////////////////////
php_stream* unix_socket_conn(char *servername);

//////////////////// 公共宏定义 /////////////////////
#define SW_ORM_IS_FALSE(var)        sw_orm_compare_strict_bool(var, 0 TSRMLS_CC)
#define SW_ORM_IS_TRUE(var)         sw_orm_compare_strict_bool(var, 1 TSRMLS_CC)
#define SW_ORM_IS_ARRAY(var)   (var != NULL && Z_TYPE_P(var) == IS_ARRAY)
#define SW_ORM_IS_NOT_ARRAY(var)   (!SW_ORM_IS_ARRAY(var))
#define SW_ORM_IS_EMPTY(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL || SW_ORM_IS_FALSE(var) || (Z_TYPE_P(var) == IS_STRING && !Z_STRLEN_P(var)) || !zend_is_true(var))
#define SW_ORM_IS_NOT_EMPTY(var)   (!SW_ORM_IS_EMPTY(var))
#define SW_ORM_IS_NULL(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL)
#define SW_ORM_IS_NOT_NULL(var)   (!SW_ORM_IS_NULL(var))
#define SW_ORM_HASHTABLE_IS_NOT_EMPTY(var)		(var != NULL && zend_hash_num_elements(var))
#define SW_ORM_HASHTABLE_IS_EMPTY(var)   (!SW_ORM_HASHTABLE_IS_NOT_EMPTY(var))

/////////////////// object.c ////////////////////////////
static inline zval* sw_orm_read_init_property(zend_class_entry *scope, zval *object, char *p, size_t pl TSRMLS_DC) {
    zval* property = sw_orm_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    if (property == NULL) {
        zend_update_property_null(scope, object, p, pl);
        return sw_orm_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    } else {
        return property;
    }
}

static inline int sw_orm_call_user_function_ex_fast(zval** object_pp, char* function_name, zval **retval_ptr_ptr, uint32_t param_count, zval ***params) {
    zval* _function_name;
    
    SW_ORM_MAKE_STD_ZVAL(_function_name);
    SW_ORM_ZVAL_STRING(_function_name, function_name, 1);
    
    if(*retval_ptr_ptr == NULL) SW_ORM_ALLOC_INIT_ZVAL(*retval_ptr_ptr);
    
    int ret = sw_orm_call_user_function_ex(EG(function_table), object_pp, _function_name, retval_ptr_ptr, param_count, params, 0, NULL TSRMLS_CC);
    
    sw_orm_zval_ptr_dtor(&_function_name);
    
    return ret;
}

static sw_orm_inline int sw_orm_call_user_function_return_bool_or_unsigned(zval** object_pp, char* function_name, uint32_t param_count, zval ***params) {
	int result = 0;
    zval tmp_point;
    zval* retval = &tmp_point, *_function_name;
	
    SW_ORM_MAKE_STD_ZVAL(_function_name);
    SW_ORM_ZVAL_STRING(_function_name, function_name, 1);

    int ret = sw_orm_call_user_function_ex(EG(function_table), object_pp, _function_name, &retval, param_count, params, 0, NULL TSRMLS_CC);
    sw_orm_zval_ptr_dtor(&_function_name);
    if (ret == FAILURE) {
    	sw_orm_zval_ptr_dtor(&retval);
    	return FAILURE;
    }

    //返回 true
    if (Z_TYPE_P(retval) == IS_TRUE)
    {
    	result = 1;
    } else if (Z_TYPE_P(retval) == IS_LONG) { //返回无符号整数
        result = Z_LVAL_P(retval);
    }
    
	sw_orm_zval_ptr_dtor(&retval);
	return result;
}

static sw_orm_inline int sw_orm_call_user_function_construct_fast(zval** object_pp, uint32_t param_count, zval ***params) {
    zval tmp_point;
    zval *construct_ret = &tmp_point;
    
    zval* function_name;
    SW_ORM_MAKE_STD_ZVAL(function_name);
    SW_ORM_ZVAL_STRING(function_name, "__construct", 1);

    int ret = sw_orm_call_user_function_ex(EG(function_table), object_pp, function_name, &construct_ret, param_count, params, 0, NULL TSRMLS_CC);

    sw_orm_zval_ptr_dtor(&function_name);
    
    sw_orm_zval_ptr_dtor(&construct_ret);
    return ret;
}

#endif
