#include <clapp/type_traits.h>
#include <clapp/value.h>
#include <gmock/gmock.h>

struct with_append_restriction_text_t {
    static std::string append_restriction_text() { return {}; }
};

struct without_append_restriction_text_t {};

TEST(typeTraits, hasAppendRestrictionText) {
    static_assert(clapp::has_append_restriction_text<
                  with_append_restriction_text_t>::value);
    static_assert(!clapp::has_append_restriction_text<
                  without_append_restriction_text_t>::value);
}

struct with_validate_t {
    static bool validate(const std::string& /*value*/,
                         const std::string& /*param_name*/) {
        return true;
    }
};

struct without_validate_t {};

TEST(typeTraits, hasValidate) {
    static_assert(clapp::has_validate<with_validate_t, std::string>::value);
    static_assert(!clapp::has_validate<without_validate_t, std::string>::value);
}

struct with_found_t {
    static clapp::found_func_t::ret_t found(const std::string& /*name*/) {
        return {};
    }
};

struct without_found_t {};

TEST(typeTraits, hasFound) {
    static_assert(clapp::has_found<with_found_t>::value);
    static_assert(!clapp::has_found<without_found_t>::value);
}

struct with_default_value_t {
    static bool default_value() { return false; }
};

struct without_default_value_t {};

TEST(typeTraits, hasDefaultValue) {
    static_assert(clapp::has_default_value<with_default_value_t>::value);
    static_assert(!clapp::has_default_value<without_default_value_t>::value);
}

TEST(typeTraits, isVector) {
    static_assert(clapp::is_vector<std::vector<int>>::value);
    static_assert(!clapp::is_vector<std::string>::value);
}

TEST(typeTraits, isChronoDuration) {
    static_assert(clapp::is_chrono_duration<std::chrono::milliseconds>::value);
    static_assert(!clapp::is_chrono_duration<std::string>::value);
}
