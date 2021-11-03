#include "rest.h"

namespace Rest
{
    MsgMap JsonBuilder::msgmap_;

    void setHttpResponse(Http::HttpResponse& rsp, const Code& code)
    {
        if(Code::success == code)
        {
            rsp.setStatusCode(Http::HttpResponse::HttpStatusCode::k200Ok);
            rsp.setStatusMessage("OK");
        }
        else if (Code::bad_request == code)
        {
            rsp.setStatusCode(Http::HttpResponse::HttpStatusCode::k400BadRequest);
            rsp.setStatusMessage("ERR");
        }
        else if (Code::unknown_url == code)
        {
            rsp.setStatusCode(Http::HttpResponse::HttpStatusCode::k301MovedPermanently);
            rsp.setStatusMessage("ERR");
        }
        else
        {
            rsp.setStatusCode(Http::HttpResponse::HttpStatusCode::k500ServErr);
            rsp.setStatusMessage("ERR");
        }

        rsp.setContentType("application/json");
        rsp.addHeader("Server", "Efs");
    }
    
    JsonParser::JsonParser(const std::string& body) 
    {
        Json::Reader reader;
        parse_ = reader.parse(body, root_);
    }
    
    bool JsonParser::createParse(std::string& terno, std::string& ip, uint16_t& port) 
    {
        if(!deleteParse(terno))
            return false;
        Json::Value tmp = root_["ip"];
        if(!tmp || !tmp.isString())
        {
            return false;
        }

        ip = tmp.asString();
        tmp = Json::Value::null;
        tmp = root_["port"];
        if(!tmp || !tmp.isUInt())
        {
            return false;
        }

        port = static_cast<uint16_t>(tmp.asUInt());

        return true;
    }
    
    bool JsonParser::deleteParse(std::string& terno) 
    {
        if(!parse_)
            return parse_;
        Json::Value tmp = root_["terno"];
        if(!tmp || !tmp.isString())
        {
            return false;
        }
        else
        {
            terno = tmp.asString();
            return true;
        }
    }
    
    bool JsonParser::routeParse(std::string& src, std::vector<std::string>& dst) 
    {
        if(!deleteParse(src))
            return false;
        Json::Value arr = root_["dst"];
        if(!arr || !arr.isArray())
        {
            return false;
        }
        else
        {
            dst.clear();
            for(unsigned int i = 0 ; i < arr.size(); ++i)
            {
                if(!arr[i].isString())
                    return false;
                dst.push_back(arr[i].asString());
            }
            return true;
        }
    }
    
    JsonBuilder::JsonBuilder(const Code& code) 
    {
        uint16_t rspCode = static_cast<uint16_t>(code);
        root_["code"] = rspCode;
        root_["msg"] = msgmap_[rspCode];
    }
    
    //todo change to templates
    JsonBuilder::JsonBuilder(const Code& code, const std::string& key, const uint16_t& value) 
    {
        uint16_t rspCode = static_cast<uint16_t>(code);
        root_["code"] = rspCode;
        root_["msg"] = msgmap_[rspCode];

        Json::Value data;
        data[key] = value;

        root_["data"] = data;
    }
    
    JsonBuilder::JsonBuilder(const Code& code, const std::string& key, const std::string& str) 
    {
        uint16_t rspCode = static_cast<uint16_t>(code);
        root_["code"] = rspCode;
        root_["msg"] = msgmap_[rspCode];
        Json::Value data;
        data[key] = str;

        root_["data"] = data;
    }
    
    void JsonBuilder::registerMsg(Code&& code, const std::string& str) 
    {
        uint16_t rspCode = static_cast<uint16_t>(code);
        registerMsg(rspCode, str);
    }
    
    void JsonBuilder::registerMsg(const uint16_t& code, const std::string& str) 
    {
        msgmap_.emplace(code, str);
    }
    
    std::string JsonBuilder::toString() 
    {
        Json::FastWriter writer;
        return writer.write(root_);
    }
}