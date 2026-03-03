#pragma once
#include <string>
#include <c4/substr_fwd.hpp>

namespace utils {

    /**
     * @brief wildcard pattern('*' and '?') matches given str.
     * Supports escape symolb '\'
     * @details
     * Wildcards are used when part of the text is random. You can use :
     * - ? to replace a single mandatory character.
     * - * to replace an unbounded length wildcard.
     * @section specinfo
     * https://github.com/SigmaHQ/sigma-specification/blob/v2.1.0/specification/sigma-rules-specification.md#search-identifier
     * @param[in]   pat pattern
     * @param[in]   str string to match
     * @return      is str matches pat
     * @author Dogan Kurt
     * @section license info
     * modified https://dogankurt.com/wildtest.c
     * If you want to use, modify or publish any code from this(wildtest.c) file, 
     * please contact original author
     * (dogan.kurt@dodobyte.com).
     * @copyright All Rights Reserved.
     */
    bool IsWildcardMatch(const char* pat, const char* str);

    /**
     * @brief wildcard pattern('*' and '?') matches given str ignoring case.
     * @section specinfo
     * https://github.com/SigmaHQ/sigma-specification/blob/v2.1.0/specification/sigma-rules-specification.md#search-identifier
     * @param[in]   pat pattern
     * @param[in]   str string to match
     * @return      is str matches pat
     * @author Dogan Kurt
     * @section license info
     * modified https://dogankurt.com/wildtest.c
     * If you want to use, modify or publish any code from this(wildtest.c) file,
     * please contact original author
     * (dogan.kurt@dodobyte.com).
     * @copyright All Rights Reserved.
    */
    bool IsWildcardMatchInsensitive(const char* pat, const char* str);

    /**
     * @brief wildcard pattern('*') matches given str
     * @param[in]   pat pattern
     * @param[in]   str string to match
     * @return      is str matches pat
     * @section specinfo
     * https://github.com/SigmaHQ/sigma-specification/blob/v2.1.0/specification/sigma-rules-specification.md#condition
     * @author Dogan Kurt
     * @section license info
     * modified https://dogankurt.com/wildtest.c
     * If you want to use, modify or publish any code from this(wildtest.c) file,
     * please contact original author
     * (dogan.kurt@dodobyte.com).
     * @copyright All Rights Reserved.
     */
    bool ConditionPatternMatch(const char* pat, const char* str);

    std::string ToString(const c4::csubstr& str);
    std::string ToString(const c4::substr& str);
} // utils