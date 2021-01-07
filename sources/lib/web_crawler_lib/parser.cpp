// Copyright 2020 Petr Portnov me@progrm-jarvis.ru

#include <gumbo.h>

#include <web_crawler_lib/parser.hpp>

namespace web_crawler_lib {

    static ::std::set<::std::string> find_elements_recursive_(GumboNode const* node /* raw non-owned pointer */,
                                                              GumboTag tag, char const* name);

    struct GuardedGumboOutput {
        //<editor-fold desc="RAII-guard implementation" defaultstate="collapsed">
        GumboOutput* const guarded;

        ~GuardedGumboOutput() {
            if (guarded) gumbo_destroy_output(&kGumboDefaultOptions, guarded);
        }

        typename ::std::add_lvalue_reference<GumboOutput>::type operator*() const { return *guarded; }

        GumboOutput* operator->() const noexcept { return guarded; }
        //</editor-fold>
    };

    ParsingResult parse_http_response(const http_response_t& response, bool const parse_children) {
        // ensure RAII-safety for raw pointer provided by a C-only library
        GuardedGumboOutput parsed{gumbo_parse(response.body().c_str())};

        return ParsingResult{
            find_elements_recursive_(parsed->root, GUMBO_TAG_IMG, "src"),
            parse_children ? find_elements_recursive_(parsed->root, GUMBO_TAG_A, "href") : ::std::set<::std::string>{}};
    }

    ::std::set<::std::string> find_elements_recursive_(GumboNode const* const node, GumboTag const tag,
                                                       char const* const name) {
        if (node->type != GUMBO_NODE_ELEMENT) return {};

        ::std::set<::std::string> references;
        if (node->v.element.tag == tag) {
            auto const href_tag = gumbo_get_attribute(&node->v.element.attributes, name);
            if (href_tag) references.emplace(href_tag->value); // deep-copy constructor
        }

        {
            auto children = &node->v.element.children;
            auto const length = children->length;
            for (::std::size_t i = 0; i < length; ++i) {
                auto const child_references
                    = find_elements_recursive_(static_cast<GumboNode const*>(children->data[i]), tag, name);
                references.insert(child_references.begin(), child_references.end());
            }
        }

        return references;
    }
    bool ParsingResult::operator==(ParsingResult const& other) const {
        return this == &other || (image_urls == other.image_urls && child_urls == other.child_urls);
    }

    ::std::ostream& operator<<(std::ostream& out, ParsingResult const& result) {
        out << "ParsingResult{image_urls={";
        {
            bool not_first = false;
            for (auto const& image_url : result.image_urls) {
                if (not_first) {
                    out << ", ";
                } else {
                    not_first = true;
                }
                out << "\"" << image_url << "\"";
            }
        }
        out << "}, child_urls={";
        {
            bool not_first = false;
            for (auto const& child_url : result.child_urls) {
                if (not_first) {
                    out << ", ";
                } else {
                    not_first = true;
                }
                out << "\"" << child_url << "\"";
            }
        }
        return out << "}}";
    }
} // namespace web_crawler_lib