#pragma once

#include <cx/tom/type.h>

#ifdef __cplusplus
extern "C" {
#endif


/****************************** Network ******************************/

///网络对象
typedef struct {
} dn_network_t;

///获取网络分类数
cx_status_t dn_network_classes(const dn_network_t *net, size_t *classes);

///获取网络层数
cx_status_t dn_network_layers(const dn_network_t *net, size_t *layers);

///获取网络输入图片尺寸
cx_status_t dn_network_input_shape(const dn_network_t *net, cx_mat_shape_t *shape);


/****************************** Detection ******************************/

///检测对象
typedef struct {
    cx_rectf_t box;             ///<外接矩形
    cx_buffer_float_t props;    ///<所有分类对应的概率，数据在detection销毁后失效
} dn_detection_t;

///检测对象集
typedef struct {
    void *begin;
    size_t size;                ///<检测对象数量
} dn_detections_t;

 ///清理检测结果数组元素.
cx_status_t dn_detections_clear(dn_detections_t *detections);

/**
 * 获取检测结果集合元素.
 * @param detections    检测结果对象
 * @param index         元素索引
 */
cx_status_t dn_detections_at(const dn_detections_t *detections, size_t index, dn_detection_t* elem);

/****************************** Detector ******************************/

///对象检测器，派生自Network
typedef struct dn_detector {
    dn_network_t *net;
    float thresh;
    float hier_thresh;
} dn_detector_t;


/**
 * 加载检测器.
 * @param detector      检测器
 * @param cfg_file      网络结构文件
 * @param weight_file   网络权重文件
 */
cx_status_t dn_detector_load(dn_detector_t **detector, cx_path_t cfg_file, cx_path_t weight_file);

///销毁检测器
cx_status_t dn_detector_destroy(dn_detector_t *detector);

/**
 * 检测器开始检测.
 * @param detector      检测器
 * @param image         输入图像
 * @param detections    检测结果数组，使用后要用dn_detections_clear释放元素（DN设计缺陷）
 * @see dn_detections_clear
 */
cx_status_t dn_detector_detect(dn_detector_t *detector, cx_imagef_t *image, dn_detections_t *detections);




#ifdef __cplusplus
}
#endif
