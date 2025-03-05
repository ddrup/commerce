#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utest/using_namespace_userver.hpp>
#include <userver/utils/daemon_run.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/server/handlers/ping.hpp>

#include "cart_service/cart_handler.hpp"
#include "catalog_service/catalog_handler.hpp"
 
int main(int argc, char* argv[]) {
    auto component_list = components::MinimalServerComponentList()
                                        .Append<cart::CartHandler>()
                                        .Append<catalog::CatalogHandler>()
                                        .Append<components::Postgres>("postgres-db-1")
                                        .Append<components::TestsuiteSupport>()
                                        .Append<userver::clients::dns::Component>()
                                        .Append<userver::server::handlers::Ping>()
                                        .Append<userver::server::handlers::TestsControl>();
                                        
    return utils::DaemonMain(argc, argv, component_list);
}