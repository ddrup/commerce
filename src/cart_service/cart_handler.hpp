#pragma once

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

namespace cart {

class CartHandler final : public userver::server::handlers::HttpHandlerBase {
public:

    static constexpr std::string_view kName = "handler-cart";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;

};

}  // namespace cart