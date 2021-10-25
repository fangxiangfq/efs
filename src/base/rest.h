#include "json.h"
#include <string>
#include <map>
#include <memory>

namespace Rest
{
    enum class Code
    {
        success = 0,
        bad_request = 1000,
        server_error,
        server_full
    };

    using MsgMap = std::map<Code, std::string>;

    class JsonParser
    {
    public:
        JsonParser(std::string& body);

        bool createParse(std::string& terno, std::string& ip, uint16_t& port);
        bool deleteParse(std::string& terno);
        bool routeParse(std::string& src, std::vector<std::string>& dst);
    private:
        bool parse_;
        Json::Value root_;
    };

    class JsonBuilder
    {
    public:
        JsonBuilder(const Code& code);
        JsonBuilder(const Code& code, const std::string& key, const uint16_t& value);
        JsonBuilder(const Code& code, const std::string& key, const std::string& str);

        std::string toString();
        static MsgMap msgmap_;
    private:
        Json::Value root_;
    };
}