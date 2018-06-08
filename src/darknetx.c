
#include <darknetx.h>
#include <darknet.h>
#include <cx/tom/op.h>
#include <stdlib.h>
#include <assert.h>


/*
    struct image
    @data    按照RGB顺序排列的planner数据

*/

#if 0
/****************************** Image ******************************/

cx_status_t dn_image_load(dn_image_t **_image, cx_path_t image_file)
{
    image im = load_image_color(image_file, 0, 0);   //DN缺陷
    *_image = cx_clone(im);
    return cx_ok;
}


cx_status_t dn_image_letterbox(dn_image_t **_image, dn_image_t *src, cx_size_t size)
{
    image im = letterbox_image(*(image *) src, size.width, size.height);
    *_image = cx_clone(im);
    return cx_ok;
}


cx_status_t dn_image_destroy(dn_image_t *_image)
{
    free_image(*(image *) _image);
}


cx_status_t dn_image_size(const dn_image_t *_image, cx_size_t *size)
{
    image *me = (image *) _image;
    size->width = me->w;
    size->height = me->h;
    return cx_ok;
}


cx_status_t dn_image_data(dn_image_t *_image, float **data)
{
    image *me = (image *) _image;
    *data = me->data;
    return cx_ok;
}
#endif


/****************************** Network ******************************/

cx_status_t dn_network_classes(const dn_network_t *net, size_t *classes)
{
    network *me = (network *) net;
    layer l = me->layers[me->n - 1];

    *classes = l.classes;
    return cx_ok;
}


cx_status_t dn_network_layers(const dn_network_t *net, size_t *layers)
{
    network *me = (network *) net;
    *layers = me->n;
    return cx_ok;
}


cx_status_t dn_network_input_shape(const dn_network_t *net, cx_mat_shape_t *shape)
{
    network *me = (network *) net;
    shape->batch = me->batch;
    shape->channels = me->c;
    shape->rows = me->h;
    shape->cols = me->w;
    return cx_ok;
}

/****************************** Detection ******************************/

cx_status_t dn_detections_clear(dn_detections_t *dets)
{
    free_detections((detection*)dets->begin, dets->size);
    dets->begin = 0;
    dets->size = 0;
    return cx_ok;
}

cx_status_t dn_detections_at(const dn_detections_t *dets, size_t index, dn_detection_t* elem)
{
    const detection *me = (const detection *) dets->begin + index;
    box b = me->bbox;

    cx_rectf_t box = { b.x - b.w / 2, b.y - b.h / 2, b.w, b.h };
    cx_buffer_float_t props = { me->classes, me->prob };
    
    elem->box = box;
    elem->props = props;
    return cx_ok;
}

/****************************** Detector ******************************/

cx_status_t dn_detector_load(dn_detector_t **detector, cx_path_t cfg_file, cx_path_t weight_file)
{
    network *net = load_network(cfg_file, weight_file, 0);
    set_batch_network(net, 1);

    dn_detector_t *d = calloc(sizeof(dn_detector_t), 1);
    d->thresh = 0.5;
    d->hier_thresh = 0.5;
    d->net = (dn_network_t *) net;
    *detector = d;
    srand(2222222);
    return cx_ok;
}


cx_status_t dn_detector_destroy(dn_detector_t *detector)
{
    if (detector) {
        free_network((network *) detector->net);
        free(detector);
    }
    return cx_ok;
}

///cx_imagef_t 转换为 image
static image to_image(cx_imagef_t* cx_im)
{
    image dn_im = {cx_im->size.width, cx_im->size.height, cx_im->channel, cx_im->data};
    return dn_im;
}


cx_status_t dn_detector_detect(dn_detector_t* detector, cx_imagef_t* _image, dn_detections_t *detections)
{
    //TODO:dn_detections_t需要提取出来，但mask分配有点复杂

    image ori_image = to_image(_image);
    cx_mat_shape_t in_shape;
    dn_network_input_shape(detector->net, &in_shape);
    cx_size_t ori_size = _image->size;

    image sized_image = letterbox_image(ori_image, in_shape.cols, in_shape.rows);

    network *net = (network *) detector->net;
    double time = what_time_is_it_now();    
    network_predict(net, sized_image.data);
    double time1 = what_time_is_it_now();
    //printf("Predicted in %f seconds.\n", time1 - time);
    int boxes = 0;
    detection *dets = get_network_boxes(net, ori_size.width, ori_size.height, detector->thresh,
                                        detector->hier_thresh, 0, 1, &boxes);
    double time2 = what_time_is_it_now();
    //printf("Predicted in %f seconds.\n", time2 - time1);

    size_t classes;
    dn_network_classes(detector->net, &classes);
    //printf("dets: %d %ld %f\n", boxes, classes, nms);
    float nms = .45;
    do_nms_sort(dets, boxes, classes, nms);
    double time3 = what_time_is_it_now();
    //printf("Predicted in %f seconds.\n", time3 - time2);

    detections->size = boxes;
    detections->begin = (dn_detection_t *) dets;

    return cx_ok;
}

