#include "../chimera.hpp"
#include "../halo_data/map.hpp"
#include "../halo_data/server.hpp"
#include "../halo_data/player.hpp"

#include <httplib.h>
#include <thread>
#include <sstream>
#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
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

    //srv.Get("/players", [](const Request &req, Response &res) {
    //  res.set_content("{\"players\":[]}", "application/json");
    //});

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

    srv.Get("/players", [](const Request &req, Response &res) {
      auto info = ServerInfoPlayerList::get_server_info_player_list();
      std::ostringstream os;
      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      os << "{\"players\":[";
      if ( info != nullptr ) {
        struct Player {
          std::string name;
          int id;
          int team;

          //friend std::ostream &operator<<(std::ostream &os, const Player &p) {
          //  os <<  "{"
          //  <<    "\"name\":\"" << p.name << "\","
          //  <<    "\"id\":" << p.id << ","
          //  <<    "\"team\":" << p.team
          //  <<  "}";
          //  return os;
          //}
          
        };
        std::vector<Player> players;
        for ( int i = 0; i < 16; i++ ) {
          const auto &player = info->players[i];
          if ( player.status == 0xFF ) {
            continue;
          }
          players.push_back(Player{
            .name = converter.to_bytes(player.name),
            .id = (int)player.player_id,
            .team = (int)player.team
          });
         
          if ( i < info->max_players -1 )
            os << ","; 
        }
        for ( int i = 0, I = players.size();i < I; i++ ) {
          os  <<  "{"
              <<    "\"name\":\"" << players[i].name << "\","
              <<    "\"id\":" << players[i].id << ","
              <<    "\"team\":" << players[i].team
              <<  "}";
          if ( i < info->max_players -1 )
            os << ","; 
        }

      }
      os << "]}";
      res.set_content(os.str(),"application/json");
    });

    srv.listen("0.0.0.0",8777);

  }

  
} // namespace Chimera
