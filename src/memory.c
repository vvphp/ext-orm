/*
  +----------------------------------------------------------------------+
  |                           swoole_orm                                 |
  +----------------------------------------------------------------------+
  | swoole_orm is a database ORM ext for mysql written in c language     |
  +----------------------------------------------------------------------+
  | LICENSE: https://github.com/caohao0730/swoole_orm/blob/master/LICENSE|
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/
#include "swoole_orm.h"
#include <stdarg.h>

char* sw_orm_multi_memcpy(char* source, int n_value, ...) {
	va_list var_arg;
	int count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(source + strlen(source), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	return source;
}

static sw_orm_inline size_t get_string_emalloc_size(char* source) {
	size_t source_size = 0;
	memcpy(&source_size, source - 4, sizeof(size_t));
	return source_size;
}

char* sw_orm_multi_memcpy_auto_realloc(char** source, int n_value, ...) {
	int source_size = get_string_emalloc_size(*source);
	
	va_list var_arg;
	int count = 0;
	int dest_len = strlen(*source) + 1;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		dest_len += strlen(tmp);
		count++;
	}
	va_end(var_arg);
	
	//need realloc
	char* dest = NULL;
	if(source_size < MM_REAL_SIZE(dest_len)) {
		sw_orm_string_emalloc_32(&dest, dest_len);
		memcpy(dest, *source, strlen(*source));
		sw_orm_string_efree_32(*source);
		*source = dest;
	} else {
		dest = *source;
	}
	
	//���¸�ֵ
	count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(dest + strlen(dest), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	
	//php_printf("====sw_orm_multi_memcpy_auto_alloc : source_size=[%d], dest_size=[%d], dest_size=[%d], \ndest_point=[%x]  dest=[%s]\n", source_size, dest_len, MM_REAL_SIZE(dest_len), dest, dest);
	return dest;
}
