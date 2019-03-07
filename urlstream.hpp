// -*- C++ -*-
/* 
 * File:   urlstream.hpp
 * Author: matthewsupernaw
 *
 * Created on August 8, 2015, 2:54 PM
 */

#ifndef URLSTREAM_HPP
#define	URLSTREAM_HPP

#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <vector>
#include <map>
#include <unistd.h>


#include <openssl/ssl.h>
#include <openssl/err.h>


namespace string_util {

    /**
     * Trim the left side.
     * 
     * @param s
     * @return 
     */
    static inline const std::string left_trim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    /**
     * Trim the right side.
     * 
     * @param s
     * @return 
     */
    static inline const std::string right_trim(std::string s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    /**
     * Trim both sides.
     * 
     * @param s
     * @return 
     */
    static inline const std::string trim(std::string s) {
        return left_trim(right_trim(s));
    }

    static const std::vector<std::string> tokenize(const std::string& str,
            const std::string& delimiters = " ", const bool trimEmpty = true) {
        std::vector<std::string> tokens;
        std::string::size_type pos, lastPos = 0;
        while (true) {
            pos = str.find_first_of(delimiters, lastPos);
            if (pos == std::string::npos) {
                pos = str.length();

                if (pos != lastPos || !trimEmpty)
                    tokens.push_back(std::vector<std::string>::value_type(str.data() + lastPos,
                        (std::vector<std::string>::value_type::size_type)pos - lastPos));

                break;
            } else {
                if (pos != lastPos || !trimEmpty)
                    tokens.push_back(std::vector<std::string>::value_type(str.data() + lastPos,
                        (std::vector<std::string>::value_type::size_type)pos - lastPos));
            }

            lastPos = pos + 1;
        }
        return tokens;
    }

    template <typename T>
    T to_number(const std::string &Text) {
        std::istringstream ss(Text);
        T result;
        return (ss >> result) ? result : 0;
    }
}

const char HEX2DEC[256] = {
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 1 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 2 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 3 */ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

    /* 4 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 5 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 6 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 7 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 8 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 9 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* A */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* B */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* C */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* D */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* E */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* F */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


// Only alphanum is safe.
const char SAFE[256] = {
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

    /* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    /* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

    /* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

class http_request {
public:

    http_request()
    : method_(std::string("undefined")),
    path_(std::string("undefined")),
    version_(std::string("undefined")),
    accept_(std::string("undefined")),
    accept_charset_(std::string("undefined")),
    accept_encoding_(std::string("undefined")),
    accept_language_(std::string("undefined")),
    accept_datetime_(std::string("undefined")),
    authorization_(std::string("undefined")),
    cache_control_(std::string("undefined")),
    connection_(std::string("undefined")),
    cookie_(std::string("undefined")),
    content_length_(std::string("undefined")),
    content_md5_(std::string("undefined")),
    content_type_(std::string("undefined")),
    date_(std::string("undefined")),
    expect_(std::string("undefined")),
    from_(std::string("undefined")),
    host_(std::string("undefined")),
    if_match_(std::string("undefined")),
    if_modified_since_(std::string("undefined")),
    if_none_match_(std::string("undefined")),
    if_range_(std::string("undefined")),
    if_unmodified_since_(std::string("undefined")),
    max_forwards_(std::string("undefined")),
    pragma_(std::string("undefined")),
    proxy_authorization_(std::string("undefined")),
    range_(std::string("undefined")),
    referer_(std::string("undefined")),
    te_(std::string("undefined")),
    upgrade_(std::string("undefined")),
    user_agent_(std::string("undefined")),
    via_(std::string("undefined")),
    warning_(std::string("undefined")),
    x_requested_with_(std::string("undefined")),
    dnt_(std::string("undefined")),
    x_forwarded_for_(std::string("undefined")),
    x_forwarded_proto_(std::string("undefined")),
    front_end_https_(std::string("undefined")),
    x_att_deviceid_(std::string("undefined")),
    x_wap_profile_(std::string("undefined")),
    proxy_connection_(std::string("undefined")),
    body_(std::string("undefined")) {
    }

    http_request(const http_request& orig) {
        this->method_ = orig.method_;
        this->path_ = orig.path_;
        this->version_ = orig.version_;
        this->accept_ = orig.accept_;
        this->accept_charset_ = orig.accept_charset_;
        this->accept_encoding_ = orig.accept_encoding_;
        this->accept_language_ = orig.accept_language_;
        this->accept_datetime_ = orig.accept_datetime_;
        this->authorization_ = orig.authorization_;
        this->cache_control_ = orig.cache_control_;
        this->connection_ = orig.connection_;
        this->cookie_ = orig.cookie_;
        this->content_length_ = orig.content_length_;
        this->content_md5_ = orig.content_md5_;
        this->content_type_ = orig.content_type_;
        this->date_ = orig.date_;
        this->expect_ = orig.expect_;
        this->from_ = orig.from_;
        this->host_ = orig.host_;
        this->if_match_ = orig.if_match_;
        this->if_modified_since_ = orig.if_modified_since_;
        this->if_none_match_ = orig.if_none_match_;
        this->if_range_ = orig.if_range_;
        this->if_unmodified_since_ = orig.if_unmodified_since_;
        this->max_forwards_ = orig.max_forwards_;
        this->pragma_ = orig.pragma_;
        this->proxy_authorization_ = orig.proxy_authorization_;
        this->range_ = orig.range_;
        this->referer_ = orig.referer_;
        this->te_ = orig.te_;
        this->upgrade_ = orig.upgrade_;
        this->user_agent_ = orig.user_agent_;
        this->via_ = orig.via_;
        this->warning_ = orig.warning_;
        this->x_requested_with_ = orig.x_requested_with_;
        this->dnt_ = orig.dnt_;
        this->x_forwarded_for_ = orig.x_forwarded_for_;
        this->x_forwarded_proto_ = orig.x_forwarded_proto_;
        this->front_end_https_ = orig.front_end_https_;
        this->x_att_deviceid_ = orig.x_att_deviceid_;
        this->x_wap_profile_ = orig.x_wap_profile_;
        this->proxy_connection_ = orig.proxy_connection_;
        this->body_ = orig.body_;
    }

    ~http_request() {
    }

    void set_method(std::string method) {
        this->method_ = method;
    }

    std::string get_method() const {
        return this->method_;
    }

    void set_path(std::string path) {
        this->path_ = path;
    }

    std::string get_path() const {
        return this->path_;
    }

    void set_version(std::string version) {
        this->version_ = version;
    }

    std::string get_version() const {
        return this->version_;
    }

    void set_accept(std::string accept) {
        this->accept_ = accept;
    }

    std::string get_accept() const {
        return this->accept_;
    }

    void set_accept_charset(std::string accept_charset) {
        this->accept_charset_ = accept_charset;
    }

    std::string get_accept_charset() const {
        return this->accept_charset_;
    }

    void set_accept_encoding(std::string accept_encoding) {
        this->accept_encoding_ = accept_encoding;
    }

    std::string get_accept_encoding() const {
        return this->accept_encoding_;
    }

    void set_accept_language(std::string accept_language) {
        this->accept_language_ = accept_language;
    }

    std::string get_accept_language() const {
        return this->accept_language_;
    }

    void set_accept_datetime(std::string accept_datetime) {
        this->accept_datetime_ = accept_datetime;
    }

    std::string get_accept_datetime() const {
        return this->accept_datetime_;
    }

    void set_authorization(std::string authorization) {
        this->authorization_ = authorization;
    }

    std::string get_authorization() const {
        return this->authorization_;
    }

    void set_cache_control(std::string cache_control) {
        this->cache_control_ = cache_control;
    }

    std::string get_cache_control() const {
        return this->cache_control_;
    }

    void set_connection(std::string connection) {
        this->connection_ = connection;
    }

    std::string get_connection() const {
        return this->connection_;
    }

    void set_cookie(std::string cookie) {
        this->cookie_ = cookie;
    }

    std::string get_cookie() const {
        return this->cookie_;
    }

    void set_content_length(size_t content_length) {
        std::stringstream ss;
        ss << content_length;
        this->content_length_ = ss.str();
    }

    size_t get_content_length() const {
        if (this->content_length_ == "undefined") {
            return 0;
        } else {
            return string_util::to_number<size_t>(this->content_length_);
        }
    }

    void set_content_md5(std::string content_md5) {
        this->content_md5_ = content_md5;
    }

    std::string get_content_md5() const {
        return this->content_md5_;
    }

    void set_content_type(std::string content_type) {
        this->content_type_ = content_type;
    }

    std::string get_content_type() const {
        return this->content_type_;
    }

    void set_date(std::string date) {
        this->date_ = date;
    }

    std::string get_date() const {
        return this->date_;
    }

    void set_expect(std::string expect) {
        this->expect_ = expect;
    }

    std::string get_expect() const {
        return this->expect_;
    }

    void set_from(std::string from) {
        this->from_ = from;
    }

    std::string get_from() const {
        return this->from_;
    }

    void set_host(std::string host) {
        this->host_ = host;
    }

    std::string get_host() const {
        return this->host_;
    }

    void set_if_match(std::string if_match) {
        this->if_match_ = if_match;
    }

    std::string get_if_match() const {
        return this->if_match_;
    }

    void set_if_modified_since(std::string if_modified_since) {
        this->if_modified_since_ = if_modified_since;
    }

    std::string get_if_modified_since() const {
        return this->if_modified_since_;
    }

    void set_if_none_match(std::string if_none_match) {
        this->if_none_match_ = if_none_match;
    }

    std::string get_if_none_match() const {
        return this->if_none_match_;
    }

    void set_if_range(std::string if_range) {
        this->if_range_ = if_range;
    }

    std::string get_if_range() const {
        return this->if_range_;
    }

    void set_if_unmodified_since(std::string if_unmodified_since) {
        this->if_unmodified_since_ = if_unmodified_since;
    }

    std::string get_if_unmodified_since() const {
        return this->if_unmodified_since_;
    }

    void set_max_forwards(std::string max_forwards) {
        this->max_forwards_ = max_forwards;
    }

    std::string get_max_forwards() const {
        return this->max_forwards_;
    }

    void set_pragma(std::string pragma) {
        this->pragma_ = pragma;
    }

    std::string get_pragma() const {
        return this->pragma_;
    }

    void set_proxy_authorization(std::string proxy_authorization) {
        this->proxy_authorization_ = proxy_authorization;
    }

    std::string get_proxy_authorization() const {
        return this->proxy_authorization_;
    }

    void set_range(std::string range) {
        this->range_ = range;
    }

    std::string get_range() const {
        return this->range_;
    }

    void set_referer(std::string referer) {
        this->referer_ = referer;
    }

    std::string get_referer() const {
        return this->referer_;
    }

    void set_te(std::string te) {
        this->te_ = te;
    }

    std::string get_te() const {
        return this->te_;
    }

    void set_upgrade(std::string upgrade) {
        this->upgrade_ = upgrade;
    }

    std::string get_upgrade() const {
        return this->upgrade_;
    }

    void set_user_agent(std::string user_agent) {
        this->user_agent_ = user_agent;
    }

    std::string get_user_agent() const {
        return this->user_agent_;
    }

    void set_via(std::string via) {
        this->via_ = via;
    }

    std::string get_via() const {
        return this->via_;
    }

    void set_warning(std::string warning) {
        this->warning_ = warning;
    }

    std::string get_warning() const {
        return this->warning_;
    }

    void set_x_requested_with(std::string x_requested_with) {
        this->x_requested_with_ = x_requested_with;
    }

    std::string get_x_requested_with() const {
        return this->x_requested_with_;
    }

    void set_dnt(std::string dnt) {
        this->dnt_ = dnt;
    }

    std::string get_dnt() const {
        return this->dnt_;
    }

    void set_x_forwarded_for(std::string x_forwarded_for) {
        this->x_forwarded_for_ = x_forwarded_for;
    }

    std::string get_x_forwarded_for() const {
        return this->x_forwarded_for_;
    }

    void set_x_forwarded_proto(std::string x_forwarded_proto) {
        this->x_forwarded_proto_ = x_forwarded_proto;
    }

    std::string get_x_forwarded_proto() const {
        return this->x_forwarded_proto_;
    }

    void set_front_end_https(std::string front_end_https) {
        this->front_end_https_ = front_end_https;
    }

    std::string get_front_end_https() const {
        return this->front_end_https_;
    }

    void set_x_att_device_id(std::string x_att_deviceid) {
        this->x_att_deviceid_ = x_att_deviceid;
    }

    std::string get_x_att_device_id() const {
        return this->x_att_deviceid_;
    }

    void set_x_wap_profile(std::string x_wap_profile) {
        this->x_wap_profile_ = x_wap_profile;
    }

    std::string get_x_wap_profile() const {
        return this->x_wap_profile_;
    }

    void set_proxy_connection(std::string proxy_connection) {
        this->proxy_connection_ = proxy_connection;
    }

    std::string get_proxy_connection() const {
        return this->proxy_connection_;
    }

    void set_body(std::string body) {
        this->body_ = body;
    }

    std::string get_body()const {
        return this->body_;
    }

    void header_from_string(std::string header) {



        std::stringstream ss_h;
        ss_h << header;


        std::vector<std::string> fields = string_util::tokenize(ss_h.str(), "\r\n");
        std::vector<std::string> method;
        method = string_util::tokenize(fields[0], " ");

        if (method.size() > 0) {
            this->set_method(method.at(0));
        }

        if (method.size() > 1) {
            this->set_path(method.at(1));
        }

        if (method.size() > 2) {
            this->set_version(method.at(2));
        }

        for (int i = 0; i < fields.size(); i++) {
            std::string entry = fields[i];
            std::vector<std::string> tokens = string_util::tokenize(entry, ":");

            if (tokens.size() > 1) {




                std::string field = tokens[0];

                std::remove(field.begin(), field.end(), ' ');

                if (field == "Accept") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept(string_util::trim(temp.str()));
                }
                if (field == "Accept-Charset") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept_charset(string_util::trim(temp.str()));
                }
                if (field == "Accept-Encoding") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept_encoding(string_util::trim(temp.str()));
                }
                if (field == "Accept-Language") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept_language(string_util::trim(temp.str()));
                }
                if (field == "Accept-Datetime") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept_datetime(string_util::trim(temp.str()));
                }
                if (field == "Authorization") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_authorization(string_util::trim(temp.str()));
                }
                if (field == "Cache-Control") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_cache_control(string_util::trim(temp.str()));
                }
                if (field == "Connection") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_connection(string_util::trim(temp.str()));
                }
                if (field == "Cookie") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_cookie(string_util::trim(temp.str()));
                }
                if (field == "Content-Length") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_length(string_util::to_number<size_t>(string_util::trim(temp.str())));
                }
                if (field == "Content-MD5") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_md5(string_util::trim(temp.str()));
                }
                if (field == "Content-Type") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_type(string_util::trim(temp.str()));
                }
                if (field == "Date") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_date(string_util::trim(temp.str()));
                }
                if (field == "Expect") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_expect(string_util::trim(temp.str()));
                }
                if (field == "From") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_from(string_util::trim(temp.str()));
                }
                if (field == "Host") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_host(string_util::trim(temp.str()));
                }
                if (field == "If-Match") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_if_match(string_util::trim(temp.str()));
                }
                if (field == "If-Modified-Since") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_if_modified_since(string_util::trim(temp.str()));
                }
                if (field == "If-None-Match") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_if_none_match(string_util::trim(temp.str()));
                }
                if (field == "If-Range") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_if_range(string_util::trim(temp.str()));
                }
                if (field == "If-Unmodified-Since") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_if_unmodified_since(string_util::trim(temp.str()));
                }
                if (field == "Max-Forwards") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_max_forwards(string_util::trim(temp.str()));
                }
                if (field == "Pragma") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_pragma(string_util::trim(temp.str()));
                }
                if (field == "Proxy-Authorization") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_proxy_authorization(string_util::trim(temp.str()));
                }
                if (field == "Range") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_range(string_util::trim(temp.str()));
                }
                if (field == "Referer") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_referer(string_util::trim(temp.str()));
                }
                if (field == "TE") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_te(string_util::trim(temp.str()));
                }
                if (field == "Upgrade") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_upgrade(string_util::trim(temp.str()));
                }
                if (field == "User-Agent") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_user_agent(string_util::trim(temp.str()));
                }
                if (field == "Via") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_via(string_util::trim(temp.str()));
                }
                if (field == "Warning") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_warning(string_util::trim(temp.str()));
                }
                if (field == "X-Requested-With") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_requested_with(string_util::trim(temp.str()));
                }
                if (field == "DNT") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_dnt(string_util::trim(temp.str()));
                }
                if (field == "X-Forwarded-For") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_forwarded_for(string_util::trim(temp.str()));
                }
                if (field == "X-Forwarded-Proto") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_forwarded_proto(string_util::trim(temp.str()));
                }
                if (field == "Front-End-Https") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_front_end_https(string_util::trim(temp.str()));
                }
                if (field == "X-ATT-DeviceId") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_att_device_id(string_util::trim(temp.str()));
                }
                if (field == "X-Wap-Profile") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_wap_profile(string_util::trim(temp.str()));
                }
                if (field == "Proxy-Connection") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_proxy_connection(string_util::trim(temp.str()));
                }

            }
        }

    }

    std::string header_to_string() const {
        std::stringstream ss;


        bool method_info = false;
        if (this->method_ != "undefined") {
            method_info = true;
            ss << this->method_ << ' ';
        }

        if (this->path_ != "undefined") {
            method_info = true;
            ss << this->path_ << ' ';
        }


        if (this->version_ != "undefined") {
            method_info = true;
            ss << this->version_;
        }

        if (method_info) {
            ss << "\r\n";
        }



        if (this->host_ != "undefined") {
            ss << "Host:" << this->get_host() << "\r\n";
        }

        if (this->accept_ != "undefined") {
            ss << "Accept:" << this->get_accept() << "\r\n";
        } else if (this->accept_charset_ != "undefined") {
            ss << "Accept-Charset:" << this->get_accept_charset() << "\r\n";
        }
        if (this->accept_encoding_ != "undefined") {
            ss << "Accept-Encoding:" << this->get_accept_encoding() << "\r\n";
        }
        if (this->accept_language_ != "undefined") {
            ss << "Accept-Language:" << this->get_accept_language() << "\r\n";
        }
        if (this->accept_datetime_ != "undefined") {
            ss << "Accept-Datetime:" << this->get_accept_datetime() << "\r\n";
        }
        if (this->authorization_ != "undefined") {
            ss << "Authorization:" << this->get_authorization() << "\r\n";
        }
        if (this->cache_control_ != "undefined") {
            ss << "Cache-Control:" << this->get_cache_control() << "\r\n";
        }
        if (this->connection_ != "undefined") {
            ss << "Connection:" << this->get_connection() << "\r\n";
        }
        if (this->cookie_ != "undefined") {
            ss << "Cookie:" << this->get_cookie() << "\r\n";
        }

        if (this->content_length_ != "undefined") {
            ss << "Content-Length:" << this->get_content_length() << "\r\n";
        }

        if (this->content_md5_ != "undefined") {
            ss << "Content-MD5:" << this->get_content_md5() << "\r\n";
        }
        if (this->content_type_ != "undefined") {
            ss << "Content-Type:" << this->get_content_type() << "\r\n";
        }
        if (this->date_ != "undefined") {
            ss << "Date:" << this->get_date() << "\r\n";
        }
        if (this->expect_ != "undefined") {
            ss << "Expect:" << this->get_expect() << "\r\n";
        }
        if (this->from_ != "undefined") {
            ss << "From:" << this->get_from() << "\r\n";
        }

        if (this->if_match_ != "undefined") {
            ss << "If-Match:" << this->get_if_match() << "\r\n";
        }
        if (this->if_modified_since_ != "undefined") {
            ss << "If-Modified-Since:" << this->get_if_modified_since() << "\r\n";
        }
        if (this->if_none_match_ != "undefined") {
            ss << "If-None-Match:" << this->get_if_none_match() << "\r\n";
        }
        if (this->if_range_ != "undefined") {
            ss << "If-Range:" << this->get_if_range() << "\r\n";
        }
        if (this->if_unmodified_since_ != "undefined") {
            ss << "If-Unmodified-Since:" << this->get_if_unmodified_since() << "\r\n";
        }
        if (this->max_forwards_ != "undefined") {
            ss << "Max-Forwards:" << this->get_max_forwards() << "\r\n";
        }
        if (this->pragma_ != "undefined") {
            ss << "Pragma:" << this->get_pragma() << "\r\n";
        }
        if (this->proxy_authorization_ != "undefined") {
            ss << "Proxy-Authorization:" << this->get_proxy_authorization() << "\r\n";
        }
        if (this->range_ != "undefined") {
            ss << "Range:" << this->get_range() << "\r\n";
        }
        if (this->referer_ != "undefined") {
            ss << "Referer:" << this->get_referer() << "\r\n";
        }
        if (this->te_ != "undefined") {
            ss << "TE:" << this->get_te() << "\r\n";
        }
        if (this->upgrade_ != "undefined") {
            ss << "Upgrade:" << this->get_upgrade() << "\r\n";
        }
        if (this->user_agent_ != "undefined") {
            ss << "User-Agent:" << this->get_user_agent() << "\r\n";
        }
        if (this->via_ != "undefined") {
            ss << "Via:" << this->get_via() << "\r\n";
        }
        if (this->warning_ != "undefined") {
            ss << "Warning:" << this->get_warning() << "\r\n";
        }
        if (this->x_requested_with_ != "undefined") {
            ss << "X-Requested-With:" << this->get_x_requested_with() << "\r\n";
        }
        if (this->dnt_ != "undefined") {
            ss << "DNT:" << this->get_dnt() << "\r\n";
        }
        if (this->x_forwarded_for_ != "undefined") {
            ss << "X-Forwarded-For:" << this->get_x_forwarded_for() << "\r\n";
        }
        if (this->x_forwarded_proto_ != "undefined") {
            ss << "X-Forwarded-Proto:" << this->get_x_forwarded_proto() << "\r\n";
        }
        if (this->front_end_https_ != "undefined") {
            ss << "Front-End-Https:" << this->get_front_end_https() << "\r\n";
        }
        if (this->x_att_deviceid_ != "undefined") {
            ss << "X-ATT-DeviceId:" << this->get_x_att_device_id() << "\r\n";
        }
        if (this->x_wap_profile_ != "undefined") {
            ss << "X-Wap-Profile:" << this->get_x_wap_profile() << "\r\n";
        }
        if (this->proxy_connection_ != "undefined") {
            ss << "Proxy-Connection:" << this->get_proxy_connection() << "\r\n";
        }
        ss << "\r\n";

        return ss.str();

    }

private:


    /**
     *Request method, GET, POST, etc.
     */
    std::string method_;

    /**
     *Method path.
     */
    std::string path_;

    /**
     *HTTP version, HTTP 1.1, etc.
     */
    std::string version_;
    /**
     * Content-Types that are acceptable
     * example: Accept: text/plain
     */
    std::string accept_;
    /**
     * Character sets that are acceptable
     * example: Accept-Charset: utf-8
     */
    std::string accept_charset_;
    /**
     * Acceptable encodings. See HTTP compression.
     * example: Accept-Encoding: gzip, deflate
     */
    std::string accept_encoding_;
    /**
     * Acceptable languages for response
     * example: Accept-Language: en-US
     */
    std::string accept_language_;
    /**
     * Acceptable version in time
     * example: Accept-Datetime: Thu, 31 May 2007 20:35:00 GMT
     */
    std::string accept_datetime_;
    /**
     * Authentication credentials for HTTP authentication
     * example: Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
     */
    std::string authorization_;
    /**
     * Used to specify directives that MUST be obeyed by all caching mechanisms along the request/response chain
     * example: Cache-Control: no-cache
     */
    std::string cache_control_;
    /**
     * What type of connection the user-agent would prefer
     * example: Connection: keep-alive
     */
    std::string connection_;
    /**
     * an HTTP cookie previously sent by the server with set_-Cookie (below)
     * example: Cookie: $Version=1; Skin=new;
     */
    std::string cookie_;
    /**
     * The length of the request body in octets (8-bit bytes)
     * example: Content-Length: 348
     */
    std::string content_length_;
    /**
     * A Base64-encoded binary MD5 sum of the content of the request body
     * example: Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
     */
    std::string content_md5_;
    /**
     * The MIME type of the body of the request (used with POST and PUT requests)
     * example: Content-Type: application/x-www-form-urlencoded
     */
    std::string content_type_;
    /**
     * The date and time that the message was sent
     * example: Date: Tue, 15 Nov 1994 08:12:31 GMT
     */
    std::string date_;
    /**
     * Indicates that particular server behaviors are required by the client
     * example: Expect: 100-continue
     */
    std::string expect_;
    /**
     * The email address of the user making the request
     * example: From: user@example.com
     */
    std::string from_;
    /**
     * The domain name of the server (for virtual hosting), and the TCP port number on which the server is listening.
     * example: Host: en.wikipedia.org:80
     */
    std::string host_;
    /**
     * Only perform the action if the client supplied entity matches the same entity on the server. This is mainly for methods like PUT to only update a resource if it has not been modified since the user last updated it.
     * example: If-Match: "737060cd8c284d8af7ad3082f209582d"
     */
    std::string if_match_;
    /**
     * Allows a 304 Not Modified to be returned if content is unchanged
     * example: If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT
     */
    std::string if_modified_since_;
    /**
     * Allows a 304 Not Modified to be returned if content is unchanged, see HTTP ETag
     * example: If-None-Match: "737060cd8c284d8af7ad3082f209582d"
     */
    std::string if_none_match_;
    /**
     * If the entity is unchanged, send me the part(s) that I am missing; otherwise, send me the entire new entity
     * example: If-Range: "737060cd8c284d8af7ad3082f209582d"
     */
    std::string if_range_;
    /**
     * Only send the response if the entity has not been modified since a specific time.
     * example: If-Unmodified-Since: Sat, 29 Oct 1994 19:43:31 GMT
     */
    std::string if_unmodified_since_;
    /**
     * Limit the number of times the message can be forwarded through proxies or gateways.
     * example: Max-Forwards: 10
     */
    std::string max_forwards_;
    /**
     * Implementation-specific headers that may have various effects anywhere along the request-response chain.
     * example: Pragma: no-cache
     */
    std::string pragma_;
    /**
     * Authorization credentials for connecting to a proxy.
     * example: Proxy-Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
     */
    std::string proxy_authorization_;
    /**
     * Request only part of an entity. Bytes are numbered from 0.
     * example: Range: bytes=500-999
     */
    std::string range_;
    /**
     * This is the address of the previous web page from which a link to the currently requested page was followed. (The word “referrer” is misspelled in the RFC as well as in most implementations.)
     * example: Referer: http://en.wikipedia.org/wiki/Main_Page
     */
    std::string referer_;
    /**
     * The transfer encodings the user agent is willing to accept: the same values as for the response header Transfer-Encoding can be used, plus the "trailers" value (related to the "chunked" transfer method) to notify the server it expects to receive additional headers (the trailers) after the last, zero-sized, chunk.
     * example: TE: trailers, deflate
     */
    std::string te_;
    /**
     * Ask the server to upgrade to another protocol.
     * example: Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11
     */
    std::string upgrade_;
    /**
     * The user agent string of the user agent
     * example: User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0
     */
    std::string user_agent_;
    /**
     * Informs the server of proxies through which the request was sent.
     * example: Via: 1.0 fred, 1.1 example.com (Apache/1.1)
     */
    std::string via_;
    /**
     * A general warning about possible problems with the entity body.
     * example: Warning: 199 Miscellaneous warning
     */
    std::string warning_;
    /**
     * mainly used to identify Ajax requests. Most JavaScript frameworks send this header with value of XMLHttpRequest
     * example: X-Requested-With: XMLHttpRequest
     */
    std::string x_requested_with_;
    /**
     * Requests a web application to disable their tracking of a user.
     * example: DNT: 1 (Do Not Track Enabled)
     */
    std::string dnt_;
    /**
     * a de facto standard for identifying the originating IP address of a client connecting to a web server through an HTTP proxy or load balancer.
     * example: X-Forwarded-For: 129.78.138.66, 129.78.64.103
     */
    std::string x_forwarded_for_;
    /**
     * a de facto standard for identifying the originating protocol of an HTTP request, since a reverse proxy (load balancer) may communicate with a web server using HTTP even if the request to the reverse proxy is HTTPS
     * example: X-Forwarded-Proto: https
     */
    std::string x_forwarded_proto_;
    /**
     * Non-standard header used by Microsoft applications and load-balancers
     * example: Front-End-Https: on
     */
    std::string front_end_https_;
    /**
     * Allows easier parsing of the MakeModel/Firmware that is usually found in the User-Agent String of AT&T Devices
     * example: x-att-deviceid: MakeModel/Firmware
     */
    std::string x_att_deviceid_;
    /**
     * Links to an XML file on the Internet with a full description and details about the device currently connecting. In the example to the right is an XML file for an AT&T Samsung Galaxy S2.
     * example: x-wap-profile:http://wap.samsungmobile.com/uaprof/SGH-I777.xml
     */
    std::string x_wap_profile_;
    /**
     * Implemented as a misunderstanding of the HTTP specifications. Common because of mistakes in implementations of early HTTP versions. Has exactly the same functionality as standard Connection header.
     * example: Proxy-Connection: keep-alive
     */
    std::string proxy_connection_;


    /**
     *HTTP body.
     */
    std::string body_;
};

class http_response {
public:

    http_response()
    : version_(std::string("undefined")),
    status_code_(std::string("undefined")),
    status_message_(std::string("undefined")),
    access_control_allow_origin_(std::string("undefined")),
    accept_ranges_(std::string("undefined")),
    age_(std::string("undefined")),
    allow_(std::string("undefined")),
    cache_control_(std::string("undefined")),
    connection_(std::string("undefined")),
    content_encoding_(std::string("undefined")),
    content_language_(std::string("undefined")),
    content_length_(std::string("undefined")),
    content_location_(std::string("undefined")),
    content_md5_(std::string("undefined")),
    content_disposition_(std::string("undefined")),
    content_range_(std::string("undefined")),
    content_type_(std::string("undefined")),
    date_(std::string("undefined")),
    etag_(std::string("undefined")),
    expires_(std::string("undefined")),
    last_modified_(std::string("undefined")),
    link_(std::string("undefined")),
    location_(std::string("undefined")),
    p3p_(std::string("undefined")),
    pragma_(std::string("undefined")),
    proxy_authenticate_(std::string("undefined")),
    refresh_(std::string("undefined")),
    retry_after_(std::string("undefined")),
    server_(std::string("undefined")),
    set_cookie_(std::string("undefined")),
    strict_transport_security_(std::string("undefined")),
    trailer_(std::string("undefined")),
    transfer_encoding_(std::string("undefined")),
    vary_(std::string("undefined")),
    via_(std::string("undefined")),
    warning_(std::string("undefined")),
    www_authenticate_(std::string("undefined")),
    x_frame_options_(std::string("undefined")),
    x_xss_protection_(std::string("undefined")),
    x_content_type_options_(std::string("undefined")),
    x_powered_by_(std::string("undefined")),
    x_ua_compatible_(std::string("undefined")),
    body_(std::string("undefined")) {
    }

    http_response(const http_response& orig) {
        this->access_control_allow_origin_ = orig.access_control_allow_origin_;
        this->accept_ranges_ = orig.accept_ranges_;
        this->age_ = orig.age_;
        this->allow_ = orig.allow_;
        this->cache_control_ = orig.cache_control_;
        this->connection_ = orig.connection_;
        this->content_encoding_ = orig.content_encoding_;
        this->content_language_ = orig.content_language_;
        this->content_length_ = orig.content_length_;
        this->content_location_ = orig.content_location_;
        this->content_md5_ = orig.content_md5_;
        this->content_disposition_ = orig.content_disposition_;
        this->content_range_ = orig.content_range_;
        this->content_type_ = orig.content_type_;
        this->date_ = orig.date_;
        this->etag_ = orig.etag_;
        this->expires_ = orig.expires_;
        this->last_modified_ = orig.last_modified_;
        this->link_ = orig.link_;
        this->location_ = orig.location_;
        this->p3p_ = orig.p3p_;
        this->pragma_ = orig.pragma_;
        this->proxy_authenticate_ = orig.proxy_authenticate_;
        this->refresh_ = orig.refresh_;
        this->retry_after_ = orig.retry_after_;
        this->server_ = orig.server_;
        this->set_cookie_ = orig.set_cookie_;
        this->strict_transport_security_ = orig.strict_transport_security_;
        this->trailer_ = orig.trailer_;
        this->transfer_encoding_ = orig.transfer_encoding_;
        this->vary_ = orig.vary_;
        this->via_ = orig.via_;
        this->warning_ = orig.warning_;
        this->www_authenticate_ = orig.www_authenticate_;
        this->x_frame_options_ = orig.x_frame_options_;
        this->x_xss_protection_ = orig.x_xss_protection_;
        this->x_content_type_options_ = orig.x_content_type_options_;
        this->x_powered_by_ = orig.x_powered_by_;
        this->x_ua_compatible_ = orig.x_ua_compatible_;
    }

    ~http_response() {
    }

    std::string get_statuscode() const {
        return status_code_;
    }

    void set_statuscode(std::string status_code) {
        this->status_code_ = status_code;
    }

    std::string get_statusmessage() const {
        return status_message_;
    }

    void set_statusmessage(std::string status_message) {
        this->status_message_ = status_message;
    }

    std::string get_version() const {
        return version_;
    }

    void set_version(std::string version) {
        this->version_ = version;
    }

    void set_access_control_allow_origin(std::string access_control_allow_origin) {
        this->access_control_allow_origin_ = access_control_allow_origin;
    }

    std::string get_access_control_allow_origin() const {
        return this->access_control_allow_origin_;
    }

    void set_accept_ranges(std::string accept_ranges) {
        this->accept_ranges_ = accept_ranges;
    }

    std::string get_accept_ranges() const {
        return this->accept_ranges_;
    }

    void set_age(std::string age) {
        this->age_ = age;
    }

    std::string get_age() const {
        return this->age_;
    }

    void set_allow(std::string allow) {
        this->allow_ = allow;
    }

    std::string get_allow() const {
        return this->allow_;
    }

    void set_cache_control(std::string cache_control) {
        this->cache_control_ = cache_control;
    }

    std::string get_cache_control() const {
        return this->cache_control_;
    }

    void set_connection(std::string connection) {
        this->connection_ = connection;
    }

    std::string get_connection() const {
        return this->connection_;
    }

    void set_content_encoding(std::string content_encoding) {
        this->content_encoding_ = content_encoding;
    }

    std::string get_content_encoding() const {
        return this->content_encoding_;
    }

    void set_content_language(std::string content_language) {
        this->content_language_ = content_language;
    }

    std::string get_content_language() const {
        return this->content_language_;
    }

    void set_content_length(std::string content_length) {
        this->content_length_ = content_length;
    }

    std::string get_content_length() const {
        return this->content_length_;
    }

    void set_content_location(std::string content_location) {
        this->content_location_ = content_location;
    }

    std::string get_content_location() const {
        return this->content_location_;
    }

    void set_content_md5(std::string content_md5) {
        this->content_md5_ = content_md5;
    }

    std::string get_content_md5() const {
        return this->content_md5_;
    }

    void set_content_disposition(std::string content_disposition) {
        this->content_disposition_ = content_disposition;
    }

    std::string get_content_disposition() const {
        return this->content_disposition_;
    }

    void set_content_range(std::string content_range) {
        this->content_range_ = content_range;
    }

    std::string get_content_range() const {
        return this->content_range_;
    }

    void set_content_type(std::string content_type) {
        this->content_type_ = content_type;
    }

    std::string get_content_type() const {
        return this->content_type_;
    }

    void set_date(std::string date) {
        this->date_ = date;
    }

    std::string get_date() const {
        return this->date_;
    }

    void set_etag(std::string etag) {
        this->etag_ = etag;
    }

    std::string get_etag() const {
        return this->etag_;
    }

    void set_expires(std::string expires) {
        this->expires_ = expires;
    }

    std::string get_expires() const {
        return this->expires_;
    }

    void set_last_modified(std::string last_modified) {
        this->last_modified_ = last_modified;
    }

    std::string get_last_modified() const {
        return this->last_modified_;
    }

    void set_link(std::string link) {
        this->link_ = link;
    }

    std::string get_link() const {
        return this->link_;
    }

    void set_location(std::string location) {
        this->location_ = location;
    }

    std::string get_location() const {
        return this->location_;
    }

    void set_p3p(std::string p3p) {
        this->p3p_ = p3p;
    }

    std::string get_p3p() const {
        return this->p3p_;
    }

    void set_pragma(std::string pragma) {
        this->pragma_ = pragma;
    }

    std::string get_pragma() const {
        return this->pragma_;
    }

    void set_proxy_authenticate(std::string proxy_authenticate) {
        this->proxy_authenticate_ = proxy_authenticate;
    }

    std::string get_proxy_authenticate() const {
        return this->proxy_authenticate_;
    }

    void set_refresh(std::string refresh) {
        this->refresh_ = refresh;
    }

    std::string get_refresh() const {
        return this->refresh_;
    }

    void set_retry_after(std::string retry_after) {
        this->retry_after_ = retry_after;
    }

    std::string get_retry_after() const {
        return this->retry_after_;
    }

    void set_server(std::string server) {
        this->server_ = server;
    }

    std::string get_server() const {
        return this->server_;
    }

    void set_set_cookie(std::string set_cookie) {
        this->set_cookie_ = set_cookie;
    }

    std::string get_set_cookie() const {
        return this->set_cookie_;
    }

    void set_strict_transport_security(std::string strict_transport_security) {
        this->strict_transport_security_ = strict_transport_security;
    }

    std::string get_strict_transport_security() const {
        return this->strict_transport_security_;
    }

    void set_trailer(std::string trailer) {
        this->trailer_ = trailer;
    }

    std::string get_trailer() const {
        return this->trailer_;
    }

    void set_transfer_encoding(std::string transfer_encoding) {
        this->transfer_encoding_ = transfer_encoding;
    }

    std::string get_transfer_encoding() const {
        return this->transfer_encoding_;
    }

    void set_vary(std::string vary) {
        this->vary_ = vary;
    }

    std::string get_vary() const {
        return this->vary_;
    }

    void set_via(std::string via) {
        this->via_ = via;
    }

    std::string get_via() const {
        return this->via_;
    }

    void set_warning(std::string warning) {
        this->warning_ = warning;
    }

    std::string get_warning() const {
        return this->warning_;
    }

    void set_www_authenticate(std::string www_authenticate) {
        this->www_authenticate_ = www_authenticate;
    }

    std::string get_www_authenticate() const {
        return this->www_authenticate_;
    }

    void set_x_frame_options(std::string x_frame_options) {
        this->x_frame_options_ = x_frame_options;
    }

    std::string get_x_frame_options() const {
        return this->x_frame_options_;
    }

    void set_x_xss_protection(std::string x_xss_protection) {
        this->x_xss_protection_ = x_xss_protection;
    }

    std::string get_x_xss_protection() const {
        return this->x_xss_protection_;
    }

    void set_x_content_type_options(std::string x_content_type_options) {
        this->x_content_type_options_ = x_content_type_options;
    }

    std::string get_x_content_type_options() const {
        return this->x_content_type_options_;
    }

    void set_x_powered_by(std::string x_powered_by) {
        this->x_powered_by_ = x_powered_by;
    }

    std::string get_x_powered_by() const {
        return this->x_powered_by_;
    }

    void set_x_ua_compatible(std::string x_ua_compatible) {
        this->x_ua_compatible_ = x_ua_compatible;
    }

    std::string get_x_ua_compatible() const {
        return this->x_ua_compatible_;
    }

    void set_body(std::string body) {
        this->body_ = body;
    }

    std::string get_body() const {
        return this->body_;
    }

    void set_data(const std::string& str){
        this->data_ =str;
    }
    
    std::string get_data()const{
        return this->data_;
    }
    
    void header_from_string(std::string header) {


        std::vector<std::string> fields = string_util::tokenize(header, "\r\n");
        std::vector<std::string> status;
        status = string_util::tokenize(fields[0], " ");

        if (status.size() > 0) {
            this->version_ = status.at(0);
        }

        if (status.size() > 1) {
            this->status_code_ = status.at(1);
        }
        if (status.size() > 2) {
            this->status_message_ = status.at(2);
        }


        for (int i = 0; i < fields.size(); i++) {
            std::string entry = fields[i];
            std::vector<std::string> tokens = string_util::tokenize(entry, ":");

            if (tokens.size() > 1) {

                std::string field = tokens[0];
                //typhoon::util::RemoveWhitespace(field);
                std::remove(field.begin(), field.end(), ' ');
                if (field == "Access-Control-Allow-Origin") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_access_control_allow_origin(string_util::trim(temp.str()));
                }
                if (field == "Accept-Ranges") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_accept_ranges(string_util::trim(temp.str()));
                }
                if (field == "Age") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_age(string_util::trim(temp.str()));
                }
                if (field == "Allow") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_allow(string_util::trim(temp.str()));
                }
                if (field == "Cache-Control") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_cache_control(string_util::trim(temp.str()));
                }
                if (field == "Connection") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_connection(string_util::trim(temp.str()));
                }
                if (field == "Content-Encoding") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_encoding(string_util::trim(temp.str()));
                }
                if (field == "Content-Language") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_language(string_util::trim(temp.str()));
                }
                if (field == "Content-Length") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_length(string_util::trim(temp.str()));
                }
                if (field == "Content-Location") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_location(string_util::trim(temp.str()));
                }
                if (field == "Content-MD5") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_md5(string_util::trim(temp.str()));
                }
                if (field == "Content-Disposition") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_disposition(string_util::trim(temp.str()));
                }
                if (field == "Content-Range") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_range(string_util::trim(temp.str()));
                }
                if (field == "Content-Type") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_content_type(string_util::trim(temp.str()));
                }
                if (field == "Date") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_date(string_util::trim(temp.str()));
                }
                if (field == "ETag") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_etag(string_util::trim(temp.str()));
                }
                if (field == "Expires") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_expires(string_util::trim(temp.str()));
                }
                if (field == "Last-Modified") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_last_modified(string_util::trim(temp.str()));
                }
                if (field == "Link") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_link(string_util::trim(temp.str()));
                }
                if (field == "Location") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_location(string_util::trim(temp.str()));
                }
                if (field == "P3P") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_p3p(string_util::trim(temp.str()));
                }
                if (field == "Pragma") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_pragma(string_util::trim(temp.str()));
                }
                if (field == "Proxy-Authenticate") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_proxy_authenticate(string_util::trim(temp.str()));
                }
                if (field == "Refresh") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_refresh(string_util::trim(temp.str()));
                }
                if (field == "Retry-After") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_retry_after(string_util::trim(temp.str()));
                }
                if (field == "Server") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_server(string_util::trim(temp.str()));
                }
                if (field == "set_-Cookie") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_set_cookie(string_util::trim(temp.str()));
                }
                if (field == "Strict-Transport-Security") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_strict_transport_security(string_util::trim(temp.str()));
                }
                if (field == "Trailer") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_trailer(string_util::trim(temp.str()));
                }
                if (field == "Transfer-Encoding") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_transfer_encoding(string_util::trim(temp.str()));
                }
                if (field == "Vary") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_vary(string_util::trim(temp.str()));
                }
                if (field == "Via") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_via(string_util::trim(temp.str()));
                }
                if (field == "Warning") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_warning(string_util::trim(temp.str()));
                }
                if (field == "WWW-Authenticate") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_www_authenticate(string_util::trim(temp.str()));
                }
                if (field == "X-Frame-Options") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_frame_options(string_util::trim(temp.str()));
                }
                if (field == "X-XSS-Protection") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_xss_protection(string_util::trim(temp.str()));
                }
                if (field == "X-Content-Type-Options") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_content_type_options(string_util::trim(temp.str()));
                }
                if (field == "X-Powered-By") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_powered_by(string_util::trim(temp.str()));
                }
                if (field == "X-UA-Compatible") {
                    std::stringstream temp;
                    for (size_t i = 1; i < tokens.size(); i++) {
                        if (i < tokens.size() - 1) {
                            temp << tokens[i] << ":";
                        } else {
                            temp << tokens[i];
                        }
                    }
                    this->set_x_ua_compatible(string_util::trim(temp.str()));
                }


            }
        }
    }

    std::string header_to_string() const {

        std::stringstream ss;

        bool has_status = false;

        if (this->version_ != "undefined") {
            has_status = true;
            ss << this->version_ << " ";
        }

        if (this->status_code_ != "undefined") {
            has_status = true;
            ss << this->status_code_ << " ";
        }

        if (this->status_message_ != "undefined") {
            has_status = true;
            ss << this->status_message_ << " ";
        }

        if (has_status) {
            ss << "\r\n";
        }

        if (this->access_control_allow_origin_ != "undefined") {
            ss << "Access-Control-Allow-Origin: " << this->get_access_control_allow_origin() << "\r\n";
        }
        if (this->accept_ranges_ != "undefined") {
            ss << "Accept-Ranges: " << this->get_accept_ranges() << "\r\n";
        }
        if (this->age_ != "undefined") {
            ss << "Age: " << this->get_age() << "\r\n";
        }
        if (this->allow_ != "undefined") {
            ss << "Allow: " << this->get_allow() << "\r\n";
        }
        if (this->cache_control_ != "undefined") {
            ss << "Cache-Control: " << this->get_cache_control() << "\r\n";
        }
        if (this->connection_ != "undefined") {
            ss << "Connection: " << this->get_connection() << "\r\n";
        }
        if (this->content_encoding_ != "undefined") {
            ss << "Content-Encoding: " << this->get_content_encoding() << "\r\n";
        }
        if (this->content_language_ != "undefined") {
            ss << "Content-Language: " << this->get_content_language() << "\r\n";
        }
        if (this->content_length_ != "undefined") {
            ss << "Content-Length: " << this->get_content_length() << "\r\n";
        }
        if (this->content_location_ != "undefined") {
            ss << "Content-Location: " << this->get_content_location() << "\r\n";
        }
        if (this->content_md5_ != "undefined") {
            ss << "Content-MD5: " << this->get_content_md5() << "\r\n";
        }
        if (this->content_disposition_ != "undefined") {
            ss << "Content-Disposition: " << this->get_content_disposition() << "\r\n";
        }
        if (this->content_range_ != "undefined") {
            ss << "Content-Range: " << this->get_content_range() << "\r\n";
        }
        if (this->content_type_ != "undefined") {
            ss << "Content-Type: " << this->get_content_type() << "\r\n";
        }
        if (this->date_ != "undefined") {
            ss << "Date: " << this->get_date() << "\r\n";
        }
        if (this->etag_ != "undefined") {
            ss << "ETag: " << this->get_etag() << "\r\n";
        }
        if (this->expires_ != "undefined") {
            ss << "Expires: " << this->get_expires() << "\r\n";
        }
        if (this->last_modified_ != "undefined") {
            ss << "Last-Modified: " << this->get_last_modified() << "\r\n";
        }
        if (this->link_ != "undefined") {
            ss << "Link: " << this->get_link() << "\r\n";
        }
        if (this->location_ != "undefined") {
            ss << "Location: " << this->get_location() << "\r\n";
        }
        if (this->p3p_ != "undefined") {
            ss << "P3P: " << this->get_p3p() << "\r\n";
        }
        if (this->pragma_ != "undefined") {
            ss << "Pragma: " << this->get_pragma() << "\r\n";
        }
        if (this->proxy_authenticate_ != "undefined") {
            ss << "Proxy-Authenticate: " << this->get_proxy_authenticate() << "\r\n";
        }
        if (this->refresh_ != "undefined") {
            ss << "Refresh: " << this->get_refresh() << "\r\n";
        }
        if (this->retry_after_ != "undefined") {
            ss << "Retry-After: " << this->get_retry_after() << "\r\n";
        }
        if (this->server_ != "undefined") {
            ss << "Server: " << this->get_server() << "\r\n";
        }
        if (this->set_cookie_ != "undefined") {
            ss << "set_-Cookie: " << this->get_set_cookie() << "\r\n";
        }
        if (this->strict_transport_security_ != "undefined") {
            ss << "Strict-Transport-Security: " << this->get_strict_transport_security() << "\r\n";
        }
        if (this->trailer_ != "undefined") {
            ss << "Trailer: " << this->get_trailer() << "\r\n";
        }
        if (this->transfer_encoding_ != "undefined") {
            ss << "Transfer-Encoding: " << this->get_transfer_encoding() << "\r\n";
        }
        if (this->vary_ != "undefined") {
            ss << "Vary: " << this->get_vary() << "\r\n";
        }
        if (this->via_ != "undefined") {
            ss << "Via: " << this->get_via() << "\r\n";
        }
        if (this->warning_ != "undefined") {
            ss << "Warning: " << this->get_warning() << "\r\n";
        }
        if (this->www_authenticate_ != "undefined") {
            ss << "WWW-Authenticate: " << this->get_www_authenticate() << "\r\n";
        }
        if (this->x_frame_options_ != "undefined") {
            ss << "X-Frame-Options: " << this->get_x_frame_options() << "\r\n";
        }
        if (this->x_xss_protection_ != "undefined") {
            ss << "X-XSS-Protection: " << this->get_x_xss_protection() << "\r\n";
        }
        if (this->x_content_type_options_ != "undefined") {
            ss << "X-Content-Type-Options: " << this->get_x_content_type_options() << "\r\n";
        }
        if (this->x_powered_by_ != "undefined") {
            ss << "X-Powered-By: " << this->get_x_powered_by() << "\r\n";
        }
        if (this->x_ua_compatible_ != "undefined") {
            ss << "X-UA-Compatible: " << this->get_x_ua_compatible() << "\r\n";
        }

        ss << "\r\n";

        return ss.str();
    }

private:

    std::string data_;

    std::string version_;

    std::string status_code_;

    std::string status_message_;
    /**
     * Specifying which web sites can participate in cross-origin resource sharing
     * example: Access-Control-Allow-Origin: *
     */
    std::string access_control_allow_origin_;
    /**
     * What partial content range types this server supports
     * example: Accept-Ranges: bytes
     */
    std::string accept_ranges_;
    /**
     * The age the object has been in a proxy cache in seconds
     * example: Age: 12
     */
    std::string age_;
    /**
     * Valid actions for a specified resource. To be used for a 405 Method not allowed
     * example: Allow: GET, HEAD
     */
    std::string allow_;
    /**
     * Tells all caching mechanisms from server to client whether they may cache this object. It is measured in seconds
     * example: Cache-Control: max-age=3600
     */
    std::string cache_control_;
    /**
     * Options that are desired for the connection[20]
     * example: Connection: close
     */
    std::string connection_;
    /**
     * The type of encoding used on the data. See HTTP compression.
     * example: Content-Encoding: gzip
     */
    std::string content_encoding_;
    /**
     * The language the content is in
     * example: Content-Language: da
     */
    std::string content_language_;
    /**
     * The length of the response body in octets (8-bit bytes)
     * example: Content-Length: 348
     */
    std::string content_length_;
    /**
     * An alternate location for the returned data
     * example: Content-Location: /index.htm
     */
    std::string content_location_;
    /**
     * A Base64-encoded binary MD5 sum of the content of the response
     * example: Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
     */
    std::string content_md5_;
    /**
     * An opportunity to raise a "File Download" dialogue box for a known MIME type with binary format or suggest a filename for dynamic content. Quotes are necessary with special characters.
     * example: Content-Disposition: attachment; filename="fname.ext"
     */
    std::string content_disposition_;
    /**
     * Where in a full body message this partial message belongs
     * example: Content-Range: bytes 21010-47021/47022
     */
    std::string content_range_;
    /**
     * The MIME type of this content
     * example: Content-Type: text/html; charset=utf-8
     */
    std::string content_type_;
    /**
     * The date and time that the message was sent
     * example: Date: Tue, 15 Nov 1994 08:12:31 GMT
     */
    std::string date_;
    /**
     * An identifier for a specific version of a resource, often a message digest
     * example: ETag: "737060cd8c284d8af7ad3082f209582d"
     */
    std::string etag_;
    /**
     * Gives the date/time after which the response is considered stale
     * example: Expires: Thu, 01 Dec 1994 16:00:00 GMT
     */
    std::string expires_;
    /**
     * The last modified date for the requested object, in RFC 2822 format
     * example: Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
     */
    std::string last_modified_;
    /**
     * Used to express a typed relationship with another resource, where the relation type is defined by RFC 5988
     * example: Link: </feed>; rel="alternate"[24]
     */
    std::string link_;
    /**
     * Used in redirection, or when a new resource has been created.
     * example: Location: http://www.w3.org/pub/WWW/People.html
     */
    std::string location_;
    /**
     * This header is supposed to set P3P policy, in the form ofP3P:CP="your_compact_policy". However, P3P did not take off,[25] most browsers have never fully implemented it, a lot of websites set this header with fake policy text, that was enough to fool browsers the existence of P3P policy and grant permissions for third party cookies.
     * example: P3P: CP="This is not a P3P policy! See http://www.google.com/support/accounts/bin/answer.py?hl=en&answer=151657 for more info."
     */
    std::string p3p_;
    /**
     * Implementation-specific headers that may have various effects anywhere along the request-response chain.
     * example: Pragma: no-cache
     */
    std::string pragma_;
    /**
     * Request authentication to access the proxy.
     * example: Proxy-Authenticate: Basic
     */
    std::string proxy_authenticate_;
    /**
     * Used in redirection, or when a new resource has been created. This refresh redirects after 5 seconds. This is a proprietary, non-standard header extension introduced by Netscape and supported by most web browsers.
     * example: Refresh: 5; url=http://www.w3.org/pub/WWW/People.html
     */
    std::string refresh_;
    /**
     * If an entity is temporarily unavailable, this instructs the client to try again after a specified period of time (seconds).
     * example: Retry-After: 120
     */
    std::string retry_after_;
    /**
     * A name for the server
     * example: Server: Apache/2.4.1 (Unix)
     */
    std::string server_;
    /**
     * an HTTP cookie
     * example: set_-Cookie: UserID=JohnDoe; Max-Age=3600; Version=1
     */
    std::string set_cookie_;
    /**
     * A HSTS Policy informing the HTTP client how long to cache the HTTPS only policy and whether this applies to subdomains.
     * example: Strict-Transport-Security: max-age=16070400; includeSubDomains
     */
    std::string strict_transport_security_;
    /**
     * The Trailer general field value indicates that the given set of header fields is present in the trailer of a message encoded with chunked transfer-coding.
     * example: Trailer: Max-Forwards
     */
    std::string trailer_;
    /**
     * The form of encoding used to safely transfer the entity to the user.Currently defined methods are: chunked, compress, deflate, gzip, identity.
     * example: Transfer-Encoding: chunked
     */
    std::string transfer_encoding_;
    /**
     * Tells downstream proxies how to match future request headers to decide whether the cached response can be used rather than requesting a fresh one from the origin server.
     * example: Vary: *
     */
    std::string vary_;
    /**
     * Informs the client of proxies through which the response was sent.
     * example: Via: 1.0 fred, 1.1 example.com (Apache/1.1)
     */
    std::string via_;
    /**
     * A general warning about possible problems with the entity body.
     * example: Warning: 199 Miscellaneous warning
     */
    std::string warning_;
    /**
     * Indicates the authentication scheme that should be used to access the requested entity.
     * example: WWW-Authenticate: Basic
     */
    std::string www_authenticate_;
    /**
     * Clickjacking protection: "deny" - no rendering within a frame, "sameorigin" - no rendering if origin mismatch
     * example: X-Frame-Options: deny
     */
    std::string x_frame_options_;
    /**
     * Cross-site scripting (XSS) filter
     * example: X-XSS-Protection: 1; mode=block
     */
    std::string x_xss_protection_;
    /**
     * The only defined value, "nosniff", prevents Internet Explorer from MIME-sniffing a response away from the declared content-type. This also applies to Google Chrome, when downloading extensions.[29]
     * example: X-Content-Type-Options: nosniff
     */
    std::string x_content_type_options_;
    /**
     * specifies the technology (e.g. ASP.NET, PHP, JBoss) supporting the web application (version details are often in X-Runtime, X-Version, or X-AspNet-Version)
     * example: X-Powered-By: PHP/5.4.0
     */
    std::string x_powered_by_;
    /**
     * Recommends the preferred rendering engine (often a backward-compatibility mode) to use to display the content. Also used to activate Chrome Frame in Internet Explorer.
     * example: X-UA-Compatible: IE=EmulateIE7 X-UA-Compatible: IE=edge X-UA-Compatible: Chrome=1
     */
    std::string x_ua_compatible_;


    /**
     *HTTP Response body.
     */
    std::string body_;
};

class url_buffer : public std::streambuf {
public:

    explicit url_buffer(std::size_t buff_sz = 256, std::size_t put_back = 8) :
    put_back_(std::max(put_back, size_t(1))),
    buffer_(std::max(buff_sz, put_back_) + put_back_) {
        char *end = &buffer_.front() + buffer_.size();
        setg(end, end, end);
    }

    virtual ~url_buffer() {

    }

    void set_socket(int socket, bool secure, SSL *ssl = NULL) {
        this->sockfd = socket;
        this->secure = secure;
        this->ssl = ssl;
    }

private:
    // overrides base class underflow()

    int_type underflow() {

        if (gptr() < egptr()) // buffer not exhausted
            return traits_type::to_int_type(*gptr());

        char *base = &buffer_.front();
        char *start = base;

        if (eback() == base) // true when this isn't the first fill
        {
            // Make arrangements for putback characters
            std::memmove(base, egptr() - put_back_, put_back_);
            start += put_back_;
        }

        // start is now the start of the buffer, proper.
        // Read from socket in to the provided buffer
        size_t n;
        if (secure) {
            n = SSL_read(ssl, start, buffer_.size() - (start - base));
        } else {
            n = read(sockfd, start, buffer_.size() - (start - base));
        }

        if (n == 0)
            return traits_type::eof();

        // set_ buffer pointers
        setg(base, start, start + n);

        return traits_type::to_int_type(*gptr());
    }
    //
    //    int_type overflow() {
    //    }
    // copy ctor and assignment not implemented;
    // copying not allowed
    url_buffer(const url_buffer &);
    url_buffer &operator=(const url_buffer &);

    bool secure;
    SSL *ssl;
private:
    int sockfd;
    const std::size_t put_back_;
    std::vector<char> buffer_;
};

template <class charT, class traits = std::char_traits<charT> >
class basic_urlstream : public std::iostream {
    int socket_good;
    std::string url, protocol_, host_, path_, query_;
    url_buffer rbuf;

    void parse(const std::string& url_s) {

        const std::string prot_end("://");
        std::string::const_iterator prot_i = std::search(url_s.begin(), url_s.end(),
                prot_end.begin(), prot_end.end());
        protocol_.reserve(std::distance(url_s.begin(), prot_i));
        std::transform(url_s.begin(), prot_i,
                std::back_inserter(protocol_),
                std::ptr_fun<int, int>(tolower)); // protocol is icase
        if (prot_i == url_s.end())
            return;
        std::advance(prot_i, prot_end.length());
        std::string::const_iterator path_i = std::find(prot_i, url_s.end(), '/');
        host_.reserve(std::distance(prot_i, path_i));
        std::transform(prot_i, path_i,
                std::back_inserter(host_),
                std::ptr_fun<int, int>(tolower)); // host is icase
        std::string::const_iterator query_i = std::find(path_i, url_s.end(), '?');
        path_.assign(path_i, query_i);
        if (query_i != url_s.end())
            ++query_i;
        query_.assign(query_i, url_s.end());
    }
    int is_open;
    int url_finished;

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    bool secure;
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *certbio;
    BIO *outbio;
    X509 *cert;
    X509_NAME *certname;
    const SSL_METHOD *method;


    /*---------------------------------------------------------------------*/
    /*--- InitCTX - initialize the SSL engine.                          ---*/

    /*---------------------------------------------------------------------*/
    SSL_CTX* init_ctx(void) {
        /* ---------------------------------------------------------- *
         * These function calls initialize openssl for correct work.  *
         * ---------------------------------------------------------- */
        OpenSSL_add_all_algorithms();
        ERR_load_BIO_strings();
        ERR_load_crypto_strings();
        SSL_load_error_strings();

        /* ---------------------------------------------------------- *
         * Create the Input/Output BIO's.                             *
         * ---------------------------------------------------------- */
        certbio = BIO_new(BIO_s_file());
        outbio = BIO_new_fp(stdout, BIO_NOCLOSE);

        /* ---------------------------------------------------------- *
         * initialize SSL library and register algorithms             *
         * ---------------------------------------------------------- */
        if (SSL_library_init() < 0)
            BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

        /* ---------------------------------------------------------- *
         * set_ SSLv2 client hello, also announce SSLv3 and TLSv1      *
         * ---------------------------------------------------------- */
        method = SSLv23_client_method();

        /* ---------------------------------------------------------- *
         * Try to create a new SSL context                            *
         * ---------------------------------------------------------- */
        if ((ctx = SSL_CTX_new((SSL_METHOD *) method)) == NULL)
            BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

        /* ---------------------------------------------------------- *
         * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
         * ---------------------------------------------------------- */
        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
        return ctx;
    }

    int open_connection(const char *hostname, int port) {
        int sd;
        struct hostent *host;
        struct sockaddr_in addr;

        if ((host = gethostbyname(hostname)) == NULL) {
            perror(hostname);
            abort();
        }
        sd = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof (addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = *(long*) (host->h_addr);
        if (connect(sd, (const struct sockaddr*) &addr, sizeof (addr)) != 0) {
            ::close(sd);
            perror(hostname);
            abort();
        }
        return sd;
    }

public:

    typedef charT char_type;
    typedef traits traits_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::off_type off_type;

    basic_urlstream() : std::iostream(&rbuf) {

    }

    basic_urlstream(const char* path) : std::iostream(&rbuf) {
        this->open(path);
    }
    basic_urlstream(basic_urlstream&& rhs);

    virtual ~basic_urlstream() {
        this->close();
    }

    void open(const char* url/*,  std::ios_base::openmode mode = std::ios_base::in*/) {

        ctx = NULL;
        ssl = NULL;
        certbio = NULL;
        outbio = NULL;
        cert = NULL;
        certname = NULL;
        method = NULL;

        this->url = url;
        this->parse(url);
        this->ssl = NULL;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
        hints.ai_socktype = SOCK_STREAM;
        secure = false;
        if (protocol_ == "https") {
            secure = true;
            sockfd = this->open_connection(host_.c_str(), 443);
            SSL_library_init();
            ctx = init_ctx();
        } else {
            sockfd = this->open_connection(host_.c_str(), 80);
        }

        if (secure) {
            ssl = SSL_new(ctx); /* create new SSL connection state */
            SSL_set_fd(ssl, sockfd); /* attach the socket descriptor */
            if (SSL_connect(ssl) == -1) {/* perform the connection */
                ERR_print_errors_fp(stderr);
                std::cout << "error!!!\n\n" << std::endl;
                exit(0);
            }

            /* ---------------------------------------------------------- *
             * get_ the remote certificate into the X509 structure         *
             * ---------------------------------------------------------- */
            cert = SSL_get_peer_certificate(ssl);
            if (cert == NULL)
                BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", this->url.c_str());

            /* ---------------------------------------------------------- *
             * extract various certificate information                    *
             * -----------------------------------------------------------*/
            certname = X509_NAME_new();
            certname = X509_get_subject_name(cert);

        }


        if (path_.size() > 0) {

            std::stringstream ss;
            ss << "GET http://" << host_;
            if (query_.size() != 0) {
                ss << path_ << "?" << query_ << "  HTTP/1.0 \r\n";
            } else {
                ss << path_ << "  HTTP/1.0 \r\n";
            }
            
            ss << "\r\n";
            http_request r;
            r.set_method(ss.str());
            std::string req = r.header_to_string();
            this->write(req.c_str(), req.size());
        }
        this->rbuf.set_socket(sockfd, secure, ssl);
    }

    int write(const char* buff, int size) {
        int n = 0;
        if (!secure) {
            n = ::write(sockfd, (void*) buff, size);
        } else {
            n = SSL_write(ssl, buff, size);
        }

        return n;
    }

    int read(char* buff, int size) {
        if (!secure) {
            return ::read(sockfd, (void*) buff, size);
        } else {
            return SSL_read(ssl, buff, size);
        }
    }

    void close() {
        ::close(sockfd);
    }

    basic_urlstream& operator<<(const std::string& val) {
        int n = this->write(val.c_str(), val.size());
    }

    /**
     * Decodes a Uniform Resource Identifier (URI) string.
     * 
     * @param sSrc
     * @return 
     */
    static std::string uri_decode(const std::string & src) {
        // Note from RFC1630:  "Sequences which start with a percent sign
        // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
        // for future extension"

        const unsigned char * pSrc = (const unsigned char *) src.c_str();
        const int SRC_LEN = src.length();
        const unsigned char * const SRC_END = pSrc + SRC_LEN;
        const unsigned char * const SRC_LAST_DEC = SRC_END - 2; // last decodable '%' 

        char * const pStart = new char[SRC_LEN];
        char * pEnd = pStart;

        while (pSrc < SRC_LAST_DEC) {
            if (*pSrc == '%') {
                char dec1, dec2;
                if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                        && -1 != (dec2 = HEX2DEC[*(pSrc + 2)])) {
                    *pEnd++ = (dec1 << 4) + dec2;
                    pSrc += 3;
                    continue;
                }
            }

            *pEnd++ = *pSrc++;
        }

        // the last 2- chars
        while (pSrc < SRC_END)
            *pEnd++ = *pSrc++;

        std::string sResult(pStart, pEnd);
        delete [] pStart;
        return sResult;
    }

    /**
     * Encodes a Uniform Resource Identifier (URI) by replacing each instance 
     * of certain characters by one, two, three, or four escape sequences 
     * representing the UTF-8 encoding of the character (will only be four 
     * escape sequences for characters composed of two "surrogate" characters).
     * 
     * @param src
     * @return 
     */
    static std::string uri_encode(const std::string & src) {
        const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
        const unsigned char * pSrc = (const unsigned char *) src.c_str();
        const int SRC_LEN = src.length();
        unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
        unsigned char * pEnd = pStart;
        const unsigned char * const SRC_END = pSrc + SRC_LEN;

        for (; pSrc < SRC_END; ++pSrc) {
            if (SAFE[*pSrc])
                *pEnd++ = *pSrc;
            else {
                // escape this char
                *pEnd++ = '%';
                *pEnd++ = DEC2HEX[*pSrc >> 4];
                *pEnd++ = DEC2HEX[*pSrc & 0x0F];
            }
        }

        std::string sResult((char *) pStart, (char *) pEnd);
        delete [] pStart;
        return sResult;
    }

protected:


};

typedef basic_urlstream<char> urlstream;
typedef basic_urlstream<wchar_t> wurlstream;

urlstream& operator<<(urlstream& out, http_request& request) {
    out << request.header_to_string();
    if (request.get_body() != "undefined")
        out << request.get_body();
    return out;
}

urlstream& operator>>(urlstream& in, http_response& response) {

    std::string line;
    std::stringstream ss;
    while (in.good()) {
        std::getline(in, line);
        ss << line << "\n";
    }

    response.set_data(ss.str());
    
    std::string r = ss.str();
    response.header_from_string(r);

    size_t content_length =
            string_util::to_number<size_t>(response.get_content_length());
    
    if (content_length) {
        response.set_body(r.substr(r.size() - content_length - 1, r.size()));
    }
    return in;
}



#endif	/* URLSTREAM_HPP */

