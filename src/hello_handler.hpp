#pragma once

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include <userver/utest/using_namespace_userver.hpp>

namespace samples::hello {

class HelloHandler final : public server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-hello-sample";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequest(server::http::HttpRequest& request,
                            server::request::RequestContext&) const override;
};

}  // namespace samples::hello