// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <web_crawler_lib/web_crawler.hpp>

namespace web_crawler_lib {

    WebCrawler::WebCrawler(::std::size_t const max_depth, ::std::string const& root_url,
                           ::std::size_t const network_workers, ::std::size_t const parser_workers,
                           ::std::string const& output_file_name)
        : max_depth_{max_depth},
          url_reader_{},
          parser_{},
          output_{output_file_name},
          network_workers_{network_workers},
          parser_workers_{parser_workers},
          writer_worker_{[this] {
              while (active_jobs_) { writer_worker(); }
          }} {
        publish_network_job({root_url, 0});
    }

    void WebCrawler::join() {
        {
            ::std::unique_lock lock{active_jobs_mutex_};
            active_jobs_cv_.wait(lock, [this] { return active_jobs_ == 0; });
        }

        // the following joins will most probably return immediately but it is an extra safety guarantee
        writer_worker_.join();
        network_workers_.join();
        parser_workers_.join();
    }
    WebCrawler WebCrawler::from(std::string const& url, ::std::size_t depth, ::std::size_t network_threads,
                                ::std::size_t parser_threads, std::string const& output_file_name) {
        return WebCrawler{depth, url, network_threads == 0 ? ::std::thread::hardware_concurrency() : network_threads,
                          parser_threads == 0 ? ::std::thread::hardware_concurrency() : parser_threads,
                          output_file_name};
    }

    void WebCrawler::unsafe_write_to_output(std::string const& value) { output_ << value << ::std::endl; }

    void WebCrawler::writer_worker() {
        ::std::unique_lock lock{writer_queue_mutex_};
        writer_queue_cv_.wait(lock, [this] { return !writer_queue_.empty(); });
        // get data under lock
        auto const result = writer_queue_.front();
        writer_queue_.pop();
        lock.unlock();
        // don't write under lock as there should only be single writer
        unsafe_write_to_output(result);

        notify_finish_job_();
    }

    void WebCrawler::publish_network_job(NetworkJob&& job) {
        notify_start_job_();

        asio::post(network_workers_, [this, job] {
            auto const url_parts = parse_url_parts(job.url);
            if (url_parts) {
                auto response = url_reader_.read(url_parts->first, "80" /* default port */, url_parts->second);
                if (response) publish_parser_job({::std::move(*response), job.depth});
            }

            notify_finish_job_();
        });
    }

    void WebCrawler::publish_parser_job(ParserJob&& job) {
        notify_start_job_();

        asio::post(parser_workers_, [this, job] {
            auto const children = job.depth < max_depth_;
            auto result = parser_.parse(job.response, children);
            for (auto image_url : result.image_urls) publish_writer_job(::std::move(image_url));
            if (children) {
                auto const next_depth = job.depth + 1;
                for (auto child_url : result.child_urls) publish_network_job({::std::move(child_url), next_depth});
            }

            notify_finish_job_();
        });
    }

    void WebCrawler::publish_writer_job(::std::string&& job) {
        notify_start_job_();
        ::std::lock_guard lock{writer_queue_mutex_};
        writer_queue_.push(::std::move(job));
        writer_queue_cv_.notify_one();
    }

    void WebCrawler::notify_start_job_() {
        ::std::lock_guard lock{active_jobs_mutex_};
        ++active_jobs_;
        active_jobs_cv_.notify_all();
    }

    void WebCrawler::notify_finish_job_() {
        ::std::lock_guard lock{active_jobs_mutex_};
        --active_jobs_;
        active_jobs_cv_.notify_all();
    }
} // namespace web_crawler_lib