#include "cart_handler.hpp"

namespace cart {

std::string CartHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /* request_context */) const {
        auto product_id = request.GetArg("product_id");
        if(product_id.empty()) {
            // throw userver::server::handlers::ClientError("No product_id provided");
        }

        return "success";
    }

}  // namespace cart