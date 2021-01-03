// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#ifndef INCLUDE_DEFINITIONS_HPP_
#define INCLUDE_DEFINITIONS_HPP_

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace web_crawler_lib {

    namespace asio = ::boost::asio;
    using tcp = asio::ip::tcp;
    namespace beast = ::boost::beast;
    namespace http = beast::http;
} // namespace web_crawler_lib

#endif // INCLUDE_DEFINITIONS_HPP_
