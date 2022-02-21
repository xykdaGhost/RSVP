#include "ParamManage.h"
#include <fstream>
#include <iostream>
/**
 * @brief Constructor of ParamManage. It will read parameter from file
 * @param fileName : the Json file path and name.
 */
ParamManage::ParamManage(const std::string& filename) {
    //open the setting file and parse it
    parseJsonFromFile(filename);

    //set the model with json root
    _parameterModel = new ParameterModel(_root);
}

/**
 * @brief Parse Json from file
 * @param filename : file name to read
 */
void ParamManage::parseJsonFromFile(std::string filename) {
    std::ifstream ifs;
    ifs.open(filename);
    _reader.parse(ifs, _root, false);
    ifs.close();
}

/**
 * @brief Parse Json from a string
 * @param str : string to read
 */
void ParamManage::parseJsonFromString(std::string str) {
    _reader.parse(str, _root);
}

/**
 * @brief Write Json to file
 * @param filename : file to write
 */
void ParamManage::writeJsonToFile(std::string filename) {
    std::ofstream ofs;
    ofs.open(filename);
    ofs << _root.toStyledString();
    ofs.close();
}

/**
 * @brief Update the json root
 */
void ParamManage::updateJsonRoot() {
    _parameterModel->setupJsonData(_root);
}

/**
 * @brief Update the json struct we defined, could be model or struct
 */
void ParamManage::updateJsonStruct() {
    _parameterModel->setupModelData(_root);
}

/**
 * @brief Output Json to styled string
 * @return the string
 */
std::string ParamManage::getJsonString() {
    return _root.toStyledString();
}
