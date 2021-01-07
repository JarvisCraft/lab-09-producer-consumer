// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <cstddef>

namespace web_crawler_lib {

    namespace asio = ::boost::asio;
    using tcp = asio::ip::tcp;
    namespace beast = ::boost::beast;
    namespace http = beast::http;
} // namespace web_crawler_lib
