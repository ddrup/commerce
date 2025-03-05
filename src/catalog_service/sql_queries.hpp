#pragma once

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/query.hpp>

namespace sql {

const userver::storages::postgres::Query kCreateTable{
    "CREATE TABLE IF NOT EXISTS products("
        "product_id SERIAL PRIMARY KEY,"
        "name TEXT UNIQUE,"
        "amount INTEGER"
    ");",
    userver::storages::postgres::Query::Name{"sample_create_table"},
};

const userver::storages::postgres::Query kSelectValue{
    "SELECT amount FROM products WHERE product_id=$1",
    userver::storages::postgres::Query::Name{"sample_select_value"},
};

const userver::storages::postgres::Query kInsertValue{
    "INSERT INTO products(name, amount)"
    "VALUES($1, $2)"
    "ON CONFLICT (name) DO NOTHING",
    userver::storages::postgres::Query::Name{"sample_insert_value"},
};

const userver::storages::postgres::Query kDeleteTable{
    "DELETE FROM products WHERE product_id=$1",
    userver::storages::postgres::Query::Name{"sample_delete_value"},
};

}  // namespace sql