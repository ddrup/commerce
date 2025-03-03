#include "hello_handler.hpp"

#include "say_hello.hpp"

namespace samples::hello {
using std::string;

string HelloHandler::HandleRequest(
    server::http::HttpRequest& request,
    server::request::RequestContext& /* request_constext */) const {
        request.GetHttpResponse().SetContentType(http::content_type::kTextPlain);
        return samples::hello::hello(request.GetArg("name"));
    }

}  // namespace samples::hello