// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <gtest/gtest.h>

#include <web_crawler_lib/url_reader.hpp>

namespace web_crawler_lib {

    TEST(url_reader, parse_url_parts) {
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("http://github.com/"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/JarvisCraft"))),
                  parse_url_parts("http://github.com/JarvisCraft"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("http://github.com"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("example.com"), ::std::string("/foo/bar/baz"))),
                  parse_url_parts("http://example.com/foo/bar/baz"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("github.com/"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/JarvisCraft"))),
                  parse_url_parts("github.com/JarvisCraft"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("github.com"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("example.com"), ::std::string("/foo/bar/baz"))),
                  parse_url_parts("example.com/foo/bar/baz"));
        EXPECT_FALSE(parse_url_parts("foo://yandex.ru/"));
        EXPECT_FALSE(parse_url_parts("https://yandex.ru/")); // sry
    }
} // namespace web_crawler_lib
