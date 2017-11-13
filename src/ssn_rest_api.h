
#include <crow.h>
class ssn_nfvi;

/*
 * @brief REST API to controll nfvi
 * @param sys ssn_nfvi pointer
 * @details
 *   User can extend this function to add new REST-API easily
 */
void rest_api_thread(ssn_nfvi* nfviptr, crow::SimpleApp* app, uint16_t rest_server_port);

