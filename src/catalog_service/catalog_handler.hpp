#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace catalog {

class CatalogHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-catalog";

    CatalogHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;

private:
    int GetAmount(std::int32_t key, const userver::server::http::HttpRequest& request) const;
    int PostProduct(const userver::server::http::HttpRequest& request) const;
    int DeleteProduct(std::int32_t key) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // catalog