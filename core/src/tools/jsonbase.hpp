#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

class JSONBase
{

public:
    virtual std::string Serialize() const = 0;
    virtual bool Deserialize(const std::string& s) = 0;
};