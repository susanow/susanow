

#pragma once
#include <crow.h>
class ssn_vnf;
class ssn_vnf_port;
class ssn_vnf_block;
class ssn_vnf_port_patch_panel;

crow::json::wvalue responce_info(bool success, const char* msg);
crow::json::wvalue vnf_port_info(const ssn_vnf_port* port);
crow::json::wvalue vnf_block_info(const ssn_vnf_block* block);
crow::json::wvalue vnf_info(const ssn_vnf* vnf);;
crow::json::wvalue ppp_info(const ssn_vnf_port_patch_panel* ppp);

