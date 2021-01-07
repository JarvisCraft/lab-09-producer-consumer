// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <web_crawler_lib/url_reader.hpp>

namespace web_crawler_lib {

    ::std::optional<::std::pair<::std::string, ::std::string>> parse_url_parts(std::string const& url) {
        size_t begin_offset;
        { // in case of protocol being specified check if it is `http`
            auto const protocol_delimiter = url.find("://");
            if (protocol_delimiter != ::std::string::npos) {
                // http://
                // 0123456
                if (protocol_delimiter != 4 || url.substr(0, 4) != "http") return ::std::nullopt;
                begin_offset = 7;
            } else {
                begin_offset = 0;
            }
        }

        auto const path_delimiter_index = url.find('/', begin_offset);
        return path_delimiter_index == ::std::string::npos
               ? ::std::make_pair(url.substr(begin_offset), "/")
               : ::std::make_pair(url.substr(begin_offset, path_delimiter_index - begin_offset),
                                  url.substr(path_delimiter_index));
    }

    std::optional<http_response_t> read_from_url(std::string const& host, std::string const& port,
                                                 std::string const& target) {
        try {
            asio::io_context context;
            beast::tcp_stream stream{context};
            stream.connect(tcp::resolver{context}.resolve(host, port));

            {
                http::request<http::string_body> request{http::verb::get, target, 11};
                request.set(http::field::host, host);
                request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                http::write(stream, request);
            }

            beast::flat_buffer buffer;
            http_response_t response;
            http::read(stream, buffer, response);

            return response;
        } catch (boost::system::system_error const& error) { return ::std::nullopt; }
    }
} // namespace web_crawler_lib