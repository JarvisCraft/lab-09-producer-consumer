// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#ifndef INCLUDE_WEB_CRAWLER_HPP_
#define INCLUDE_WEB_CRAWLER_HPP_

#include <string>
namespace web_crawler_lib {

    class WebCrawler {
    public:

        void join();

        static WebCrawler from(::std::string const& url,
                               ::std::size_t depth, ::std::size_t network_threads, ::std::size_t parser_threads,
                               ::std::string const& output_file_name);
    };
}

#endif // INCLUDE_WEB_CRAWLER_HPP_
