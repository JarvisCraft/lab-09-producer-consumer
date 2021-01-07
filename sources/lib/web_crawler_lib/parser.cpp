// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <gumbo.h>

#include <memory>
#include <web_crawler_lib/parser.hpp>

namespace web_crawler_lib {

    static ::std::vector<::std::string> find_elements_recursive_(GumboNode const* node /* raw non-owned pointer */,
                                                                 GumboTag tag, char const* name);

    ParsingResult WebPageParser::parse(const UrlReader::response_t& response, bool const parse_children) {
        // ensure RAII-safety for raw pointer provided by a C-only library
        ::std::unique_ptr<GumboOutput> parsed{gumbo_parse(response.body().c_str())};

        return ParsingResult{find_elements_recursive_(parsed->root, GUMBO_TAG_IMG, "src"),
                             parse_children ? find_elements_recursive_(parsed->root, GUMBO_TAG_A, "href")
                                            : ::std::vector<::std::string>{}};
    }

    ::std::vector<::std::string> find_elements_recursive_(GumboNode const* const node, GumboTag const tag,
                                                          char const* const name) {
        if (node->type != GUMBO_NODE_ELEMENT) return {};

        ::std::vector<::std::string> references;
        if (node->v.element.tag == tag) {
            auto const href_tag = gumbo_get_attribute(&node->v.element.attributes, name);
            if (href_tag) references.emplace_back(href_tag->value); // deep-copy constructor
        }

        {
            auto children = &node->v.element.children;
            auto const length = children->length;
            for (::std::size_t i = 0; i < length; ++i) {
                auto const child_references
                    = find_elements_recursive_(static_cast<GumboNode const*>(children->data[i]), tag, name);
                references.insert(references.end(), child_references.begin(), child_references.end());
            }
        }

        return references;
    }
} // namespace web_crawler_lib