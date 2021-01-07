// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#pragma once

#include <string>
#include <vector>
#include <web_crawler_lib/parser.hpp>
#include <web_crawler_lib/url_reader.hpp>

namespace web_crawler_lib {

    /**
     * @brief Result of parsing url-reader's response.
     */
    struct ParsingResult {
        /// URLs to the images
        ::std::vector<::std::string> image_urls;
        /// URLs to children
        ::std::vector<::std::string> child_urls; // may be empty
    };

    class WebPageParser final {
    public:
        /**
         * @brief Parses the given response.
         * @param response response which should be parsed
         * @return result of parsing
         */
        [[nodiscard]] ParsingResult parse(UrlReader::response_t const& response, bool parse_children);
    };
} // namespace web_crawler_lib
