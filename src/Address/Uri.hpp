/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 *
 * This file is part of libsmart/Stm32NetX, which is distributed under the terms
 * of the BSD 3-Clause License. You should have received a copy of the BSD 3-Clause
 * License along with libsmart/Stm32NetX. If not, see <https://spdx.org/licenses/BSD-3-Clause.html>.
 *
 * ----------------------------------------------------------------------------
 * Portions of the code are derived from Ben Lewis's work,
 * which is licensed under the MIT License. You can find the original work at:
 * <https://github.com/ben-zen/uri-library>
 * ----------------------------------------------------------------------------
 */


#pragma once
#include <cctype>
#include <map>
#include <string>
#include <stdexcept>
#include <utility>

namespace Stm32NetX {
    class Uri {
        /* URIs are broadly divided into two categories: hierarchical and
         * non-hierarchical. Both hierarchical URIs and non-hierarchical URIs have a
         * few elements in common; all URIs have a scheme of one or more alphanumeric
         * characters followed by a colon, and they all may optionally have a query
         * component preceded by a question mark, and a fragment component preceded by
         * an octothorpe (hash mark: '#'). The query consists of stanzas separated by
         * either ampersands ('&') or semicolons (';') (but only one or the other),
         * and each stanza consists of a key and an optional value; if the value
         * exists, the key and value must be divided by an equals sign.
         *
         * The following is an example from Wikipedia of a hierarchical URI:
         * scheme:[//[user:password@]domain[:port]][/]path[?query][#fragment]
         */

    public:
        enum class scheme_category {
            Hierarchical,
            NonHierarchical
        };

        enum class component {
            Scheme,
            Content,
            Username,
            Password,
            Host,
            Port,
            Path,
            Query,
            Fragment
        };

        enum class query_argument_separator {
            ampersand,
            semicolon
        };

        Uri(char const *uri_text, scheme_category category = scheme_category::Hierarchical,
            query_argument_separator separator = query_argument_separator::ampersand) : m_category(category),
            m_port(0),
            m_path_is_rooted(false),
            m_separator(separator) {
            setup(std::string(uri_text), category);
        };

        Uri(std::string const &uri_text, scheme_category category = scheme_category::Hierarchical,
            query_argument_separator separator = query_argument_separator::ampersand) : m_category(category),
            m_port(0),
            m_path_is_rooted(false),
            m_separator(separator) {
            setup(uri_text, category);
        };

        Uri(std::map<component, std::string> const &components,
            scheme_category category,
            bool rooted_path,
            query_argument_separator separator = query_argument_separator::ampersand) : m_category(category),
            m_path_is_rooted(rooted_path),
            m_separator(separator) {
            if (components.count(component::Scheme)) {
                if (components.at(component::Scheme).empty()) {
                    throw std::invalid_argument("Scheme cannot be empty.");
                }
                m_scheme = components.at(component::Scheme);
            } else {
                throw std::invalid_argument("A URI must have a scheme.");
            }

            if (category == scheme_category::Hierarchical) {
                if (components.count(component::Content)) {
                    throw std::invalid_argument("The content component is only for use in non-hierarchical URIs.");
                }

                bool has_username = components.count(component::Username);
                bool has_password = components.count(component::Password);
                if (has_username && has_password) {
                    m_username = components.at(component::Username);
                    m_password = components.at(component::Password);
                } else if ((has_username && !has_password) || (!has_username && has_password)) {
                    throw std::invalid_argument("If a username or password is supplied, both must be provided.");
                }

                if (components.count(component::Host)) {
                    m_host = components.at(component::Host);
                }

                if (components.count(component::Port)) {
                    m_port = std::stoul(components.at(component::Port));
                }

                if (components.count(component::Path)) {
                    m_path = components.at(component::Path);
                } else {
                    throw std::invalid_argument("A path is required on a hierarchical URI, even an empty path.");
                }
            } else {
                if (components.count(component::Username)
                    || components.count(component::Password)
                    || components.count(component::Host)
                    || components.count(component::Port)
                    || components.count(component::Path)) {
                    throw std::invalid_argument(
                        "None of the hierarchical components are allowed in a non-hierarchical URI.");
                }

                if (components.count(component::Content)) {
                    m_content = components.at(component::Content);
                } else {
                    throw std::invalid_argument(
                        "Content is a required component for a non-hierarchical URI, even an empty string.");
                }
            }

            if (components.count(component::Query)) {
                m_query = components.at(component::Query);
            }

            if (components.count(component::Fragment)) {
                m_fragment = components.at(component::Fragment);
            }
        }

        Uri(Uri const &other, std::map<component, std::string> const &replacements) : m_category(other.m_category),
            m_path_is_rooted(other.m_path_is_rooted),
            m_separator(other.m_separator) {
            m_scheme = (replacements.count(component::Scheme))
                           ? replacements.at(component::Scheme)
                           : other.m_scheme;

            m_username = (replacements.count(component::Username))
                             ? replacements.at(component::Username)
                             : other.m_username;

            m_password = (replacements.count(component::Password))
                             ? replacements.at(component::Password)
                             : other.m_password;

            m_host = (replacements.count(component::Host))
                         ? replacements.at(component::Host)
                         : other.m_host;

            m_port = (replacements.count(component::Port))
                         ? std::stoul(replacements.at(component::Port))
                         : other.m_port;

            m_path = (replacements.count(component::Path))
                         ? replacements.at(component::Path)
                         : other.m_path;

            m_content = (replacements.count(component::Content))
                            ? replacements.at(component::Content)
                            : other.m_content;

            m_query = (replacements.count(component::Query))
                          ? replacements.at(component::Query)
                          : other.m_query;

            m_fragment = (replacements.count(component::Fragment))
                             ? replacements.at(component::Fragment)
                             : other.m_fragment;
        }

        // Copy constructor; just use the copy assignment operator internally.
        Uri(Uri const &other) {
            *this = other;
        };

        // Copy assignment operator
        Uri &operator=(Uri const &other) {
            if (this != &other) {
                m_scheme = other.m_scheme;
                m_content = other.m_content;
                m_username = other.m_username;
                m_password = other.m_password;
                m_host = other.m_host;
                m_path = other.m_path;
                m_query = other.m_query;
                m_fragment = other.m_fragment;
                m_query_dict = other.m_query_dict;
                m_category = other.m_category;
                m_port = other.m_port;
                m_path_is_rooted = other.m_path_is_rooted;
                m_separator = other.m_separator;
            }
            return *this;
        }

        ~Uri() = default;

        /**
         * Retrieves the scheme part of the URI. The scheme is converted to lower case letters.
         * The scheme specifies the protocol to be used for accessing the resource.
         *
         * @return A constant reference to the string containing the scheme of the URI.
         */
        std::string const &get_scheme() const {
            return m_scheme;
        };

        /**
         * Retrieves the category of the URI scheme.
         * The category indicates whether the URI is hierarchical or non-hierarchical.
         *
         * @return The category of the URI scheme as a value of the scheme_category enum.
         */
        scheme_category get_scheme_category() const {
            return m_category;
        };

        /**
         * Retrieves the content part of a non-hierarchical URI.
         * The content is specific to non-hierarchical URIs, and accessing it
         * for hierarchical URIs will result in an error.
         *
         * @return A constant reference to the string containing the content of the non-hierarchical URI.
         * @throws std::domain_error if the URI is hierarchical.
         */
        std::string const &get_content() const {
            if (m_category != scheme_category::NonHierarchical) {
                throw std::domain_error("The content component is only valid for non-hierarchical URIs.");
            }
            return m_content;
        };

        /**
         * Retrieves the username part of the URI.
         * The username component is only valid for hierarchical URIs. Attempting
         * to access it for non-hierarchical URIs will result in an error.
         *
         * @return A constant reference to the string containing the username of the URI.
         * @throws std::domain_error if the URI is non-hierarchical.
         */
        std::string const &get_username() const {
            if (m_category != scheme_category::Hierarchical) {
                throw std::domain_error("The username component is only valid for hierarchical URIs.");
            }
            return m_username;
        };

        /**
         * Retrieves the password part of the URI.
         * The password component is only valid for hierarchical URIs. Attempting
         * to access it for non-hierarchical URIs will result in an error.
         *
         * @return A constant reference to the string containing the password of the URI.
         * @throws std::domain_error if the URI is non-hierarchical.
         */
        std::string const &get_password() const {
            if (m_category != scheme_category::Hierarchical) {
                throw std::domain_error("The password component is only valid for hierarchical URIs.");
            }
            return m_password;
        };

        /**
         * Retrieves the host part of the URI.
         * The host component is only valid for hierarchical URIs.
         *
         * @return A constant reference to the string containing the host of the URI.
         * @throws std::domain_error if the URI is non-hierarchical.
         */
        std::string const &get_host() const {
            if (m_category != scheme_category::Hierarchical) {
                throw std::domain_error("The host component is only valid for hierarchical URIs.");
            }
            return m_host;
        };

        /**
         * Retrieves the port number of the URI.
         * The port component is only valid for hierarchical URIs. Attempting
         * to access it for non-hierarchical URIs will result in an error.
         *
         * @return The port number of the URI as a 32-bit unsigned integer.
         * @throws std::domain_error if the URI is non-hierarchical.
         */
        uint32_t get_port() const {
            if (m_category != scheme_category::Hierarchical) {
                throw std::domain_error("The port component is only valid for hierarchical URIs.");
            }
            return m_port;
        };

        /**
         * Retrieves the path part of the URI.
         * The path component is a hierarchical structure in the URI.
         * This method is only valid for URIs with a hierarchical scheme.
         *
         * @return A constant reference to the string containing the path of the URI.
         * @throws std::domain_error if the URI scheme is not hierarchical.
         */
        std::string const &get_path() const {
            if (m_category != scheme_category::Hierarchical) {
                throw std::domain_error("The path component is only valid for hierarchical URIs.");
            }
            return m_path;
        };

        /**
         * Retrieves the query part of the URI.
         * The query contains the data to be sent to the server after the '?' in the URI.
         *
         * @return A constant reference to the string containing the query part of the URI.
         */
        std::string const &get_query() const {
            return m_query;
        };

        /**
         * Retrieves the query dictionary of the URI.
         * The query dictionary contains key-value pairs of the query parameters.
         *
         * @return A constant reference to a map containing the query parameters and their corresponding values.
         */
        std::map<std::string, std::string> const &get_query_dictionary() const {
            return m_query_dict;
        };

        /**
         * Retrieves the fragment part of the URI.
         * The fragment specifies a secondary resource or location within the primary resource.
         *
         * @return A constant reference to the string containing the fragment of the URI.
         */
        std::string const &get_fragment() const {
            return m_fragment;
        };


        /**
         * Constructs and retrieves the resource part of the URI.
         * The resource includes the path, and optionally the query and fragment components.
         * If the path is rooted, it is prepended with a forward slash.
         *
         * @return A string representing the full resource of the URI, including the path, and optionally the query and fragment.
         */
        std::string get_resource() const {
            std::string full_resource;

            if (m_path_is_rooted) {
                full_resource.append("/");
            }
            full_resource.append(m_path);

            if (!m_query.empty()) {
                full_resource.append("?");
                full_resource.append(m_query);
            }

            if (!m_fragment.empty()) {
                full_resource.append("#");
                full_resource.append(m_fragment);
            }

            return full_resource;
        }


        /**
         * Converts the URI components into a full URI string representation.
         *
         * The resulting string will include the scheme, host, port, path, query,
         * and fragment depending on the URI's structure and components.
         *
         * @return A string containing the full URI.
         */
        std::string to_string() const {
            std::string full_uri;
            full_uri.append(m_scheme);
            full_uri.append(":");

            if (m_category == scheme_category::Hierarchical) {
                full_uri.append("//");
                if (!(m_username.empty() || m_password.empty())) {
                    full_uri.append(m_username);
                    full_uri.append(":");
                    full_uri.append(m_password);
                    full_uri.append("@");
                }

                full_uri.append(m_host);

                if (m_port != 0) {
                    full_uri.append(":");
                    full_uri.append(std::to_string(m_port));
                }
            } else {
                full_uri.append(m_content);
            }

            full_uri.append(get_resource());
            return full_uri;
        };


    private:
        /**
         * Sets up the URI by parsing the provided URI text and initializing the components.
         * This includes the scheme, content, query, and fragment parts of the URI.
         *
         * @param uri_text A constant reference to the string containing the URI text.
         * @param category The scheme category to be used for parsing the URI.
         * @throw std::invalid_argument if the URI text is of zero length.
         */
        void setup(std::string const &uri_text, scheme_category category) {
            size_t const uri_length = uri_text.length();

            if (uri_length == 0) {
                throw std::invalid_argument("URIs cannot be of zero length.");
            }

            std::string::const_iterator cursor = parse_scheme(uri_text,
                                                              uri_text.begin());
            // After calling parse_scheme, *cursor == ':'; none of the following parsers
            // expect a separator character, so we advance the cursor upon calling them.
            cursor = parse_content(uri_text, (cursor + 1));

            if ((cursor != uri_text.end()) && (*cursor == '?')) {
                cursor = parse_query(uri_text, (cursor + 1));
            }

            if ((cursor != uri_text.end()) && (*cursor == '#')) {
                cursor = parse_fragment(uri_text, (cursor + 1));
            }

            init_query_dictionary(); // If the query string is empty, this will be empty too.
        };

        /**
         * Parses the scheme component from the provided URI string. The scheme is converted to lower case letter.
         * The scheme specifies the protocol to be used for accessing the resource.
         *
         * @param uri_text The complete URI as a string.
         * @param scheme_start An iterator pointing to the start of the scheme component in the URI string.
         * @return An iterator pointing to the end of the scheme component in the URI string.
         * @throws std::invalid_argument If an invalid character is found in the scheme or if the scheme component is empty or not properly terminated.
         */
        std::string::const_iterator parse_scheme(std::string const &uri_text,
                                                 std::string::const_iterator scheme_start) {
            std::string::const_iterator scheme_end = scheme_start;
            while ((scheme_end != uri_text.end()) && (*scheme_end != ':')) {
                if (!(std::isalnum(*scheme_end) || (*scheme_end == '-')
                      || (*scheme_end == '+') || (*scheme_end == '.'))) {
                    throw std::invalid_argument("Invalid character found in the scheme component. Supplied URI was: \""
                                                + uri_text + "\".");
                }
                ++scheme_end;
            }

            if (scheme_end == uri_text.end()) {
                throw std::invalid_argument("End of URI found while parsing the scheme. Supplied URI was: \""
                                            + uri_text + "\".");
            }

            if (scheme_start == scheme_end) {
                throw std::invalid_argument("Scheme component cannot be zero-length. Supplied URI was: \""
                                            + uri_text + "\".");
            }

            m_scheme = std::string(scheme_start, scheme_end);
            std::transform(m_scheme.begin(), m_scheme.end(), m_scheme.begin(), ::tolower);
            return scheme_end;
        };

        /**
         * Parses the content component of a URI starting from the specified iterator position.
         * The method processes the content to extract and store components like the authority and path.
         *
         * @param uri_text A constant reference to the URI string.
         * @param content_start An iterator pointing to the start position of the content in the URI string.
         * @return An iterator pointing to the end position of the content in the URI string.
         */
        std::string::const_iterator parse_content(std::string const &uri_text,
                                                  std::string::const_iterator content_start) {
            std::string::const_iterator content_end = content_start;
            while ((content_end != uri_text.end()) && (*content_end != '?') && (*content_end != '#')) {
                ++content_end;
            }

            m_content = std::string(content_start, content_end);

            if ((m_category == scheme_category::Hierarchical) && (m_content.length() > 0)) {
                // If it's a hierarchical URI, the content should be parsed for the hierarchical components.
                std::string::const_iterator path_start = m_content.begin();
                std::string::const_iterator path_end = m_content.end();
                if (!m_content.compare(0, 2, "//")) {
                    // In this case an authority component is present.
                    std::string::const_iterator authority_cursor = (m_content.begin() + 2);
                    if (m_content.find_first_of('@') != std::string::npos) {
                        std::string::const_iterator userpass_divider = parse_username(uri_text,
                            m_content,
                            authority_cursor);
                        authority_cursor = parse_password(uri_text, m_content, (userpass_divider + 1));
                        // After this call, *authority_cursor == '@', so we skip over it.
                        ++authority_cursor;
                    }

                    authority_cursor = parse_host(uri_text, m_content, authority_cursor);

                    if ((authority_cursor != m_content.end()) && (*authority_cursor == ':')) {
                        authority_cursor = parse_port(uri_text, m_content, (authority_cursor + 1));
                    }

                    if ((authority_cursor != m_content.end()) && (*authority_cursor == '/')) {
                        // Then the path is rooted, and we should note this.
                        m_path_is_rooted = true;
                        path_start = authority_cursor + 1;
                    }

                    // If we've reached the end and no path is present then set path_start
                    // to the end.
                    if (authority_cursor == m_content.end()) {
                        path_start = m_content.end();
                    }
                } else if (!m_content.compare(0, 1, "/")) {
                    m_path_is_rooted = true;
                    ++path_start;
                }

                // We can now build the path based on what remains in the content string,
                // since that's all that exists after the host and optional port component.
                m_path = std::string(path_start, path_end);
            }
            return content_end;
        };

        /**
         * Parses the username section of a URI, delineated by a starting iterator and ending at a colon.
         * Throws an exception if '@' is found without a password following the username.
         *
         * @param uri_text The full text of the URI being parsed.
         * @param content The part of the URI content being examined for the username.
         * @param username_start An iterator pointing to the beginning of the username in the content.
         * @return An iterator pointing to the end of the username in the content.
         * @throws std::invalid_argument if the username is not followed by a password.
         */
        std::string::const_iterator parse_username(std::string const &uri_text,
                                                   std::string const &content,
                                                   std::string::const_iterator username_start) {
            std::string::const_iterator username_end = username_start;
            // Since this is only reachable when '@' was in the content string, we can
            // ignore the end-of-string case.
            while (*username_end != ':') {
                if (*username_end == '@') {
                    throw std::invalid_argument("Username must be followed by a password. Supplied URI was: \""
                                                + uri_text + "\".");
                }
                ++username_end;
            }
            m_username = std::string(username_start, username_end);
            return username_end;
        };

        /**
         * Parses the password from the given URI text, starting from the specified iterator position.
         *
         * @param uri_text The full URI text containing the password.
         * @param content The content to be considered while parsing.
         * @param password_start An iterator pointing to the start of the password in the URI text.
         * @return An iterator pointing to the position just after the end of the password in the URI text.
         */
        std::string::const_iterator parse_password(std::string const &uri_text,
                                                   std::string const &content,
                                                   std::string::const_iterator password_start) {
            std::string::const_iterator password_end = password_start;
            while (*password_end != '@') {
                ++password_end;
            }

            m_password = std::string(password_start, password_end);
            return password_end;
        };

        /**
         * Parses the host part of a URI from the given position within the content. The host can be a domain,
         * an IPv4 address, an IPv6 address, or an IPvFuture literal. IPv6 and IPvFuture literals are parsed
         * within square brackets.
         *
         * @param uri_text The full text of the URI being parsed.
         * @param content The content from which the host part is to be parsed.
         * @param host_start An iterator pointing to the start of the host part within the content.
         * @return An iterator pointing to the end of the parsed host part within the content.
         * @throws std::invalid_argument If the end of the content is encountered while parsing an IP literal.
         */
        std::string::const_iterator parse_host(std::string const &uri_text,
                                               std::string const &content,
                                               std::string::const_iterator host_start) {
            std::string::const_iterator host_end = host_start;
            // So, the host can contain a few things. It can be a domain, it can be an
            // IPv4 address, it can be an IPv6 address, or an IPvFuture literal. In the
            // case of those last two, it's of the form [...] where what's between the
            // brackets is a matter of which IPv?? version it is.
            while (host_end != content.end()) {
                if (*host_end == '[') {
                    // We're parsing an IPv6 or IPvFuture address, so we should handle that
                    // instead of the normal procedure.
                    while ((host_end != content.end()) && (*host_end != ']')) {
                        ++host_end;
                    }

                    if (host_end == content.end()) {
                        throw std::invalid_argument("End of content component encountered "
                                                    "while parsing the host component. "
                                                    "Supplied URI was: \""
                                                    + uri_text + "\".");
                    }

                    ++host_end;
                    break;
                    // We can stop looping, we found the end of the IP literal, which is the
                    // whole of the host component when one's in use.
                } else if ((*host_end == ':') || (*host_end == '/')) {
                    break;
                } else {
                    ++host_end;
                }
            }

            m_host = std::string(host_start, host_end);
            return host_end;
        };

        /**
         * Parses the port portion of the URI and validates that it contains only digits.
         * If an invalid character is encountered, an exception is thrown.
         * The port number is then stored in the member variable m_port.
         *
         * @param uri_text The complete URI string for context in error messages.
         * @param content The substring of the URI that contains the port and subsequent parts.
         * @param port_start The iterator marking the beginning of the port segment within the content string.
         *
         * @return An iterator pointing to the end of the port segment in the content string.
         *
         * @throws std::invalid_argument If an invalid character is encountered in the port segment.
         */
        std::string::const_iterator parse_port(std::string const &uri_text,
                                               std::string const &content,
                                               std::string::const_iterator port_start) {
            std::string::const_iterator port_end = port_start;
            while ((port_end != content.end()) && (*port_end != '/')) {
                if (!std::isdigit(*port_end)) {
                    throw std::invalid_argument("Invalid character while parsing the port. "
                                                "Supplied URI was: \"" + uri_text + "\".");
                }

                ++port_end;
            }

            m_port = std::stoul(std::string(port_start, port_end));
            return port_end;
        };

        /**
         * Parses the query part of the URI from the provided start iterator until the end
         * of the query, indicated by either the end of the input string or a hash ('#') character.
         * The query portion is stored internally in the member variable `m_query`.
         *
         * @param uri_text The entire URI text from which the query is to be parsed.
         * @param query_start An iterator marking the beginning of the query within the URI text.
         * @return An iterator pointing to the character immediately following the end
         *         of the query part within the URI text.
         */
        std::string::const_iterator parse_query(std::string const &uri_text,
                                                std::string::const_iterator query_start) {
            std::string::const_iterator query_end = query_start;
            while ((query_end != uri_text.end()) && (*query_end != '#')) {
                // Queries can contain almost any character except hash, which is reserved
                // for the start of the fragment.
                ++query_end;
            }
            m_query = std::string(query_start, query_end);
            return query_end;
        };

        /**
         * Parses the fragment part of the URI and stores it in the member variable `m_fragment`.
         *
         * @param uri_text A constant reference to the string containing the full URI text.
         * @param fragment_start An iterator pointing to the start of the fragment within the URI text.
         * @return An iterator pointing to the end of the URI text.
         */
        std::string::const_iterator parse_fragment(std::string const &uri_text,
                                                   std::string::const_iterator fragment_start) {
            m_fragment = std::string(fragment_start, uri_text.end());
            return uri_text.end();
        };

        /**
         * Initializes the query dictionary by parsing the query string.
         *
         * The query string is split into key-value pairs using a specified separator (either '&' or ';').
         * Each key is mapped to its corresponding value in the dictionary. Keys without a value will
         * have an empty string as their value in the map. If a duplicate key is found, an exception is thrown.
         *
         * @throws std::invalid_argument If a duplicate key is found in the query string.
         */
        void init_query_dictionary() {
            if (!m_query.empty()) {
                // Loop over the query string looking for '&'s, then check each one for
                // an '=' to find keys and values; if there's not an '=' then the key
                // will have an empty value in the map.
                char separator = (m_separator == query_argument_separator::ampersand) ? '&' : ';';
                size_t carat = 0;
                size_t stanza_end = m_query.find_first_of(separator);
                do {
                    std::string stanza = m_query.substr(
                        carat, ((stanza_end != std::string::npos) ? (stanza_end - carat) : std::string::npos));
                    size_t key_value_divider = stanza.find_first_of('=');
                    std::string key = stanza.substr(0, key_value_divider);
                    std::string value;
                    if (key_value_divider != std::string::npos) {
                        value = stanza.substr((key_value_divider + 1));
                    }

                    if (m_query_dict.count(key) != 0) {
                        throw std::invalid_argument("Bad key in the query string!");
                    }

                    m_query_dict.emplace(key, value);
                    carat = ((stanza_end != std::string::npos)
                                 ? (stanza_end + 1)
                                 : std::string::npos);
                    stanza_end = m_query.find_first_of(separator, carat);
                } while ((stanza_end != std::string::npos)
                         || (carat != std::string::npos));
            }
        }

        std::string m_scheme;
        std::string m_content;
        std::string m_username;
        std::string m_password;
        std::string m_host;
        std::string m_path;
        std::string m_query;
        std::string m_fragment;

        std::map<std::string, std::string> m_query_dict;

        scheme_category m_category;
        unsigned long m_port{};
        bool m_path_is_rooted{};
        query_argument_separator m_separator;
    };
}
