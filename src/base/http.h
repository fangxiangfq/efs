#pragma once
#include <map>
#include <string>
#include "buffer.h"

namespace Http
{
    class HttpRequest
    {
    public:
        enum class Method
        {
            kInvalid, kGet, kPost, kHead, kPut, kDelete
        };
        enum class Version
        {
            kUnknown, kHttp10, kHttp11, kHttp20
        };

        void setVersion(Version v) { version_ = v; }
        Version getVersion() const { return version_; }
        bool setMethod(const char* start, const char* end);
        Method method() const { return method_; }
        const char* methodString() const;
        void setPath(const char* start, const char* end){path_.assign(start, end);}
        const std::string& path() const{ return path_; }
        void setQuery(const char* start, const char* end){ query_.assign(start, end);}
        const std::string& body() const { return body_; }
        std::string& body() { return body_; }
        const std::string& query() const { return query_; }
        const std::map<std::string, std::string>& headers() const { return headers_; }

        void addHeader(const char* start, const char* colon, const char* end);
        std::string getHeader(const std::string& field) const;
        void swap(HttpRequest& that);

    private:
        Method method_;
        Version version_;
        std::string path_;
        std::string query_;
        std::string body_;
        std::map<std::string, std::string> headers_;
        //time
    };

    class HttpResponse
    {
    public:
        enum class HttpStatusCode
        {
            kUnknown,
            k200Ok = 200,
            k301MovedPermanently = 301,
            k400BadRequest = 400,
            k404NotFound = 404,
            k500ServErr = 500,
        };

        explicit HttpResponse(bool close)
            : statusCode_(HttpStatusCode::kUnknown),
            closeConnection_(close){}

        void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
        void setStatusMessage(const std::string& message) { statusMessage_ = message; }
        void setCloseConnection(bool on) { closeConnection_ = on; }
        bool closeConnection() const { return closeConnection_; }
        void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }
        void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }
        void setBody(const std::string& body) { body_ = body; }
        void appendToBuffer(Buffer::Buffer* output) const;
    private:
        std::map<std::string, std::string> headers_;
        HttpStatusCode statusCode_;
        std::string statusMessage_;
        bool closeConnection_;
        std::string body_;
    };

    class HttpContext
    {
    public:
        enum HttpRequestParseState
        {
            kExpectRequestLine,
            kExpectHeaders,
            kExpectBody,
            kGotAll,
        };

        HttpContext() :state_(kExpectRequestLine){}
        bool parseRequest(Buffer::Buffer* buf);
        bool gotAll() const { return state_ == kGotAll; }

        void reset()
        {
            state_ = kExpectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        const HttpRequest& request() const
        { return request_; }

        HttpRequest& request()
        { return request_; }

    private:
        bool processRequestLine(const char* begin, const char* end);
        HttpRequestParseState state_;
        HttpRequest request_;
    };
}