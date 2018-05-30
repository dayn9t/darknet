
#include <darknetx.h>
#include <darknet.h>
#include <cx/tom/op.h>
#include <stdlib.h>
#include <assert.h>


/*
    struct image
    @data    按照RGB顺序排列的planner数据

*/

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


cx_status_t dn_network_input_size(const dn_network_t *net, cx_size_t *size)
{
    network *me = (network *) net;
    size->width = me->w;
    size->height = me->h;
    return cx_ok;
}


/****************************** Detector ******************************/

cx_status_t dn_detector_load(dn_detector_t **detector, cx_path_t cfg_file, cx_path_t weight_file)
{
    network *net = load_network(cfg_file, weight_file, 0);
    set_batch_network(net, 1);

    dn_detector_t *d = calloc(sizeof(dn_detector_t), 1);
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


cx_status_t dn_detector_detect(dn_detector_t *detector, dn_image_t *image, dn_detections_t *detections)
{
    cx_size_t net_size;
    dn_network_input_size(detector->net, &net_size);
    cx_size_t image_size;
    dn_image_size(image, &image_size);

    dn_image_t *sized;
    dn_image_letterbox(&sized, image, net_size);

    network *net = (network *) detector->net;
    double time = what_time_is_it_now();
    float *data;
    dn_image_data(sized, &data);
    network_predict(net, data);
    printf("Predicted in %f seconds.\n", what_time_is_it_now() - time);

    int boxes = 0;
    detection *dets = get_network_boxes(net, image_size.width, image_size.height, detector->thresh,
                                        detector->hier_thresh, 0, 1, &boxes);

    size_t classes;
    dn_network_classes(detector->net, &classes);

    float nms = .45;
    do_nms_sort(dets, boxes, classes, nms);

    detections->size = boxes;
    detections->begin = (dn_detection_t *) dets;

    return cx_ok;
}


cx_status_t dn_detections_release(dn_detections_t *detections)
{
    free_detections((detection*)detections->begin, detections->size);
    return cx_ok;
}


cx_status_t dn_detection_box(const dn_detections_t *_det, cx_rectf_t *_box)
{
    detection *me = (detection *) _det;
    box b = me->bbox;
    _box->x = b.x - b.w / 2;
    _box->y = b.y - b.h / 2;
    _box->width = b.w;
    _box->height = b.h;
    return cx_ok;
}

cx_status_t dn_detection_props(const dn_detections_t *_det, cx_floats_t* props)
{
    detection *me = (detection *) _det;
    cx_buffer_float_t b = { me->classes, me->prob };
    cx_vec_copy(b, props);
    return cx_ok;
}
