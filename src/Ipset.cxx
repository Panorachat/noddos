/*
   Copyright 2017 Steven Hessing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 * Ipset.cxx
 *
 *  Created on: Aug 9, 2017
 *      Author: Steven HEssing
 */

#include "Ipset.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>


std::string getIpsetUuid (const std::string inUuid) {
    std::string res = "";
    std::stringstream ss;
    ss.str(inUuid);
    std::string item;
    uint8_t count = 0;
    while (std::getline(ss, item, '-')) {
        if (count == 0) {
            res += item;
        }
        count++;
    }
    return res;
}

std::string getIpsetName (const std::string inUuid, bool inSrc, bool inIpv4) {
    std::string res = "Nod";
    if (inSrc == true) {
        res += "src-";
    } else {
        res += "dst";
        if (inIpv4 == false) {
            res += "6-";
        } else {
            res += "4-";
        }
    }
    res += getIpsetUuid(inUuid);
    return res;
    }

bool Ipset::ipset_exec(enum ipset_cmd cmd, const boost::asio::ip::address &inIpAddress, uint32_t timeout) {

    int r = ipset_session_data_set(session, IPSET_SETNAME, ipsetName.c_str());

    const struct ipset_type *type = ipset_type_get(session, cmd);
    if (type == NULL) {
         return false;
    }

    if (inIpAddress.is_v4()) {
        uint8_t family = NFPROTO_IPV4;
        ipset_session_data_set(session, IPSET_OPT_FAMILY, &family);
        struct in_addr sin;
        inet_aton (inIpAddress.to_string().c_str(), &sin);
        ipset_session_data_set(session, IPSET_OPT_IP, &sin);
    } else {
        uint8_t family = NFPROTO_IPV6;
        ipset_session_data_set(session, IPSET_OPT_FAMILY, &family);
        unsigned char buf[sizeof(struct in6_addr)];
        int s = inet_pton(AF_INET6, inIpAddress.to_string().c_str(), buf);
        ipset_session_data_set(session, IPSET_OPT_IP, &buf);
    }
    if (timeout) {
        ipset_session_data_set(session, IPSET_OPT_TIMEOUT, &timeout);
    }
    r = ipset_cmd(session, cmd, 0);

//    r = ipset_commit(session);
//    ipset_data_reset(session->data);
    return r == 0;
}

bool Ipset::ipset_exec(enum ipset_cmd cmd, const MacAddress &Mac, uint32_t timeout) {
    int r;
    r = ipset_session_data_set(session, IPSET_SETNAME, ipsetName.c_str());

    const struct ipset_type *type = ipset_type_get(session, cmd);
    if (type == NULL) {
         return false;
    }
    // ipset_parse_ether(session, IPSET_OPT_ETHER, Mac.c_str());
    // ipset_session_data_set(session, IPSET_OPT_ETHER, Mac.c_str());
    r = ipset_parse_elem(session, (ipset_opt)type->last_elem_optional, Mac.c_str());
    if (r < 0) {
        std::string e = "Can't call ipset_parse_elem, error: ";
        e.append(ipset_session_error(session));
        ipset_session_fini(session);
        throw std::runtime_error(e.c_str());
        return false;
    } else {
        if (timeout) {
            r = ipset_session_data_set(session, IPSET_OPT_TIMEOUT, &timeout);
            if (r != 0) {
                std::string e = "Can't set timeout for " + ipsetName + ", error: " + ipset_session_error(session);
                ipset_session_fini(session);
                throw std::runtime_error(e.c_str());
            }
        }
        r = ipset_cmd(session, cmd, 0);
        if (r != 0) {
            std::string e = "Can't call ipset_cmd, error: ";
            e.append(ipset_session_error(session));
            ipset_session_fini(session);
            throw std::runtime_error(e.c_str());
            return false;
        }
    }
//    r = ipset_commit(session);
//    ipset_data_reset(session->data);

    return r == 0;
}


