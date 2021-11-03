#include "http.h"
#include <assert.h>
#include <algorithm>

namespace Http
{
    bool HttpRequest::setMethod(const char* start, const char* end)
    {
        assert(method_ == Method::kInvalid);
        std::string m(start, end);
        if (m == "GET")
        {
            method_ = Method::kGet;
        }
        else if (m == "POST")
        {
            method_ = Method::kPost;
        }
        else if (m == "HEAD")
        {
            method_ = Method::kHead;
        }
        else if (m == "PUT")
        {
            method_ = Method::kPut;
        }
        else if (m == "DELETE")
        {
            method_ = Method::kDelete;
        }
        else
        {
            method_ = Method::kInvalid;
        }
        return method_ != Method::kInvalid;
    }

    const char* HttpRequest::methodString() const
    {
        const char* result = "UNKNOWN";
        switch(method_)
        {
        case Method::kGet:
            result = "GET";
            break;
        case Method::kPost:
            result = "POST";
            break;
        case Method::kHead:
            result = "HEAD";
            break;
        case Method::kPut:
            result = "PUT";
            break;
        case Method::kDelete:
            result = "DELETE";
            break;
        default:
            break;
        }
        return result;
    }

    void HttpRequest::addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }
        headers_[field] = value;
    }

    std::string HttpRequest::getHeader(const std::string& field) const
    {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    void HttpRequest::swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        headers_.swap(that.headers_);
    }

    void HttpResponse::appendToBuffer(Buffer::Buffer* output) const
    {
        char buf[32];
        snprintf(buf, sizeof buf, "HTTP/1.1 %d ", static_cast<int>(statusCode_));
        output->append(buf);  
        output->append(statusMessage_);
        output->append("\r\n");

        if (closeConnection_)
        {
            output->append("Connection: close\r\n");
        }
        else
        {
            snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
            output->append(buf);
            output->append("Connection: Keep-Alive\r\n");
        }

        for (const auto& header : headers_)
        {
            output->append(header.first);
            output->append(": ");
            output->append(header.second);
            output->append("\r\n");
        }

        output->append("\r\n");
        output->append(body_);
    }

    bool HttpContext::processRequestLine(const char* begin, const char* end)
    {
        bool succeed = false;
        const char* start = begin;
        const char* space = std::find(start, end, ' ');
        if (space != end && request_.setMethod(start, space))
        {
            start = space+1;
            space = std::find(start, end, ' ');
            if (space != end)
            {
                const char* question = std::find(start, space, '?');
                if (question != space)
                {
                    request_.setPath(start, question);
                    request_.setQuery(question, space);
                }
                else
                {
                    request_.setPath(start, space);
                }
                start = space+1;
                succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
                if (succeed)
                {
                    if (*(end-1) == '1')
                    {
                    request_.setVersion(HttpRequest::Version::kHttp11);
                    }
                    else if (*(end-1) == '0')
                    {
                    request_.setVersion(HttpRequest::Version::kHttp10);
                    }
                    else
                    {
                        succeed = false;
                    }
                }
            }
        }
        return succeed;
    }

    bool HttpContext::parseRequest(Buffer::Buffer* buf)
    {
        const char kCRLF[] = "\r\n";
        bool ok = true;
        bool hasMore = true;
        while (hasMore)
        {
            if (state_ == kExpectRequestLine)
            {
                const char* crlf = std::search(buf->peek(), buf->readEnd(), kCRLF, kCRLF+2);
                if (crlf != buf->readEnd())
                {
                    ok = processRequestLine(buf->peek(), crlf);
                    if (ok)
                    {
                        buf->retrieveUntil(crlf + 2);
                        state_ = kExpectHeaders;
                    }
                    else
                    {
                        hasMore = false;
                    }
                }
                else
                {
                    hasMore = false;
                }
            }
            else if (state_ == kExpectHeaders)
            {
                const char* crlf = std::search(buf->peek(), buf->readEnd(), kCRLF, kCRLF+2);
                if (crlf != buf->readEnd())
                {
                    const char* colon = std::find(buf->peek(), crlf, ':');
                    if (colon != crlf)
                    {
                        request_.addHeader(buf->peek(), colon, crlf);
                    }
                    else
                    {
                        state_ = kExpectBody;
                        hasMore = true;
                    }
                    buf->retrieveUntil(crlf + 2);
                }
                else
                {
                    hasMore = false;
                }
            }
            else if (state_ == kExpectBody)
            {
                buf->retrieveAsStringAll(request_.body());
                state_ = kGotAll;
                hasMore = false;
            }
        }
        return ok;
    }
}