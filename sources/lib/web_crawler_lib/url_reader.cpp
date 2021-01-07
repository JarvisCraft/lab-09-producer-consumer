// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <web_crawler_lib/url_reader.hpp>
#include <regex>

namespace web_crawler_lib {

    ::std::regex const URL_PARTS_REGULAR_EXPRESSION_{R"(^(?:http\:\/\/)?([^\/]+)(\/.*)$)"};

    std::optional<UrlReader::response_t> UrlReader::read(std::string const& host, std::string const& port,
                                                         std::string const& target) const {
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
            response_t response;
            http::read(stream, buffer, response);

            return response;
        } catch (boost::system::system_error const& error) { return ::std::nullopt; }
    }

    ::std::optional<::std::pair<::std::string, ::std::string>> parse_url_parts(std::string const& url) {
        ::std::smatch matches;

        if (::std::regex_search(url, matches, URL_PARTS_REGULAR_EXPRESSION_)) {
            auto const& target = matches[2];
            return ::std::make_pair(matches[1], target.length() == 0 ? ::std::string("/") : target.str());
        }
        return ::std::nullopt;
    }
} // namespace web_crawler_lib