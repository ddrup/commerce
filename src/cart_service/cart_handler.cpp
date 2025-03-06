#include "cart_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace cart {

CartHandler::CartHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      http_client_(
          context.FindComponent<userver::components::HttpClient>("http-client")
              .GetHttpClient()) {}

std::string CartHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /* request_context */) const {
  auto product_id = request.GetArg("product_id");
  if (product_id.empty()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{"No product_id provided"});
  }

  std::string url = "http://localhost:8080/v1/product?product_id=" + product_id;
  auto response = http_client_.CreateRequest()
                      .get(url)
                      .timeout(std::chrono::seconds(1))
                      .perform();
  if (!response->IsOk()) {
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{"Catalog service unavailable"});
  }

  std::string amount = response->body();
  if (std::stoi(amount) <= 0) {
    throw userver::server::handlers::ResourceNotFound(
        userver::server::handlers::ExternalBody{
            "Product with given ID not found"});
  }

  // Next step: pay things

  return amount + "\n";
}

}  // namespace cart