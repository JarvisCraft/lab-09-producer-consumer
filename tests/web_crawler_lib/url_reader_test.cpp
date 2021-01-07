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
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("github.com/"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/JarvisCraft"))),
                  parse_url_parts("github.com/JarvisCraft"));
        EXPECT_EQ(::std::make_optional(::std::make_pair(::std::string("github.com"), ::std::string("/"))),
                  parse_url_parts("github.com"));
    }
} // namespace web_crawler_lib

::std::ostream & operator<<(::std::ostream &out, ::std::optional<::std::pair<::std::string, ::std::string>> x) {
    if (x) {
        return out << "{\"" << x->first << "\", \"" << x->second << "\"}";
    } else {
        return  out << "{}";
    }
}