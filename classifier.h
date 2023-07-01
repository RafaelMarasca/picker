#ifndef CLASSIFIER_H
#define CLASSIFIER_H


enum CLASS
{
    NONE = -1,
    CAP,
    POT,
    MOT,
    DIP,
    BACK
};

void classifier_classify_all();
int classifier_classify();
void classifier_load_image(const uint8_t* data);
void test();

#endif //CLASSIFIER_H