// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <gumbo.h>

#include <memory>
#include <web_crawler_lib/parser.hpp>

namespace web_crawler_lib {

    static ::std::vector<::std::string> find_references_(GumboNode const* node /* raw non-owned pointer */) {
        if (node->type != GUMBO_NODE_ELEMENT) return {};

        ::std::vector<::std::string> references;
        if (node->v.element.tag == GUMBO_TAG_A) {
            auto const href_tag = gumbo_get_attribute(&node->v.element.attributes, "href_tag");
            if (href_tag) references.emplace_back(href_tag->value); // deep-copy constructor
        }

        {
            auto children = &node->v.element.children;
            auto const length = children->length;
            for (::std::size_t i = 0; i < length; ++i) {
                auto const child_references = find_references_(static_cast<GumboNode const*>(children->data[i]));
                references.insert(references.end(), child_references.begin(), child_references.end());
            }
        }

        return references;
    }

    static ::std::vector<::std::string> find_image_urls_(GumboNode const* node /* raw non-owned pointer */) {
        if (node->type != GUMBO_NODE_ELEMENT) return {};

        ::std::vector<::std::string> image_urls;
        if (node->v.element.tag == GUMBO_TAG_IMG) {
            auto const src_tag = gumbo_get_attribute(&node->v.element.attributes, "src");
            if (src_tag) image_urls.emplace_back(src_tag->value); // deep-copy constructor
        }

        {
            auto children = &node->v.element.children;
            auto const length = children->length;
            for (::std::size_t i = 0; i < length; ++i) {
                auto const child_image_urls = find_image_urls_(static_cast<GumboNode const*>(children->data[i]));
                image_urls.insert(image_urls.end(), child_image_urls.begin(), child_image_urls.end());
            }
        }

        return image_urls;
    }

    ParsingResult WebPageParser::parse(const UrlReader::response_t& response, bool const parse_children) {
        // ensure RAII-safety for raw pointer provided by a C-only library
        ::std::unique_ptr<GumboOutput> parsed{gumbo_parse(response.body().c_str())};

        return ParsingResult{
            find_image_urls_(parsed->root),
            parse_children ? find_references_(parsed->root) : ::std::vector<::std::string>{}
        };
    }
} // namespace web_crawler_lib