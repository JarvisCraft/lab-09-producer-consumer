// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#pragma once

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <web_crawler_lib/parser.hpp>
#include <web_crawler_lib/url_reader.hpp>

namespace web_crawler_lib {

    class WebCrawler {
    public:
        /**
         * @brief Awaits for this crawler to end its job.
         */
        void join();

        /**
         * @brief Creates a new web-crawler using the given parameters.
         *
         * @param url root url
         * @param depth depth of search
         * @param network_threads number of threads to use for network
         * @param parser_threads number of threads to parse data
         * @param output_file_name name of the output file
         * @return created web-crawler
         */
        static WebCrawler from(::std::string const& url, ::std::size_t depth, ::std::size_t network_threads,
                               ::std::size_t parser_threads, ::std::string const& output_file_name);

    private:
        // Generic configuration
        const ::std::size_t max_depth_;

        // Services
        UrlReader url_reader_;
        WebPageParser parser_;
        ::std::ofstream output_;

        // Threads
        asio::thread_pool network_workers_;
        asio::thread_pool parser_workers_;
        ::std::thread writer_worker_;

        // FIXME
        ::std::atomic_size_t active_jobs_ = 1;

        struct NetworkJob {
            ::std::string url;
            ::std::size_t depth;
        };

        ::std::queue<NetworkJob> network_queue_;
        mutable ::std::mutex network_queue_mutex_;
        ::std::condition_variable network_queue_cv_;

        struct ParserJob {
            UrlReader::response_t response;
            ::std::size_t depth;
        };

        ::std::queue<ParserJob> parser_queue_;
        mutable ::std::mutex parser_queue_mutex_;
        ::std::condition_variable parser_queue_cv_;

        ::std::queue<::std::string> writer_queue_;
        mutable ::std::mutex writer_queue_mutex_;
        ::std::condition_variable writer_queue_cv_;

        // default-initializes members yet not starting anything
        WebCrawler(::std::size_t max_depth, ::std::string const& root_url, ::std::size_t const network_workers,
                   ::std::size_t const parser_workers, ::std::string const& output_file_name)
            : max_depth_{max_depth},
              url_reader_{},
              parser_{},
              output_{output_file_name},
              network_workers_{network_workers},
              parser_workers_{parser_workers},
              writer_worker_{[this] {
                  while (active_jobs_) { writer_worker(); }
              }} {
            for (size_t i = 0; i < network_workers; ++i) asio::post(network_workers_, [this] { network_worker(); });
            for (size_t i = 0; i < parser_workers; ++i) asio::post(parser_workers_, [this] { parser_worker(); });
            publish_network_job({root_url, 0});
        }

        // Service users

        /**
         * @brief Unsafely writes the given string to the output. This does not provide any concurrency guarantees.
         * @param value value to be written into the output
         */
        void unsafe_write_to_output(::std::string const& value);

        void publish_network_job(NetworkJob&& job);

        void publish_parser_job(ParserJob&& job);

        void publish_writer_job(::std::string&& job);

        // Worker bodies

        void network_worker();

        void parser_worker();

        // single worker only
        void writer_worker();
    };
} // namespace web_crawler_lib
