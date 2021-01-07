// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#pragma once

#include <boost/beast/version.hpp>
#include <optional>
#include <string>
#include <utility>
#include <web_crawler_lib/definitions.hpp>

namespace web_crawler_lib {

    /// Response type.
    using http_response_t = http::response<http::string_body>;

    /**
     * @brief Gets the parts of the given URL, i.e. host and target.
     * @param url HTTP-URL whose parts should be resolved
     * @return pair of host and target or an empty optional if the URL was incorrect
     */
    ::std::optional<::std::pair<::std::string, ::std::string>> parse_url_parts(std::string const& url);

    [[nodiscard]] ::std::optional<http_response_t> read_from_url(::std::string const& host, ::std::string const& port,
                                                                 ::std::string const& target);
} // namespace web_crawler_lib
