// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <web_crawler_lib/web_crawler.hpp>

namespace web_crawler_lib {

    void WebCrawler::join() {
        network_workers_.join();
        parser_workers_.join();
        writer_worker_.join();
    }

    WebCrawler WebCrawler::from(std::string const& url, ::std::size_t depth, ::std::size_t network_threads,
                                ::std::size_t parser_threads, std::string const& output_file_name) {
        return WebCrawler{depth, url, network_threads == 0 ? ::std::thread::hardware_concurrency() : network_threads,
                          parser_threads == 0 ? ::std::thread::hardware_concurrency() : parser_threads,
                          output_file_name};
    }
    void WebCrawler::unsafe_write_to_output(std::string const& value) { output_ << value << ::std::endl; }

    void WebCrawler::network_worker() {
        ::std::unique_lock url_queue_lock{network_queue_mutex_};
        network_queue_cv_.wait(url_queue_lock, [this] { return !network_queue_.empty(); });
        // get next job under lock
        auto const job = network_queue_.front();
        network_queue_.pop();
        url_queue_lock.unlock();

        // Thread-safe logic
        auto const url_parts = parse_url_parts(job.url);
        if (url_parts) {
            auto response = url_reader_.read(url_parts->first, "80" /* default port */, url_parts->second);
            if (response) publish_parser_job({::std::move(*response), job.depth});
        }
    }

    void WebCrawler::parser_worker() {
        ::std::unique_lock parser_queue_lock{parser_queue_mutex_};
        parser_queue_cv_.wait(parser_queue_lock, [this] { return !parser_queue_.empty(); });
        // get next job under lock
        auto const job = parser_queue_.front();
        parser_queue_.pop();
        parser_queue_lock.unlock();

        auto const children = job.depth < max_depth_;
        auto result = parser_.parse(job.response, children);
        for (auto image_url : result.image_urls) publish_writer_job(::std::move(image_url));
        if (children) {
            auto const next_depth = job.depth + 1;
            for (auto child_url : result.child_urls) publish_network_job({::std::move(child_url), next_depth});
        }
    }

    void WebCrawler::writer_worker() {
        ::std::unique_lock lock{writer_queue_mutex_};
        writer_queue_cv_.wait(lock, [this] { return !writer_queue_.empty(); });
        // get data under lock
        auto const result = writer_queue_.front();
        writer_queue_.pop();
        lock.unlock();
        // don't write under lock as there should only be single writer
        unsafe_write_to_output(result);
    }

    void WebCrawler::publish_network_job(NetworkJob&& job) {
        ::std::lock_guard lock{network_queue_mutex_};
        network_queue_.push(::std::move(job));
        network_queue_cv_.notify_one();
    }

    void WebCrawler::publish_parser_job(ParserJob&& job) {
        ::std::lock_guard lock{parser_queue_mutex_};
        parser_queue_.push(::std::move(job));
        parser_queue_cv_.notify_one();
    }

    void WebCrawler::publish_writer_job(::std::string&& job) {
        ::std::lock_guard lock{writer_queue_mutex_};
        writer_queue_.push(::std::move(job));
        writer_queue_cv_.notify_one();
    }
} // namespace web_crawler_lib