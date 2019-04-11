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
#include "php_swoole_orm.h"
#include "zend_variables.h"

ZEND_DECLARE_MODULE_GLOBALS(swoole_orm)

//普通函数
const zend_function_entry swoole_orm_functions[] =
{
    PHP_FE(swoole_orm_version, arginfo_swoole_orm_void)
    PHP_FE_END
};

//模块实体定义
zend_module_entry swoole_orm_module_entry =
{
#if ZEND_MODULE_API_NO >= 20050922
    STANDARD_MODULE_HEADER_EX,
    NULL,
    NULL,
#else
    STANDARD_MODULE_HEADER,
#endif
    "swoole_orm",
    swoole_orm_functions,
    PHP_MINIT(swoole_orm),
    PHP_MSHUTDOWN(swoole_orm),
    PHP_RINIT(swoole_orm),     //RINIT
    PHP_RSHUTDOWN(swoole_orm), //RSHUTDOWN
    PHP_MINFO(swoole_orm),
    PHP_SWOOLE_ORM_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SWOOLE_ORM
ZEND_GET_MODULE(swoole_orm)
#endif

//类成员函数参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_construct_oo, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_select_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, join)
ZEND_ARG_INFO(0, columns)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_insert_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_replace_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_update_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_orm_delete_oo, 0, 0, 1)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()
 
//类的成员函数
static zend_function_entry swoole_orm_methods[] = {
    PHP_ME(swoole_orm, __construct, arginfo_swoole_orm_construct_oo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(swoole_orm, __destruct, arginfo_swoole_orm_void, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(swoole_orm, select, arginfo_swoole_orm_select_oo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_orm, insert, arginfo_swoole_orm_insert_oo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_orm, replace, arginfo_swoole_orm_replace_oo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_orm, update, arginfo_swoole_orm_update_oo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_orm, delete, arginfo_swoole_orm_delete_oo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    NULL, NULL, NULL
};

// ini 配置
PHP_INI_BEGIN()
PHP_INI_END()

static void php_swoole_orm_init_globals(zend_swoole_orm_globals *swoole_orm_globals)
{
}

PHP_MINIT_FUNCTION(swoole_orm)
{
	ZEND_INIT_MODULE_GLOBALS(swoole_orm, php_swoole_orm_init_globals, NULL);
    REGISTER_INI_ENTRIES();
    
    //初始化 swoole_orm 类
    INIT_CLASS_ENTRY(swoole_orm_ce, "swoole_orm", swoole_orm_methods);
    swoole_orm_ce_ptr = zend_register_internal_class(&swoole_orm_ce TSRMLS_CC);
    swoole_orm_ce_ptr->serialize = zend_class_serialize_deny;
    swoole_orm_ce_ptr->unserialize = zend_class_unserialize_deny;
    
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(swoole_orm)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(swoole_orm)
{
	DISPLAY_INI_ENTRIES();
}

PHP_RINIT_FUNCTION(swoole_orm)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(swoole_orm)
{
	return SUCCESS;
}

//Function swoole_orm_version
PHP_FUNCTION(swoole_orm_version)
{
    SW_ORM_RETURN_STRING(PHP_SWOOLE_ORM_VERSION, 1);
}

//swoole_orm构造函数
PHP_METHOD(swoole_orm, __construct) {
}

PHP_METHOD(swoole_orm, __destruct) {
}

//replace 替换
PHP_METHOD(swoole_orm, replace) {
    char *table = NULL;
    size_t table_len;
    zval *data = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data) == FAILURE) {
        RETURN_FALSE;
    }
    
    //解析data数据
    char *replace_sql, *replace_keys,*replace_value;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    sw_orm_string_emalloc_32(&replace_sql, 0);
    sw_orm_string_emalloc_32(&replace_keys, 0);
    sw_orm_string_emalloc_32(&replace_value, 0);

    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        sw_orm_string_efree_32(replace_keys);
        sw_orm_string_efree_32(replace_value);
        sw_orm_string_efree_32(replace_sql);
        sw_orm_php_fatal_error(E_WARNING, "input data must be key/value hash, not index array.");
        RETURN_FALSE;
    } else {
        sw_orm_multi_memcpy_auto_realloc(&replace_keys, 3, "`", key, "`,");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 1, "NULL,");
            break;
        case IS_ARRAY:
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 1, "ARRAY,");
            break;
        case IS_TRUE:
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 1, "1,");
            break;
        case IS_FALSE:
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 1, "0,");
            break;
        case IS_LONG:
            sw_orm_itoa(Z_LVAL_P(value), longval);
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 2, doubleval, ",");
            break;
        case IS_STRING:
            sw_orm_multi_memcpy_auto_realloc(&replace_value, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    SW_ORM_HASHTABLE_FOREACH_END();

    rtrim_str(replace_keys, ",");
    rtrim_str(replace_value, ",");
    sw_orm_multi_memcpy_auto_realloc(&replace_sql, 7, "REPLACE INTO `", table, "` (", replace_keys ,") values (", replace_value, ")");
    sw_orm_string_efree_32(replace_keys);
    sw_orm_string_efree_32(replace_value);


    zval *ret_val = NULL, *z_sql = NULL;
    SW_ORM_MAKE_STD_ZVAL(ret_val);
    array_init(ret_val);
    
    SW_ORM_MAKE_STD_ZVAL(z_sql);
    SW_ORM_ZVAL_STRING(z_sql, replace_sql, 1);
    sw_orm_string_efree_32(replace_sql);
    
    add_assoc_zval(ret_val, "sql", z_sql);
    RETVAL_ZVAL(ret_val, 1, 1);
    
}

//insert 插入
PHP_METHOD(swoole_orm, insert) {
    char *table = NULL;
    size_t table_len;
    zval *data = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data) == FAILURE) {
        RETURN_FALSE;
    }
    
    //解析data数据
    char *insert_sql, *insert_keys,*insert_value;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    sw_orm_string_emalloc_32(&insert_sql, 0);
    sw_orm_string_emalloc_32(&insert_keys, 0);
    sw_orm_string_emalloc_32(&insert_value, 0);

    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        sw_orm_string_efree_32(insert_keys);
        sw_orm_string_efree_32(insert_value);
        sw_orm_string_efree_32(insert_sql);
        sw_orm_php_fatal_error(E_WARNING, "input data must be key/value hash, not index array.");
        RETURN_FALSE;
    } else {
        sw_orm_multi_memcpy_auto_realloc(&insert_keys, 3, "`", key, "`,");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 1, "NULL,");
            break;
        case IS_ARRAY:
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 1, "ARRAY,");
            break;
        case IS_TRUE:
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            break;
        case IS_FALSE:
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            break;
        case IS_LONG:
            sw_orm_itoa(Z_LVAL_P(value), longval);
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 2, doubleval, ",");
            break;
        case IS_STRING:
            sw_orm_multi_memcpy_auto_realloc(&insert_value, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    SW_ORM_HASHTABLE_FOREACH_END();

    rtrim_str(insert_keys, ",");
    rtrim_str(insert_value, ",");
    sw_orm_multi_memcpy_auto_realloc(&insert_sql, 7, "INSERT INTO `", table, "` (", insert_keys ,") values (", insert_value, ")");
    sw_orm_string_efree_32(insert_keys);
    sw_orm_string_efree_32(insert_value);
    
    
    zval *ret_val = NULL, *z_sql = NULL;
    SW_ORM_MAKE_STD_ZVAL(ret_val);
    array_init(ret_val);
    
    SW_ORM_MAKE_STD_ZVAL(z_sql);
    SW_ORM_ZVAL_STRING(z_sql, insert_sql, 1);
    sw_orm_string_efree_32(insert_sql);
    
    add_assoc_zval(ret_val, "sql", z_sql);
    RETVAL_ZVAL(ret_val, 1, 1);
}

//update 更新
PHP_METHOD(swoole_orm, update) {
    char *table = NULL;
    size_t table_len;
    zval *data = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &where) == FAILURE) {
        RETURN_FALSE;
    }

    //更新语句
    char *update_sql;
    sw_orm_string_emalloc_32(&update_sql, 0);

    char *update_datas;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    sw_orm_string_emalloc_32(&update_datas, 0);

    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        sw_orm_string_efree_32(update_datas);
        sw_orm_string_efree_32(update_sql);
        sw_orm_php_fatal_error(E_WARNING, "input data must be key/value hash, not index array.");
        RETURN_FALSE;
    } else {
        sw_orm_multi_memcpy_auto_realloc(&update_datas, 3, "`", key, "` = ");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 1, "NULL,");
            break;
        case IS_ARRAY:
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 1, "ARRAY,");
            break;
        case IS_TRUE:
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 1, "1,");
            break;
        case IS_FALSE:
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 1, "0,");
            break;
        case IS_LONG:
            sw_orm_itoa(Z_LVAL_P(value), longval);
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 2, doubleval, ",");
            break;
        case IS_STRING:
            sw_orm_multi_memcpy_auto_realloc(&update_datas, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    SW_ORM_HASHTABLE_FOREACH_END();

    rtrim_str(update_datas, ",");
    sw_orm_multi_memcpy_auto_realloc(&update_sql, 4, "UPDATE `", table, "` SET ", update_datas);
    sw_orm_string_efree_32(update_datas);
    
    //where条件
    zval *map;
    SW_ORM_MAKE_STD_ZVAL(map);
    array_init(map);
		
    where_clause(where, map, & update_sql);
    
    zval *ret_val = NULL, *z_sql = NULL;
    SW_ORM_MAKE_STD_ZVAL(ret_val);
    array_init(ret_val);
    
    SW_ORM_MAKE_STD_ZVAL(z_sql);
    SW_ORM_ZVAL_STRING(z_sql, update_sql, 1);
    sw_orm_string_efree_32(update_sql);
    
    add_assoc_zval(ret_val, "sql", z_sql);
    add_assoc_zval(ret_val, "bind_value", map);
    RETVAL_ZVAL(ret_val, 1, 1);
}

//delete 删除
PHP_METHOD(swoole_orm, delete) {
    char *table = NULL;
    size_t table_len;
    zval *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &table, &table_len, &where) == FAILURE) {
        RETURN_FALSE;
    }

    //更新语句
    char *delete_sql;
    sw_orm_string_emalloc_32(&delete_sql, 0);
    sw_orm_multi_memcpy_auto_realloc(&delete_sql, 3, "DELETE FROM `", table, "` ");
	
    //where条件
    zval *map;
    SW_ORM_MAKE_STD_ZVAL(map);
    array_init(map);

    where_clause(where, map, & delete_sql);
    
    zval *ret_val = NULL, *z_sql = NULL;
    SW_ORM_MAKE_STD_ZVAL(ret_val);
    array_init(ret_val);
    
    SW_ORM_MAKE_STD_ZVAL(z_sql);
    SW_ORM_ZVAL_STRING(z_sql, delete_sql, 1);
    sw_orm_string_efree_32(delete_sql);
    
    add_assoc_zval(ret_val, "sql", z_sql);
    add_assoc_zval(ret_val, "bind_value", map);
    RETVAL_ZVAL(ret_val, 1, 1);
}

//获取 select
PHP_METHOD(swoole_orm, select) {
    char* table = NULL;
    size_t table_len;
    zval* join = NULL, *columns = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|zz", &table, &table_len, &join, &columns, &where) == FAILURE) {
        RETURN_FALSE;
    }

    //查询语句初始化
    char *sql;
    zval *map;
    
    SW_ORM_MAKE_STD_ZVAL(map);
    array_init(map);
    sw_orm_string_emalloc_32(&sql, 0);
    
    select_context(table, map, join, columns, where, &sql);
    
    zval *ret_val = NULL, *z_sql = NULL;
    SW_ORM_MAKE_STD_ZVAL(ret_val);
    array_init(ret_val);
    
    SW_ORM_MAKE_STD_ZVAL(z_sql);
    SW_ORM_ZVAL_STRING(z_sql, sql, 1);
    sw_orm_string_efree_32(sql);
    
    add_assoc_zval(ret_val, "sql", z_sql);
    add_assoc_zval(ret_val, "bind_value", map);
    RETVAL_ZVAL(ret_val, 1, 1);
}

char *select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql) {
    //表别名
    char* table_query;
    sw_orm_string_emalloc_32(&table_query, 0);

    char table_match[MAX_TABLE_SIZE] = {0};
    char alias_match[MAX_TABLE_SIZE] = {0};
    preg_table_match(table, table_match, alias_match);
    if (sw_orm_is_string_not_empty(table_match) && sw_orm_is_string_not_empty(alias_match)) {
        sw_orm_multi_memcpy_auto_realloc(&table_query, 5, "`", table_match, "` AS `", alias_match, "`");
    } else {
        sw_orm_multi_memcpy_auto_realloc(&table_query, 3, "`", table, "`");
    }

    //解析 join
    char* first_join_key = NULL;
    zval* real_where = where;
    zval* real_columns = columns;

    if (SW_ORM_IS_ARRAY(join) && (first_join_key = sw_orm_get_array_key_index(join, 0)) != NULL && sw_orm_strpos(first_join_key, "[") == 0) { //第二个参数不为空，而且判断是 join 的情况
        if (sw_orm_is_string_not_empty(alias_match)) {
            handle_join(join, alias_match, &table_query);
        } else {
            handle_join(join, table, &table_query);
        }
    } else {
        if (SW_ORM_IS_NULL(where)) { //第四个参数为空时，第二个参数为 column , 第三个参数为 where
            real_columns = join;
            real_where = columns;
        }
    }

    //选择要查询的列
    char* column_query;
    sw_orm_string_emalloc_32(&column_query, 0);

    column_push(real_columns, map, &column_query);

    sw_orm_multi_memcpy_auto_realloc(sql, 4, "SELECT ", column_query, " FROM ", table_query);

    sw_orm_string_efree_32(column_query);
    sw_orm_string_efree_32(table_query);
		
    where_clause(real_where, map, sql);
    return *sql;
}

//处理 where 条件
char* where_clause(zval* where, zval* map, char** sql) {
    if (SW_ORM_IS_EMPTY(where)) {
        return *sql;
    }

    char* group_by_condition = NULL;
    char* having_condition = NULL;
    char* order_condition = NULL;
    char* limit_condition = NULL;

    char* where_condition = NULL;
    sw_orm_string_emalloc_32(&where_condition, 0);

    if (SW_ORM_IS_ARRAY(where)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(where), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            if (strcmp(key, "GROUP") == 0) { //分组
                sw_orm_string_emalloc_32(&group_by_condition, 0);
                group_by_implode(value, &group_by_condition);
            } else if (strcmp(key, "HAVING") == 0) { //分组条件
                sw_orm_string_emalloc_32(&having_condition, 0);
                having_implode(value, map, &having_condition);
            } else if (strcmp(key, "ORDER") == 0) { //排序
                sw_orm_string_emalloc_32(&order_condition, 0);
                order_implode(value, &order_condition);
            } else if (strcmp(key, "LIMIT") == 0) { //结果条数
                sw_orm_string_emalloc_32(&limit_condition, 0);
                limit_implode(value, &limit_condition);
            } else { // where clause
                where_implode(key, value, map, &where_condition, "AND");
            }
        }
        SW_ORM_HASHTABLE_FOREACH_END();

        strreplace(where_condition, "( AND", "(");
        trim(ltrim_str(ltrim(where_condition), "AND"));
        if (where_condition[0] != '\0') {
            sw_orm_multi_memcpy_auto_realloc(sql, 2, " WHERE ", where_condition);
        }
    }

    sw_orm_string_efree_32(where_condition);

    if (group_by_condition != NULL) {
        sw_orm_multi_memcpy_auto_realloc(sql, 2, " GROUP BY ", group_by_condition);
        sw_orm_string_efree_32(group_by_condition);
    }

    if (having_condition != NULL) {
        sw_orm_multi_memcpy_auto_realloc(sql, 2, " HAVING ", having_condition);
        sw_orm_string_efree_32(having_condition);
    }

    if (order_condition != NULL) {
        sw_orm_multi_memcpy_auto_realloc(sql, 2, " ORDER BY ", order_condition);
        sw_orm_string_efree_32(order_condition);
    }

    if (limit_condition != NULL) {
        sw_orm_multi_memcpy_auto_realloc(sql, 2, " LIMIT ", limit_condition);
        sw_orm_string_efree_32(limit_condition);
    }

    return *sql;
}

//where condition
char* where_implode(char* key, zval* value, zval* map, char** where_query, char* connector) {
    //key是否是 AND 或者 OR
    char relation_ship[MAX_OPERATOR_SIZE] = {0};
    preg_and_or_match(key, relation_ship);

    if (Z_TYPE_P(value) == IS_ARRAY && sw_orm_is_string_not_empty(relation_ship)) {
        char* relation_key;
        zval* relation_value;
        uint32_t relation_key_len;
        int relation_key_type;

        char* sub_where_clause = NULL;
        sw_orm_string_emalloc_32(&sub_where_clause, 0);

        sw_orm_multi_memcpy_auto_realloc(where_query, 1, " AND (");

        SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(value), relation_key, relation_key_len, relation_key_type, relation_value)
        if (HASH_KEY_IS_STRING != relation_key_type) {
            continue;
        } else {
            where_implode(relation_key, relation_value, map, &sub_where_clause, relation_ship);
        }
        SW_ORM_HASHTABLE_FOREACH_END();

        sw_orm_multi_memcpy_auto_realloc(where_query, 2, sub_where_clause, ")");
        sw_orm_string_efree_32(sub_where_clause);
        return *where_query;
    }

    char column[MAX_TABLE_SIZE] = {0};
    char operator[MAX_OPERATOR_SIZE] = {0};
    preg_operator_match(key, column, operator);

    if (sw_orm_is_string_not_empty(column)) {

        column_quote(column, column);

        if (sw_orm_is_string_not_empty(operator)) {
            if (strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0) { // >, >=, <, <=
                add_map(map, value);
                sw_orm_multi_memcpy_auto_realloc(where_query, 4, connector, column, operator, " ? ");
            } else if (strcmp(operator, "!") == 0) { //not equal
                switch (Z_TYPE_P(value)) {
                case IS_NULL:
                    sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NOT NULL ");
                    break;
                case IS_ARRAY:
                    sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "NOT IN (");
                    handle_where_not_in(value, where_query, map);
                    sw_orm_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    break;
                case IS_LONG:
                case IS_STRING:
                case IS_DOUBLE:
                case IS_TRUE:
                case IS_FALSE:
                    add_map(map, value);
                    sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "!= ? ");
                    break;
                }
            } else if (strcmp(operator, "~") == 0 ||  strcmp(operator, "!~") == 0) { //like
                if (Z_TYPE_P(value) == IS_STRING) {
                    add_map(map, value);
                    sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, (strcmp(operator, "~") == 0 ? "LIKE ? " : "NOT LIKE ? "));
                } else if (Z_TYPE_P(value) == IS_ARRAY) {
                    char* like_connector = sw_orm_get_array_key_index(value, 0);
                    if (like_connector != NULL && (strcmp(like_connector, "AND") == 0 || strcmp(like_connector, "OR") == 0)) {
                        //自定义LIKE连接符
                        zval* connetor_value = php_sw_orm_array_get_value(Z_ARRVAL_P(value), like_connector);

                        if (Z_TYPE_P(connetor_value) == IS_ARRAY) {
                            sw_orm_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                            handle_like_array(connetor_value, where_query, column, operator, map, like_connector);
                            sw_orm_multi_memcpy_auto_realloc(where_query, 1, ") ");
                        }
                    } else { //默认括号内LIKE连接符为 OR
                        sw_orm_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                        handle_like_array(value, where_query, column, operator, map, "OR");
                        sw_orm_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    }
                }
            } else if (strcmp(operator, "<>") == 0 ||  strcmp(operator, "><") == 0) {
                if (Z_TYPE_P(value) == IS_ARRAY) {
                    zval* between_a = sw_orm_get_element_by_hashtable_index(Z_ARRVAL_P(value), 0);
                    zval* between_b = sw_orm_get_element_by_hashtable_index(Z_ARRVAL_P(value), 1);
                    if (SW_ORM_IS_NOT_EMPTY(between_a) && (Z_TYPE_P(between_a) == IS_LONG || Z_TYPE_P(between_a) == IS_STRING)
                            && SW_ORM_IS_NOT_EMPTY(between_b) && (Z_TYPE_P(between_b) == IS_LONG || Z_TYPE_P(between_b) == IS_STRING)) {
                        sw_orm_multi_memcpy_auto_realloc(where_query, 2, connector, " ");
                        if (strcmp(operator, "><") == 0) {
                            sw_orm_multi_memcpy_auto_realloc(where_query, 1, "NOT ");
                        }
                        add_map(map, between_a);
                        sw_orm_multi_memcpy_auto_realloc(where_query, 3, "(", column, "BETWEEN ? ");
                        add_map(map, between_b);
                        sw_orm_multi_memcpy_auto_realloc(where_query, 1, "AND ?) ");
                    }
                }
            }
        } else {
            switch (Z_TYPE_P(value)) {
            case IS_NULL:
                sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NULL ");
                break;
            case IS_ARRAY:
                sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IN (");
                handle_where_not_in(value, where_query, map);
                sw_orm_multi_memcpy_auto_realloc(where_query, 1, ") ");
                break;
            case IS_LONG:
            case IS_STRING:
            case IS_DOUBLE:
            case IS_TRUE:
            case IS_FALSE:
                add_map(map, value);
                sw_orm_multi_memcpy_auto_realloc(where_query, 3, connector, column, "= ? ");
                break;
            }
        }
    }

    ltrim_str(*where_query, connector);
    return *where_query;
}

//handle group by
char* group_by_implode(zval* group, char** group_by_condition) {
    if (SW_ORM_IS_NOT_EMPTY(group)) {
        if (Z_TYPE_P(group) == IS_STRING) {
            sw_orm_multi_memcpy_auto_realloc(group_by_condition, 1, Z_STRVAL_P(group));
        } else if (Z_TYPE_P(group) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;


            SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(group), key, key_len, key_type, value)
            if (Z_TYPE_P(value) == IS_STRING) {
                sw_orm_multi_memcpy_auto_realloc(group_by_condition, 2, Z_STRVAL_P(value), ",");
            }
            SW_ORM_HASHTABLE_FOREACH_END();

            char* tmp = (*group_by_condition) +  strlen(*group_by_condition) - 1;
            if (*tmp == ',') {
                *tmp = '\0';
            }
        }
    }
    return *group_by_condition;
}

//handle having
char* having_implode(zval* having, zval* map, char** having_condition) {

    if (SW_ORM_IS_ARRAY(having)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(having), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            where_implode(key, value, map, having_condition, "AND");
        }
        SW_ORM_HASHTABLE_FOREACH_END();
    }

    strreplace(*having_condition, "( AND", "(");
    trim(ltrim_str(ltrim(*having_condition), "AND"));
    return *having_condition;
}

//order by
char* order_implode(zval* order, char** order_condition) {
    if (SW_ORM_IS_NOT_EMPTY(order)) {
        if (Z_TYPE_P(order) == IS_STRING) {
            sw_orm_multi_memcpy_auto_realloc(order_condition, 1, Z_STRVAL_P(order));
        } else if (Z_TYPE_P(order) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;

            SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(order), key, key_len, key_type, value)
            if (HASH_KEY_IS_STRING != key_type) {
                if (Z_TYPE_P(value) == IS_STRING) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(Z_STRVAL_P(value), column);
                    sw_orm_multi_memcpy_auto_realloc(order_condition, 2, column, ",");
                }
            } else {
                if (Z_TYPE_P(value) == IS_STRING && (strcmp(Z_STRVAL_P(value), "ASC") == 0 || strcmp(Z_STRVAL_P(value), "DESC") == 0)) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(key, column);
                    sw_orm_multi_memcpy_auto_realloc(order_condition, 3, column, Z_STRVAL_P(value), ",");
                }
            }
            SW_ORM_HASHTABLE_FOREACH_END();
            rtrim_str(*order_condition, ",");
        }
    }
    return *order_condition;
}

//limit
char* limit_implode(zval* limit, char** limit_condition) {
    if (SW_ORM_IS_NOT_EMPTY(limit)) {
        if (Z_TYPE_P(limit) == IS_STRING || Z_TYPE_P(limit) == IS_LONG) {
            convert_to_string(limit);
            if (is_numeric_string(Z_STRVAL_P(limit), Z_STRLEN_P(limit), NULL, NULL, 0)) {
                sw_orm_multi_memcpy_auto_realloc(limit_condition, 1, Z_STRVAL_P(limit));
            }
        } else if (Z_TYPE_P(limit) == IS_ARRAY) {
            zval* offset_val = sw_orm_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 0);
            zval* limit_val = sw_orm_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 1);
            convert_to_string(limit_val);
            convert_to_string(offset_val);

            if (is_numeric_string(Z_STRVAL_P(limit_val), Z_STRLEN_P(limit_val), NULL, NULL, 0)
                    && is_numeric_string(Z_STRVAL_P(offset_val), Z_STRLEN_P(offset_val), NULL, NULL, 0)) {
                sw_orm_multi_memcpy_auto_realloc(limit_condition, 3, Z_STRVAL_P(limit_val), " OFFSET ", Z_STRVAL_P(offset_val));
            }
        }
    }

    return *limit_condition;
}

//like array情况
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;
    
    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(like_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        add_map(map, value);
        sw_orm_multi_memcpy_auto_realloc(where_query, 3, column, strcmp(operator, "~") == 0 ? "LIKE ? " : "NOT LIKE ? ", connector);
    }
    SW_ORM_HASHTABLE_FOREACH_END();
    rtrim_str(rtrim(*where_query), connector);
    return *where_query;
}

//not in 情况
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;

    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(not_in_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        add_map(map, value);
        sw_orm_multi_memcpy_auto_realloc(where_query, 1, " ?,");
    }
    SW_ORM_HASHTABLE_FOREACH_END();

    rtrim_str(rtrim(*where_query), ",");
    return *where_query;
}

zval* add_map(zval* map, zval* value) {
    zval *copy = sw_orm_zval_copy(value);
    add_next_index_zval(map, copy);
    return map;
}

//处理查询列
char* column_push(zval* columns, zval* map, char** column_query) {
    if (SW_ORM_IS_EMPTY(columns) || (Z_TYPE_P(columns) == IS_STRING && strcmp(Z_STRVAL_P(columns), "*") == 0)) {
        sw_orm_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }

    if (Z_TYPE_P(columns) == IS_STRING) {
        sw_orm_multi_memcpy_auto_realloc(column_query, 1, Z_STRVAL_P(columns));
        return *column_query;
    } else if (SW_ORM_IS_ARRAY(columns)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(columns), key, key_len, key_type, value)
        if (Z_TYPE_P(value) != IS_STRING) {
            continue;
        }

        char match_column[MAX_TABLE_SIZE] = {0};
        char match_alias[MAX_TABLE_SIZE] = {0};
        preg_table_match(Z_STRVAL_P(value), match_column, match_alias);

        if (sw_orm_is_string_not_empty(match_column) && sw_orm_is_string_not_empty(match_alias)) {
            sw_orm_multi_memcpy_auto_realloc(column_query, 4, match_column, " AS `", match_alias, "`,");
        } else {
            sw_orm_multi_memcpy_auto_realloc(column_query, 2, Z_STRVAL_P(value), ",");
        }

        SW_ORM_HASHTABLE_FOREACH_END();

        rtrim_str(rtrim(*column_query), ",");
        return *column_query;
    } else {
        sw_orm_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }
}

//处理join
char* handle_join(zval *join, char *table, char** table_query) {
    char* sub_table;
    zval* relation;
    uint32_t key_len;
    int key_type;

    SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(join), sub_table, key_len, key_type, relation)
    if (HASH_KEY_IS_STRING != key_type) { //非字符串
        continue;
    }

    char join_join[MAX_TABLE_SIZE] = {0};
    char join_table[MAX_TABLE_SIZE] = {0};
    char join_alias[MAX_TABLE_SIZE] = {0};
    preg_join_match(sub_table, join_join, join_table, join_alias);


    if (sw_orm_is_string_not_empty(join_join) && sw_orm_is_string_not_empty(join_table)) {
        sw_orm_multi_memcpy_auto_realloc(table_query, 5, " ", get_join_type(join_join), " JOIN `", join_table, "` ");
        if (sw_orm_is_string_not_empty(join_alias)) {
            sw_orm_multi_memcpy_auto_realloc(table_query, 3, "AS `", join_alias, "` ");
        }

        if (Z_TYPE_P(relation) == IS_STRING) {
            sw_orm_multi_memcpy_auto_realloc(table_query, 3, "USING (`", Z_STRVAL_P(relation), "`) ");
        } else if (Z_TYPE_P(relation) == IS_ARRAY) {
            if (is_set_array_index(Z_ARRVAL_P(relation), 0)) { //relation 为数组
                sw_orm_multi_memcpy_auto_realloc(table_query, 1, "USING (`");
                sw_orm_implode(relation, "`,`", table_query);
                sw_orm_multi_memcpy_auto_realloc(table_query, 1, "`) ");
            } else { //relation 为 Key Hash
                char *key;
                zval *value;

                sw_orm_multi_memcpy_auto_realloc(table_query, 1, "ON ");

                SW_ORM_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(relation), key, key_len, key_type, value)
                if (HASH_KEY_IS_STRING != key_type) { //非字符串
                    continue;
                }

                char* table_column = NULL;
                sw_orm_string_emalloc_32(&table_column, 0);
                if (sw_orm_strpos(key, ".") >= 0) {
                    column_quote(key, table_column);
                } else {
                    sw_orm_multi_memcpy_auto_realloc(&table_column, 5, "`", table, "`.`", key, "`");
                }

                //alias
                if (sw_orm_is_string_not_empty(join_alias)) {
                    sw_orm_multi_memcpy_auto_realloc(table_query, 4, table_column, "=`", join_alias, "`");
                } else {
                    sw_orm_multi_memcpy_auto_realloc(table_query, 4, table_column, "= `", join_table, "`");
                }

                sw_orm_string_efree_32(table_column);

                sw_orm_multi_memcpy_auto_realloc(table_query, 3, ".`", Z_STRVAL_P(value), "` AND");
                SW_ORM_HASHTABLE_FOREACH_END();

                rtrim_str(rtrim(*table_query), "AND");
            }
        }

    }
    SW_ORM_HASHTABLE_FOREACH_END();
    return *table_query;
}

char* column_quote(char* string, char* table_column) {
    char tmp[MAX_TABLE_SIZE] = {0};

    sprintf(tmp, " `%s` ", string);

    if (strlen(tmp) >= MAX_TABLE_SIZE) {
        sw_orm_php_fatal_error(E_ERROR, "column size is too long, [%s]", string);
    }

    if (sw_orm_strpos(tmp, ".") >= 0) {
        if (strlen(tmp) + 5 >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "column + alias size is too long, [%s]", string);
        }
        strreplace(tmp, ".", "`.`");
    }

    strcpy(table_column, tmp);
    return table_column;
}

char* get_join_type(char* type) {
    if (strcmp(type, "<") == 0) {
        return "LEFT";
    } else if (strcmp(type, ">") == 0) {
        return "RIGHT";
    } else if (strcmp(type, "<>") == 0) {
        return "FULL";
    } else if (strcmp(type, "><") == 0) {
        return "INNER";
    } else {
        return "";
    }
}

//匹配表和别名
int preg_join_match(char* key, char* join, char* table, char* alias) {
    int join_start = -1;
    int join_end = -1;
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    join[0] = '\0';
    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( join_start == -1 && c_key == '[') {
            join_start = i;
        }

        if (table_start == -1 && join_start == -1 && c_key != '[' && !sw_orm_is_space(c_key)) {
            table_start = i;
        }

        if (join_end != -1 && table_start == -1 && !sw_orm_is_space(c_key)) {
            table_start = i;
        }

        if ( join_start != -1 && c_key == ']') {
            join_end = i;
        }

        if (table_start != -1 && c_key == '(') {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "join table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "join alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    if (join_start != -1 && join_end != -1 && join_start < join_end) {
        if (join_end - join_start >= MAX_OPERATOR_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "join operator size is too long, [%s]", key);
        }

        memset(join, 0, MAX_OPERATOR_SIZE);
        memcpy(join, key + join_start + 1, join_end - join_start - 1);
        if (!(strcmp(join, ">") == 0 || strcmp(join, "<") == 0 || strcmp(join, "<>") == 0 || strcmp(join, "><") == 0)) {
            join[0] = '\0';
        }
    }
    return 1;
}

//匹配表和别名
int preg_table_match(char* key, char* table, char* alias) {
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( table_start == -1 && !sw_orm_is_space(c_key)) {
            table_start = i;
        }

        if (table_end == -1 && (c_key == '(' || sw_orm_is_space(c_key))) {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    return 1;
}

//匹配列名和操作符
int preg_operator_match(char* key, char* column, char* operator) {
    int column_start = -1;
    int column_end = -1;
    int column_end_is_space = -1;
    int operator_start = -1;
    int operator_end = -1;

    int key_len = strlen(key);

    column[0] = '\0';
    operator[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( column_start == -1 && !sw_orm_is_space(c_key)) {
            column_start = i;
        }

        if (column_end == -1 && (c_key == '[' || sw_orm_is_space(c_key))) {
            column_end = i;
        }

        if (column_end_is_space == -1 && sw_orm_is_space(c_key)) {
            column_end_is_space = i;
        }

        if ( operator_start == -1 && c_key == '[') {
            operator_start = i;
        }

        if ( operator_end == -1 && c_key == ']') {
            operator_end = i;
        }
    }

    if (operator_start == -1 || operator_end == -1 || operator_start > operator_end) {
        column_end = column_end_is_space == -1 ? key_len : column_end_is_space;
    }

    if (column_start != -1 && column_end != -1 && column_end > column_start) {
        if (column_end - column_start - 1 >= MAX_TABLE_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "column size is too long [%s]", key);
        }

        memset(column, 0, MAX_TABLE_SIZE);
        memcpy(column, key + column_start, column_end - column_start);
    }

    if (operator_start != -1 && operator_end != -1 && operator_start < operator_end) {
        if (operator_end - operator_start - 1 >= MAX_OPERATOR_SIZE) {
            sw_orm_php_fatal_error(E_ERROR, "operator size is too long [%s]", key);
        }

        memset(operator, 0, MAX_OPERATOR_SIZE);
        memcpy(operator, key + operator_start + 1, operator_end - operator_start - 1);
        if (!(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 ||
                strcmp(operator, "!") == 0 || strcmp(operator, "~") == 0 || strcmp(operator, "!~") == 0 || strcmp(operator, "<>") == 0 || strcmp(operator, "><") == 0)) {
            operator[0] = '\0';
        }
    }

    return 1;
}

//匹配是否 AND 或者 OR
int preg_and_or_match(char* key, char* relation) {
    int relation_start = -1;
    int relation_end = -1;

    relation[0] = '\0';

    int key_len = strlen(key);
    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];

        if ( relation_start == -1 && !sw_orm_is_space(c_key)) {
            relation_start = i;
        }

        if (relation_end == -1 && ( c_key == '#' || sw_orm_is_space(c_key))) {
            relation_end = i;
        }

        if (relation_end == -1 && i == key_len - 1) {
            relation_end = key_len;
        }
    }

    if (relation_start != -1 && relation_end != -1 && relation_end > relation_start && relation_end - relation_start - 1 < MAX_OPERATOR_SIZE) {
        memset(relation, 0, MAX_OPERATOR_SIZE);
        memcpy(relation, key + relation_start, relation_end - relation_start);
        if (strcmp(relation, "AND") != 0 && strcmp(relation, "OR") != 0 && strcmp(relation, "and") != 0 && strcmp(relation, "or") != 0 ) {
            relation[0] = '\0';
        }
    }

    return 1;
}
