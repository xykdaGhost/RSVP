#ifndef PARAMETERMODEL_H
#define PARAMETERMODEL_H

#include "TreeModel.h"


/**
 * @brief The parameter model in a tree.
 */
class ParameterModel : public TreeModel
{
public:
    TreeItem* getRootItem();
    ParameterModel(const Json::Value root, QObject *parent = nullptr);
    ~ParameterModel() override;

    void setupModelData(const Json::Value root) override;
    void setupJsonData(Json::Value& root);
    void setupJsonStruct();


    struct Parameter {
            struct Aec {
                double expTime;
                int mannualExpTime;
                int mannualGain;
                int speed;

            } aec;
            struct Camera {
                int gain;
                int height;
                int offsetX;
                int offsetY;
                std::string path;
                int width;
            } camera;
            struct Capture {
                int interval;
                bool trigger;
                std::string savePath;
                bool saveRaw;
                int saveRawInterval;

            } capture;
            struct Alg {
                bool hdr;
                bool yolo;
                bool autoExpo;
            } alg;
        } _parameter;

    struct Parameter& paramStruct() {return _parameter;}
};

#endif // PARAMETERMODEL_H
