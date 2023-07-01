

#include "pico/stdlib.h"
#include <stdio.h>
#include <inttypes.h>
#include "hardware/gpio.h"
#include "classifier.h"
#include "edge/edge-impulse-sdk/classifier/ei_run_classifier.h"

#define WIDTH 176
#define HEIGHT 144
#define SLIDE_WINDOW 112
#define HOP 36


const uint8_t* img; 
static uint8_t img_cut [36*36];
static int current_index = -1;


int cut(int offset)
{
    if((WIDTH - offset%WIDTH) < HOP || (SLIDE_WINDOW - offset%WIDTH) < HOP)
    {
        offset += (HOP * WIDTH) - (offset%WIDTH) + 4;
    }

    if((HEIGHT*WIDTH - offset) <= ((HOP-1)*WIDTH))
        return -1;

    int index = offset;
    for(int j = 0; j < HOP; j++)
    {
        for(int i = 0; i < HOP; i++)
        {
            img_cut[j*HOP + i] = img[index+i];
        }
        index += WIDTH;
    }
    return offset+HOP;
}

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) 
{
    for(int i = 0; i < length; i++)
    {
        unsigned int value  = img_cut[offset+i];
        out_ptr[i] = (value << 16) + (value << 8) + value;
    }

    return 0;
}

void print_inference_result(ei_impulse_result_t result) {

    //Print how long it took to perform inference
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);

    // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }

    // Print the prediction results (classification)
#else
   ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }
#endif

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

}

void classifier_load_image(const uint8_t* data)
{
    img = data;
    current_index = 4;
}

int classifier_classify()
{

    float max = 0.0f;
    int inference = -1;
    
    if(current_index != -1)
    {
        current_index = cut(current_index);

        ei_impulse_result_t result = {nullptr};

        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = 36*36;
        features_signal.get_data = &raw_feature_get_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
        
        for(int i = 0; i <  EI_CLASSIFIER_LABEL_COUNT; i++)
        {
            if(result.classification[i].value > max)
            { 
                inference = i;
                max = result.classification[i].value;
            }    
        }
    }

    return inference;
}

void classifier_classify_all()
{
    int j  = 4;

    ei_printf("Edge Impulse standalone inferencing (Raspberry Pi Pico)\n");
   
    while(1)
    {
        j = cut(j);

        if (j == -1)
            break;

        ei_impulse_result_t result = {nullptr};

        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = 36*36;
        features_signal.get_data = &raw_feature_get_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);

        print_inference_result(result);
    }
}

void test()
{
   
    if(current_index != -1)
    {
        current_index = cut(current_index);
    
        for(int i = 0; i < (36*36); i++)
        {
            if(img_cut[i] == '\n')
                printf("%c", img_cut[i]+1);
            else
                printf("%c", img_cut[i]);
        }

    }

}
