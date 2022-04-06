#include "ParameterModel.h"
#include "../JsonWork/ParamManage.h"
/**
 * @brief Constructor of ParameterModel, set the parameter tree
 * @param root : the json root
 * @param parent : the parent object
 */
ParameterModel::ParameterModel(const Json::Value root, QObject *parent) :
    TreeModel(parent)
{    
    rootItem = new TreeItem({"参数", ""});

    TreeItem* aec = new TreeItem({"曝光参数", ""}, rootItem);
    TreeItem* camera = new TreeItem({"相机参数", ""}, rootItem);
    TreeItem* capture = new TreeItem({"捕获参数", ""}, rootItem);
    TreeItem* alg = new TreeItem({"算法参数", ""}, rootItem);
    rootItem->appendChild(aec);
    rootItem->appendChild(camera);
    rootItem->appendChild(capture);
    rootItem->appendChild(alg);

    TreeItem* expTime_a = new TreeItem({"-曝光系数Ａ:", root["aec"]["expTime_a"].asInt()}, aec);
    TreeItem* expTime_b = new TreeItem({"-曝光系数B:", root["aec"]["expTime_b"].asInt()}, aec);
    TreeItem* maxGain = new TreeItem({"-最大增益:", root["aec"]["maxGain"].asInt()}, aec);
    TreeItem* minGain = new TreeItem({"-最小增益:", root["aec"]["minGain"].asInt()}, aec);
    TreeItem* speed = new TreeItem({"-速度(km/h):", root["aec"]["speed"].asInt()}, aec);
    TreeItem* minExpTime = new TreeItem({"-最小曝光时间(us):", root["aec"]["minExpTime"].asInt()}, aec);
    aec->appendChild(expTime_a);
    aec->appendChild(expTime_b);
    aec->appendChild(maxGain);
    aec->appendChild(minGain);
    aec->appendChild(speed);
    aec->appendChild(minExpTime);

    TreeItem* gain = new TreeItem({"-增益:", root["camera"]["gain"].asInt()}, camera);
    TreeItem* width = new TreeItem({"-宽(pixs):", root["camera"]["width"].asInt()}, camera);
    TreeItem* height = new TreeItem({"-高(pixs):", root["camera"]["height"].asInt()}, camera);
    TreeItem* offsetX = new TreeItem({"-偏移Ｘ(pixs):", root["camera"]["offsetX"].asInt()}, camera);
    TreeItem* offsetY = new TreeItem({"-偏移Ｙ(pixs):", root["camera"]["offsetY"].asInt()}, camera);
    TreeItem* path = new TreeItem({"-图像来源:", QString::fromStdString(root["camera"]["path"].asString())}, camera);
    camera->appendChild(gain);
    camera->appendChild(width);
    camera->appendChild(height);
    camera->appendChild(offsetX);
    camera->appendChild(offsetY);
    camera->appendChild(path);

    TreeItem* interval = new TreeItem({"-拍摄间隔(ms):", root["capture"]["interval"].asInt()}, capture);
    TreeItem* trigger = new TreeItem({"-硬件触发:", root["capture"]["trigger"].asBool()}, capture);
    TreeItem* savePath = new TreeItem({"-存储地址:", QString::fromStdString(root["capture"]["savePath"].asString())}, capture);
    TreeItem* saveRaw = new TreeItem({"-存储原图:", root["capture"]["saveRaw"].asBool()}, capture);
    TreeItem* saveRawInterval = new TreeItem({"-原图间隔(ms):", root["capture"]["saveRawInterval"].asInt()}, capture);
    TreeItem* saveResInterval = new TreeItem({"-结果间隔(ms):", root["capture"]["saveResInterval"].asInt()}, capture);
    TreeItem* saveResult = new TreeItem({"-存储结果P:", root["capture"]["saveResult"].asBool()}, capture);
    capture->appendChild(interval);
    capture->appendChild(trigger);
    capture->appendChild(savePath);
    capture->appendChild(saveRaw);
    capture->appendChild(saveRawInterval);
    capture->appendChild(saveResInterval);
    capture->appendChild(saveResult);

    TreeItem* hdr = new TreeItem({"-HDR:", root["alg"]["hdr"].asBool()}, alg);
    TreeItem* yolo = new TreeItem({"-YOLO:", root["alg"]["yolo"].asBool()}, alg);
    TreeItem* autoExpo = new TreeItem({"-自动曝光:", root["alg"]["autoExpo"].asBool()}, alg);

    alg->appendChild(hdr);
    alg->appendChild(yolo);
    alg->appendChild(autoExpo);

    //setup the struct
    setupJsonStruct();
}

ParameterModel::~ParameterModel() {

}

/**
 * @brief Setup model data from json root
 * @param root : json root
 */
void ParameterModel::setupModelData(const Json::Value root) {
    rootItem->child(0)->child(0)->setData(root["aec"]["expTime_a"].asInt(), 1);
    rootItem->child(0)->child(1)->setData(root["aec"]["expTime_b"].asInt(), 1);
    rootItem->child(0)->child(2)->setData(root["aec"]["maxGain"].asInt(), 1);
    rootItem->child(0)->child(3)->setData(root["aec"]["minGain"].asInt(), 1);
    rootItem->child(0)->child(4)->setData(root["aec"]["speed"].asInt(), 1);
    rootItem->child(0)->child(5)->setData(root["aec"]["minExpTime"].asInt(), 1);

    rootItem->child(1)->child(0)->setData(root["camera"]["gain"].asInt(), 1);
    rootItem->child(1)->child(1)->setData(root["camera"]["width"].asInt(), 1);
    rootItem->child(1)->child(2)->setData(root["camera"]["height"].asInt(), 1);
    rootItem->child(1)->child(3)->setData(root["camera"]["offsetX"].asInt(), 1);
    rootItem->child(1)->child(4)->setData(root["camera"]["offsetY"].asInt(), 1);
    rootItem->child(1)->child(5)->setData(QString::fromStdString(root["camera"]["path"].asString()), 1);

    rootItem->child(2)->child(0)->setData(root["capture"]["interval"].asInt(), 1);
    rootItem->child(2)->child(1)->setData(root["capture"]["trigger"].asBool(), 1);
    rootItem->child(2)->child(2)->setData(QString::fromStdString(root["capture"]["savePath"].asString()), 1);
    rootItem->child(2)->child(3)->setData(root["capture"]["saveRaw"].asBool(), 1);
    rootItem->child(2)->child(4)->setData(root["capture"]["saveRawInterval"].asInt(), 1);
    rootItem->child(2)->child(5)->setData(root["capture"]["saveResInterval"].asInt(), 1);
    rootItem->child(2)->child(6)->setData(root["capture"]["saveResult"].asBool(), 1);

    rootItem->child(3)->child(0)->setData(root["alg"]["hdr"].asBool(), 1);
    rootItem->child(3)->child(1)->setData(root["alg"]["yolo"].asBool(), 1);
    rootItem->child(3)->child(2)->setData(root["alg"]["autoExpo"].asBool(), 1);
}

/**
 * @brief Setup json root from model
 * @param root : json root
 */
void ParameterModel::setupJsonData(Json::Value& root) {
    root["aec"]["expTime_a"] = rootItem->child(0)->child(0)->data(1).toInt();
    root["aec"]["expTime_b"] = rootItem->child(0)->child(1)->data(1).toInt();
    root["aec"]["maxGain"] = rootItem->child(0)->child(2)->data(1).toInt();
    root["aec"]["minGain"] = rootItem->child(0)->child(3)->data(1).toInt();
    root["aec"]["speed"] = rootItem->child(0)->child(0)->data(4).toInt();
    root["aec"]["minExpTime"] = rootItem->child(0)->child(5)->data(1).toInt();

    root["camera"]["gain"] = rootItem->child(1)->child(0)->data(1).toInt();
    root["camera"]["width"] = rootItem->child(1)->child(1)->data(1).toInt();
    root["camera"]["height"] = rootItem->child(1)->child(2)->data(1).toInt();
    root["camera"]["offsetX"] = rootItem->child(1)->child(3)->data(1).toInt();
    root["camera"]["offsetY"] = rootItem->child(1)->child(4)->data(1).toInt();
    root["camera"]["path"] = rootItem->child(1)->child(5)->data(1).toString().toStdString();

    root["capture"]["interval"] = rootItem->child(2)->child(0)->data(1).toInt();
    root["capture"]["trigger"] = rootItem->child(2)->child(1)->data(1).toBool();
    root["capture"]["savePath"] = rootItem->child(2)->child(2)->data(1).toString().toStdString();
    root["capture"]["saveRaw"] = rootItem->child(2)->child(3)->data(1).toBool();
    root["capture"]["saveRawInterval"] = rootItem->child(2)->child(4)->data(1).toInt();
    root["capture"]["saveResInterval"] = rootItem->child(2)->child(5)->data(1).toInt();
    root["capture"]["saveResult"] = rootItem->child(2)->child(6)->data(1).toBool();

    root["alg"]["hdr"] = rootItem->child(3)->child(0)->data(1).toBool();
    root["alg"]["yolo"] = rootItem->child(3)->child(1)->data(1).toBool();
    root["alg"]["autoExpo"] = rootItem->child(3)->child(2)->data(1).toBool();

    setupJsonStruct();
}

/**
 * @brief Setup json struct for check
 */
void ParameterModel::setupJsonStruct() {
    _parameter.aec.expTime_a = rootItem->child(0)->child(0)->data(1).toInt();
    _parameter.aec.expTime_b = rootItem->child(0)->child(1)->data(1).toInt();
    _parameter.aec.maxGain = rootItem->child(0)->child(2)->data(1).toInt();
    _parameter.aec.minGain = rootItem->child(0)->child(3)->data(1).toInt();
    _parameter.aec.speed = rootItem->child(0)->child(4)->data(1).toInt();
    _parameter.aec.minExpTime = rootItem->child(0)->child(5)->data(1).toInt();

    _parameter.camera.gain = rootItem->child(1)->child(0)->data(1).toInt();
    _parameter.camera.width = rootItem->child(1)->child(1)->data(1).toInt();
    _parameter.camera.height = rootItem->child(1)->child(2)->data(1).toInt();
    _parameter.camera.offsetX = rootItem->child(1)->child(3)->data(1).toInt();
    _parameter.camera.offsetY = rootItem->child(1)->child(4)->data(1).toInt();
    _parameter.camera.path = rootItem->child(1)->child(5)->data(1).toString().toStdString();

    _parameter.capture.interval = rootItem->child(2)->child(0)->data(1).toInt();
    _parameter.capture.trigger = rootItem->child(2)->child(1)->data(1).toBool();
    _parameter.capture.savePath = rootItem->child(2)->child(2)->data(1).toString().toStdString();
    _parameter.capture.saveRaw = rootItem->child(2)->child(3)->data(1).toBool();
    _parameter.capture.saveRawInterval = rootItem->child(2)->child(4)->data(1).toInt();
    _parameter.capture.saveResInterval = rootItem->child(2)->child(5)->data(1).toInt();
    _parameter.capture.saveResult = rootItem->child(2)->child(6)->data(1).toBool();
}

TreeItem* ParameterModel::getRootItem() {
    return rootItem;
}
