#pragma once

#include <stddef.h>


/****************************** Common ******************************/

///路径
typedef const char* cx_path_t;


/****************************** Status ******************************/

///状态
typedef struct {
    int code;
} cx_status_t;


///判定状态是否OK
#define cx_if_ok(s) (s.code == 0)

///正常状态
const extern cx_status_t cx_ok;

/****************************** GEO ******************************/

///2D点
typedef struct {
    int x;
    int y;
} cx_point_t;

///2D点
typedef struct {
    float x;
    float y;
} cx_pointf_t;

///2D尺寸
typedef struct {
    int width;
    int height;
} cx_size_t;

///2D尺寸
typedef struct {
    float width;
    float height;
} cx_sizef_t;

///矩形
typedef struct {
    int x;
    int y;
    int width;
    int height;
} cx_rect_t;

///矩形
typedef struct {
    float x;
    float y;
    float width;
    float height;
} cx_rectf_t;

/****************************** Array ******************************/

///整数数组
typedef struct {
    int* begin;
    size_t size;
} cx_ints_t;


///整数数组
typedef struct {
    float* begin;
    size_t size;
} cx_floats_t;


/****************************** Misc ******************************/

///创建为对象
#define cx_new(T)			(T*)calloc(sizeof(T), 1)

///创建对象的新拷贝
void* cx_new_copy(size_t size, const void *src);

///创建对象值的副本
#define cx_clone(v) cx_new_copy(sizeof(v), &v)