// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <gtest/gtest.h>

#include <web_crawler_lib/parser.hpp>

namespace web_crawler_lib {

    [[nodiscard]] static http_response_t ok_http_response_(::std::string &&body);

    TEST(parser, parse_http_response) {
        EXPECT_EQ((ParsingResult{{}, {"example.com"}}),
                  parse_http_response(ok_http_response_("<html><a href='example.com'/></html>"), true));
        EXPECT_EQ((ParsingResult{{}, {}}),
                  parse_http_response(ok_http_response_("<html><a href='example.com'/></html>"), false));

        EXPECT_EQ((ParsingResult{{}, {"example.com"}}),
                  parse_http_response(ok_http_response_("<html><b><i><a href='example.com'/></i></b></html>"), true));
        EXPECT_EQ((ParsingResult{{}, {}}),
                  parse_http_response(ok_http_response_("<html><b><i><a href='example.com'/></i></b></html>"), false));
        EXPECT_EQ((ParsingResult{{"picture.png"}, {}}),
                  parse_http_response(ok_http_response_("<html><img src='picture.png'></html>"), true));
        EXPECT_EQ((ParsingResult{{"picture.png"}, {}}),
                  parse_http_response(ok_http_response_("<html><img src='picture.png'></html>"), false));

        EXPECT_EQ((ParsingResult{{"beautiful.png"}, {"progrm-jarvis.ru"}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<a href='progrm-jarvis.ru'/>"
                                                        "<img src='beautiful.png'>"
                                                        "</html>"),
                                      true));
        EXPECT_EQ((ParsingResult{{"beautiful.png"}, {}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<a href='progrm-jarvis.ru'/>"
                                                        "<img src='beautiful.png'>"
                                                        "</html>"),
                                      false));

        EXPECT_EQ((ParsingResult{{"first.png", "second.png", "third.png"}, {"https://oma.gad/"}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<img src='first.png'>"
                                                        "<a href='https://oma.gad/'/>"
                                                        "<img src='second.png'>"
                                                        "<img src='third.png'>"
                                                        "</html>"),
                                      true));
        EXPECT_EQ((ParsingResult{{"first.png", "second.png", "third.png"}, {}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<img src='first.png'>"
                                                        "<a href='https://oma.gad/'/>"
                                                        "<img src='second.png'>"
                                                        "<img src='third.png'>"
                                                        "</html>"),
                                      false));

        EXPECT_EQ((ParsingResult{{"some.jpg", "thing.gif"}, {"nohello.com", "yeshello.xyz"}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<img src='some.jpg'>"
                                                        "<b><a href='nohello.com'/></b>"
                                                        "<i><img src='thing.gif'></i>"
                                                        "<a href='yeshello.xyz'/>"
                                                        "</html>"),
                                      true));
        EXPECT_EQ((ParsingResult{{"some.jpg", "thing.gif"}, {}}),
                  parse_http_response(ok_http_response_("<html>"
                                                        "<img src='some.jpg'>"
                                                        "<b><a href='nohello.com'/></b>"
                                                        "<i><img src='thing.gif'></i>"
                                                        "<a href='yeshello.xyz'/>"
                                                        "</html>"),
                                      false));
    }

    http_response_t ok_http_response_(std::string &&body) { return {http::status::ok, 11, ::std::move(body)}; }
} // namespace web_crawler_lib
