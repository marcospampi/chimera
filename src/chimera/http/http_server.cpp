#include "../chimera.hpp"
#include "../halo_data/map.hpp"

#include <httplib.h>
#include <thread>
#include <sstream>
#include <iostream>
namespace Chimera
{
  void server();
  std::thread *http_server_thread;
  int set_up_http_server() {
    http_server_thread = new std::thread(&server);
    return 1;
  }
  int shutdown_http_server() {
    if( http_server_thread ) {
      http_server_thread->detach();

      delete http_server_thread;
      http_server_thread = nullptr;
    }
  }

  void server() {
    using namespace httplib;
    httplib::Server srv;

    srv.Get("/players", [](const Request &req, Response &res) {
      res.set_content("{\"players\":[]}", "application/json");
    });

    srv.Get("/maps", [](const Request &req, Response &res) {
      const auto &map_list = get_map_list();
      const int count = map_list.map_count;
      const auto *list = static_cast<MapIndexCustomEdition*>(map_list.map_list);
      std::ostringstream out_s;
      out_s << "{\"maps\":[";
      if (  count > 0) {
        for ( int i = 0, I = count; i < I; i++) {
          if ( list[i].file_name ){
            out_s << "\"" << list[i].file_name << "\"";  
            if ( i < I - 1 )
              out_s << ",";
          }
        }
      }
      out_s << "]}";
      
      res.set_content(out_s.str(), "application/json");

    });

    srv.listen("0.0.0.0",8777);

  }

  
} // namespace Chimera
