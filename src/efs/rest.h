#pragma once
#include <string>
#include <map>
#include <memory>
#include "http.h"
#include "json/json.h"

namespace Rest
{
    enum class Code
    {
        success = 0,
        unknown_url = 1000,
        bad_request = 2000,
        server_error,
        server_full,
        terno_exist,
        terno_not_exist
    };

    using MsgMap = std::map<uint16_t, std::string>;

    class JsonParser
    {
    public:
        JsonParser(const std::string& body);

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

        static void registerMsg(Code&& code, const std::string& str);
        static void registerMsg(const uint16_t& code, const std::string& str);
        std::string toString();
        static MsgMap msgmap_;
    private:
        Json::Value root_;
    };

    void setHttpResponse(Http::HttpResponse& rsp, const Code& code);
}