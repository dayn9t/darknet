#pragma once

#include <cx/tom/type.h>

#ifdef __cplusplus
extern "C" {
#endif


/****************************** Image ******************************/

///图像
typedef struct {
} dn_image_t;

///加载图像
cx_status_t dn_image_load(dn_image_t **image, cx_path_t image_file);

///缩放图像,保持宽高比，并移动到目标图像中心，其余区域填充0.5
cx_status_t dn_image_letterbox(dn_image_t **image, dn_image_t *src, cx_size_t size);

///销毁图像
cx_status_t dn_image_destroy(dn_image_t *image);

///获取图像尺寸
cx_status_t dn_image_size(const dn_image_t *image, cx_size_t *size);

///获取图像数据
cx_status_t dn_image_data(dn_image_t *image, float **data);

/****************************** Network ******************************/

///网络对象
typedef struct {
} dn_network_t;

///获取网络分类数
cx_status_t dn_network_classes(const dn_network_t *net, size_t *classes);

///获取网络层数
cx_status_t dn_network_layers(const dn_network_t *net, size_t *layers);

///获取网络输入图片尺寸
cx_status_t dn_network_input_size(const dn_network_t *net, cx_size_t *size);

/****************************** Detector ******************************/

///检测对象
typedef struct {
} dn_detection_t;

///检测对象集
typedef struct {
    dn_detection_t *begin;
    size_t size;
} dn_detections_t;


///对象检测器，派生自Network
typedef struct {
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
cx_status_t dn_detector_detect(dn_detector_t *detector, dn_image_t *image, dn_detections_t *detections);

 ///清理检测结果数组元素.
cx_status_t dn_detections_clear(dn_detections_t *detections);

/**
 * 获取检测结果对象外接矩形.
 * @param detection     检测结果对象
 * @param box           外接矩形
 */
cx_status_t dn_detection_box(const dn_detections_t *detection, cx_rectf_t* box);

/**
 * 获取检测结果对象分类概率.
 * @param detection     检测结果对象
 * @param props         所有分类对应的概率，数据在detection销毁后失效
 */
cx_status_t dn_detection_props(const dn_detections_t *detection, cx_floats_t* props);


#ifdef __cplusplus
}
#endif
