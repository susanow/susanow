
#pragma once
#include <crow.h>
class ssn_nfvi;

struct Middleware {
    struct context {};
    void before_handle(crow::request&, crow::response&, context&) {}
    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
      res.add_header("Access-Control-Allow-Origin", "*");
      res.add_header("X-Packet-Is-Everything", "true");
    }
};

/*
 * @brief REST API to control nfvi
 * @param sys ssn_nfvi pointer
 * @details
 *   User can extend this function to add new REST-API easily
 */
void rest_api_thread(ssn_nfvi* nfviptr,
    crow::App<Middleware>* app,
    uint16_t rest_server_port);

