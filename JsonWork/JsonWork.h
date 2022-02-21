#ifndef RSVP_JSONWORK_H
#define RSVP_JSONWORK_H
#include <jsoncpp/json/json.h>
class JsonWork {
public:
    JsonWork() = default;
    virtual ~JsonWork() = default;

    virtual void parseJsonFromFile(std::string filename) = 0;
    virtual void parseJsonFromString(std::string str) = 0;
    virtual void writeJsonToFile(std::string filename) = 0;
    virtual void updateJsonRoot() = 0;
    virtual void updateJsonStruct() = 0;
    virtual std::string getJsonString() = 0;
protected:
    Json::Value _root;
    Json::Reader _reader;
};


#endif //RSVP_JSONWORK_H
