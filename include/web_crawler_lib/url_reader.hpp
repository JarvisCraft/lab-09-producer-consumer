// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#pragma once

#include <boost/beast/version.hpp>
#include <optional>
#include <string>
#include <utility>
#include <web_crawler_lib/definitions.hpp>

namespace web_crawler_lib {

    /**
     * @brief Gets the parts of the given URL, i.e. host and target.
     * @param url HTTP-URL whose parts should be resolved
     * @return pair of host and target or an empty optional if the URL was incorrect
     */
    ::std::optional<::std::pair<::std::string, ::std::string>> parse_url_parts(std::string const& url);

    /**
     * @brief Service responsible for reading data via HTTP.
     */
    class UrlReader {
    public:
        /// Response type.
        using response_t = http::response<http::string_body>;

        [[nodiscard]] ::std::optional<response_t> read(::std::string const& host, ::std::string const& port,
                                                       ::std::string const& target) const;
    };
} // namespace web_crawler_lib
