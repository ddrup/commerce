#include "catalog_handler.hpp"
#include "sql_queries.hpp"

namespace catalog {

CatalogHandler::CatalogHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1")
              .GetCluster()) {
  pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       sql::kCreateTable);
}

std::string CatalogHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /* request_context */) const {
  auto product_id_arg = request.GetArg("product_id");
  if (product_id_arg.empty()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "No 'product_id' query argument"});
  }

  auto product_id = std::stoi(product_id_arg);

  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kTextPlain);
  switch (request.GetMethod()) {
    case userver::server::http::HttpMethod::kGet:
      return std::to_string(GetAmount(product_id, request));
    case userver::server::http::HttpMethod::kPost:
      return std::to_string(PostProduct(request));
    case userver::server::http::HttpMethod::kDelete:
      return std::to_string(DeleteProduct(product_id));
    default:
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
  }
}

int CatalogHandler::GetAmount(
    std::int32_t key,
    const userver::server::http::HttpRequest& request) const {
  userver::storages::postgres::ResultSet res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           sql::kSelectValue, key);

  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return {};
  }

  return res.AsSingleRow<std::int32_t>();
}

int CatalogHandler::PostProduct(
    const userver::server::http::HttpRequest& request) const {
  std::string_view name = request.GetArg("name");
  auto amount = request.GetArg("amount");

  if(amount.empty()) amount = "0";

  if (name.empty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert_key_value",
      userver::storages::postgres::ClusterHostType::kMaster, {});

  auto res = transaction.Execute(sql::kInsertValue, name, std::stoi(amount));
  if (res.RowsAffected()) {
    transaction.Commit();
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return std::stoi(amount);
  }

  res = transaction.Execute("SELECT amount FROM products WHERE name=$1", name);
  transaction.Rollback();

  return res.AsSingleRow<int>();
}

int CatalogHandler::DeleteProduct(std::int32_t key) const {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, sql::kDeleteTable,
      key);

  return res.RowsAffected();
}

}  // namespace catalog