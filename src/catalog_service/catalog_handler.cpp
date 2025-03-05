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
  const auto& product_id = request.GetArg("product_id");
  if (product_id.empty()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "No 'product_id' query argument"});
  }

  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kTextPlain);
  switch (request.GetMethod()) {
    case userver::server::http::HttpMethod::kGet:
      return std::to_string(GetValue(product_id, request));
    case userver::server::http::HttpMethod::kPost:
      return std::to_string(PostValue(product_id, request));
    case userver::server::http::HttpMethod::kDelete:
      return std::to_string(DeleteValue(product_id));
    default:
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
  }
}

int CatalogHandler::GetValue(
    std::string_view key,
    const userver::server::http::HttpRequest& request) const {
  userver::storages::postgres::ResultSet res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           sql::kSelectValue, key);

  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return {};
  }

  return res.AsSingleRow<int>();
}

int CatalogHandler::PostValue(
    std::string_view key,
    const userver::server::http::HttpRequest& request) const {
  std::string_view name = request.GetArg("name");
  std::string_view amount_arg = request.GetArg("amount");
  std::string_view amount = amount_arg.empty() ? "0" : amount_arg;
  if (name.empty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
      "sample_transaction_insert_key_value",
      userver::storages::postgres::ClusterHostType::kMaster, {});

  auto res = transaction.Execute(sql::kInsertValue, name, amount);
  if (res.RowsAffected()) {
    transaction.Commit();
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return res.AsSingleRow<int>();
  }

  res = transaction.Execute(sql::kSelectValue, key);
  transaction.Rollback();

  auto result = res.AsSingleRow<int>();
  if (std::to_string(result) != amount) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
  }

  return result;
}

int CatalogHandler::DeleteValue(std::string_view key) const {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, sql::kDeleteTable,
      key);

  return res.RowsAffected();
}

}  // namespace catalog