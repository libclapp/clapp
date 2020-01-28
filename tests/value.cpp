#include <clapp/exception.h>
#include <clapp/value.h>
#include <gmock/gmock.h>

TEST(value, convert_value_string) {
    ASSERT_THAT(clapp::value::convert_value<std::string>("aba"),
                testing::Eq(std::string{"aba"}));
    ASSERT_THAT(clapp::value::convert_value<std::string>("abba"),
                testing::Eq(std::string{"abba"}));
}

TEST(value, convert_value_path) {
    ASSERT_THAT(clapp::value::convert_value<clapp::fs::path>("/tmp/test"),
                testing::Eq(clapp::fs::path{"/tmp/test"}));
}

TEST(value, convert_value_uint8_t) {
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0"),
                testing::Eq(std::uint8_t{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("100"),
                testing::Eq(std::uint8_t{100u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("255"),
                testing::Eq(std::uint8_t{255u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("2.555"),
                testing::Eq(std::uint8_t{2}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0xab"),
                testing::Eq(std::uint8_t{0xabu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0xff"),
                testing::Eq(std::uint8_t{0xffu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("077"),
                testing::Eq(std::uint8_t{077}));
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("256"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("-1"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("zzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_uint16_t) {
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0"),
                testing::Eq(std::uint16_t{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("1000"),
                testing::Eq(std::uint16_t{1000u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("65535"),
                testing::Eq(std::uint16_t{65535u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("2222.555"),
                testing::Eq(std::uint16_t{2222}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0xffab"),
                testing::Eq(std::uint16_t{0xffabu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0xffff"),
                testing::Eq(std::uint16_t{0xffffu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("01234"),
                testing::Eq(std::uint16_t{01234u}));
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("65536"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("-100"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_uint32_t) {
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0"),
                testing::Eq(std::uint32_t{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("10000"),
                testing::Eq(std::uint32_t{10000u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("4294967295"),
                testing::Eq(std::uint32_t{4294967295u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0xffab"),
                testing::Eq(std::uint32_t{0xffabu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0xffffffff"),
                testing::Eq(std::uint32_t{0xffffffffu}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0123456"),
                testing::Eq(std::uint32_t{0123456u}));
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("4294967296"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("-10000"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("zz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_uint64_t) {
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("0"),
                testing::Eq(std::uint64_t{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("1000000"),
                testing::Eq(std::uint64_t{1000000u}));
    ASSERT_THAT(
        clapp::value::convert_value<std::uint64_t>("18446744073709551615"),
        testing::Eq(std::uint64_t{18446744073709551615ull}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("0xffab"),
                testing::Eq(std::uint64_t{0xffabu}));
    ASSERT_THAT(
        clapp::value::convert_value<std::uint64_t>("0xffffffffffffffff"),
        testing::Eq(std::uint64_t{0xffffffffffffffffull}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("012345671234"),
                testing::Eq(std::uint64_t{012345671234ull}));
    ASSERT_THROW(
        clapp::value::convert_value<std::uint64_t>("18446744073709551616"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint64_t>("zzzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_int8_t) {
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("0"),
                testing::Eq(std::int8_t{0}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("100"),
                testing::Eq(std::int8_t{100}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("127"),
                testing::Eq(std::int8_t{127}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("-127"),
                testing::Eq(std::int8_t{-127}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("-128"),
                testing::Eq(std::int8_t{-128}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("0xa"),
                testing::Eq(std::int8_t{0xa}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("-0x40"),
                testing::Eq(std::int8_t{-0x40}));
    ASSERT_THAT(clapp::value::convert_value<std::int8_t>("-077"),
                testing::Eq(std::int8_t{-077}));
    ASSERT_THROW(clapp::value::convert_value<std::int8_t>("128"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int8_t>("-129"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int8_t>("zzzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_int16_t) {
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("0"),
                testing::Eq(std::int16_t{0}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("10000"),
                testing::Eq(std::int16_t{10000}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("32767"),
                testing::Eq(std::int16_t{32767}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("-32767"),
                testing::Eq(std::int16_t{-32767}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("-32768"),
                testing::Eq(std::int16_t{-32768}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("0x4fab"),
                testing::Eq(std::int16_t{0x4fab}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("-0x7fff"),
                testing::Eq(std::int16_t{-0x7fff}));
    ASSERT_THAT(clapp::value::convert_value<std::int16_t>("-01234"),
                testing::Eq(std::int16_t{-01234ll}));
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("32768"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("-32769"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("zzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_int32_t) {
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("0"),
                testing::Eq(std::int32_t{0}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("1000000"),
                testing::Eq(std::int32_t{1000000}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("2147483647"),
                testing::Eq(std::int32_t{2147483647}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-2147483647"),
                testing::Eq(std::int32_t{-2147483647}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-2147483648"),
                testing::Eq(std::int32_t{-2147483648}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("0xffab"),
                testing::Eq(std::int32_t{0xffabu}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-0x3fffffff"),
                testing::Eq(std::int32_t{-0x3fffffff}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-0123456"),
                testing::Eq(std::int32_t{-0123456ll}));
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("2147483648"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("-2147483649"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("zz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_int64_t) {
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("0"),
                testing::Eq(std::int64_t{0}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("10000000000"),
                testing::Eq(std::int64_t{10000000000ll}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("9223372036854775807"),
        testing::Eq(std::int64_t{9223372036854775807ll}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-9223372036854775807"),
        testing::Eq(std::int64_t{-9223372036854775807ll}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-9223372036854775808"),
        testing::Eq(std::int64_t{-9223372036854775807ll - 1ll}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("0x3fffffffffffff"),
                testing::Eq(std::int64_t{0x3fffffffffffffLL}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-0x3fffffffffffffff"),
        testing::Eq(std::int64_t{-0x3fffffffffffffffLL}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("-012345671234"),
                testing::Eq(std::int64_t{-012345671234ll}));
    ASSERT_THROW(
        clapp::value::convert_value<std::int64_t>("9223372036854775808"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(
        clapp::value::convert_value<std::int64_t>("-9223372036854775809"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int64_t>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_double) {
    ASSERT_THAT(clapp::value::convert_value<double>("0"),
                testing::Eq(double{0}));
    ASSERT_THAT(clapp::value::convert_value<double>("10000000000"),
                testing::Eq(double{10000000000.}));
    ASSERT_THAT(clapp::value::convert_value<double>("0.125"),
                testing::Eq(double{0.125}));
    ASSERT_THAT(clapp::value::convert_value<double>("9223372036854775807"),
                testing::Eq(double{9223372036854775807.}));
    ASSERT_THAT(clapp::value::convert_value<double>("-9223372036854775807"),
                testing::Eq(double{-9223372036854775807.}));
    ASSERT_THAT(clapp::value::convert_value<double>("-9223372036854775808"),
                testing::Eq(double{-9223372036854775807. - 1.}));
    ASSERT_THAT(clapp::value::convert_value<double>("5e-1"),
                testing::Eq(double{.5}));
    ASSERT_THROW(clapp::value::convert_value<double>("10e-600"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<double>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_float) {
    ASSERT_THAT(clapp::value::convert_value<float>("0"),
                testing::Eq(float{0.f}));
    ASSERT_THAT(clapp::value::convert_value<float>("10000000000"),
                testing::Eq(float{10000000000.f}));
    ASSERT_THAT(clapp::value::convert_value<float>("0.125"),
                testing::Eq(float{0.125f}));
    ASSERT_THAT(clapp::value::convert_value<float>("5e-1"),
                testing::Eq(float{.5f}));
    ASSERT_THROW(clapp::value::convert_value<float>("10e-600"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<float>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convert_value_chrono_nanoseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::nanoseconds>("0"),
                testing::Eq(std::chrono::nanoseconds{0u}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::nanoseconds>("1000000"),
        testing::Eq(std::chrono::nanoseconds{1000000u}));
}

TEST(value, convert_value_chrono_microseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::microseconds>("0"),
                testing::Eq(std::chrono::microseconds{0u}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::microseconds>("1000000"),
        testing::Eq(std::chrono::microseconds{1000000u}));
}

TEST(value, convert_value_chrono_milliseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::milliseconds>("0"),
                testing::Eq(std::chrono::milliseconds{0u}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::milliseconds>("1000000"),
        testing::Eq(std::chrono::milliseconds{1000000u}));
}

TEST(value, convert_value_chrono_seconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::seconds>("0"),
                testing::Eq(std::chrono::seconds{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::seconds>("1000000"),
                testing::Eq(std::chrono::seconds{1000000u}));
}

TEST(value, convert_value_chrono_minutes) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::minutes>("0"),
                testing::Eq(std::chrono::minutes{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::minutes>("1000000"),
                testing::Eq(std::chrono::minutes{1000000u}));
}

TEST(value, convert_value_chrono_hours) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::hours>("0"),
                testing::Eq(std::chrono::hours{0u}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::hours>("1000000"),
                testing::Eq(std::chrono::hours{1000000u}));
}

TEST(value, get_chrono_postfix) {
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::nanoseconds>(),
                testing::StrEq("ns"));
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::microseconds>(),
                testing::StrEq("µs"));
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::milliseconds>(),
                testing::StrEq("ms"));
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::seconds>(),
                testing::StrEq("s"));
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::minutes>(),
                testing::StrEq("m"));
    ASSERT_THAT(clapp::value::get_chrono_postfix<std::chrono::hours>(),
                testing::StrEq("h"));
}

TEST(value, default_value_uint8_t) {
    constexpr std::uint8_t value{10};
    clapp::value::default_value_t<std::uint8_t> dv{value};
    std::stringstream ss;
    ss << "default-value: " << static_cast<std::uint16_t>(value);
    ASSERT_THAT(dv.append_restriction_text(), testing::StrEq(ss.str()));
    ASSERT_THAT(dv.default_value(), testing::Eq(std::uint8_t{value}));
}

TEST(value, default_value_int8_t) {
    constexpr std::int8_t value{-10};
    clapp::value::default_value_t<std::int8_t> dv{value};
    std::stringstream ss;
    ss << "default-value: " << static_cast<std::int16_t>(value);
    ASSERT_THAT(dv.append_restriction_text(), testing::StrEq(ss.str()));
    ASSERT_THAT(dv.default_value(), testing::Eq(std::int8_t{value}));
}

TEST(value, default_value_int32_t) {
    constexpr std::int32_t value{100'000};
    clapp::value::default_value_t<std::int32_t> dv{value};
    std::stringstream ss;
    ss << "default-value: " << value;
    ASSERT_THAT(dv.append_restriction_text(), testing::StrEq(ss.str()));
    ASSERT_THAT(dv.default_value(), testing::Eq(value));
}

TEST(value, default_value_string) {
    const std::string value{"value"};
    clapp::value::default_value_t<std::string> dv{value};
    std::stringstream ss;
    ss << "default-value: " << value;
    ASSERT_THAT(dv.append_restriction_text(), testing::StrEq(ss.str()));
    ASSERT_THAT(dv.default_value(), testing::Eq(value));
}

TEST(value, default_value_milliseconds) {
    constexpr std::uint64_t value{30};
    clapp::value::default_value_t<std::chrono::milliseconds> dv{
        std::chrono::milliseconds{value}};
    std::stringstream ss;
    ss << "default-value: " << value << "ms";
    ASSERT_THAT(dv.append_restriction_text(), testing::StrEq(ss.str()));
    ASSERT_THAT(dv.default_value(),
                testing::Eq(std::chrono::milliseconds{value}));
}

TEST(value, min_max_value_uint8_t) {
    constexpr std::uint8_t min{10};
    constexpr std::uint8_t max{50};
    clapp::value::min_max_value_t<std::uint8_t> mmv{min, max};
    std::stringstream ss;
    ss << "constraint: [" << static_cast<std::int16_t>(min) << ","
       << static_cast<std::int16_t>(max) << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(ss.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(30, "option"));
    ASSERT_THROW(mmv.validate(5, "option"), clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(55, "option"), clapp::exception::out_of_range_t);
}

TEST(value, min_max_value_int32_t) {
    constexpr std::int32_t min{-1024};
    constexpr std::int32_t max{50};
    clapp::value::min_max_value_t<std::int32_t> mmv{min, max};
    std::stringstream ss;
    ss << "constraint: [" << min << "," << max << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(ss.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(-100, "option"));
    ASSERT_THROW(mmv.validate(-2000, "option"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(55, "option"), clapp::exception::out_of_range_t);
}

TEST(value, min_max_value_double_t) {
    const double min{100.};
    const double max{200.5};
    clapp::value::min_max_value_t<double> mmv{min, max};
    std::stringstream ss;
    ss << "constraint: [" << min << "," << max << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(ss.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(130, "option"));
    ASSERT_THROW(mmv.validate(5, "option"), clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(200.6, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, min_max_value_milliseconds_t) {
    const std::chrono::milliseconds min{std::chrono::milliseconds{100}};
    const std::chrono::milliseconds max{std::chrono::milliseconds{200}};
    clapp::value::min_max_value_t<std::chrono::milliseconds> mmv{min, max};
    std::stringstream ss;
    ss << "constraint: [" << min.count() << "ms," << max.count() << "ms]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(ss.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(std::chrono::milliseconds{130}, "option"));
    ASSERT_THROW(mmv.validate(std::chrono::milliseconds{50}, "option"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(std::chrono::milliseconds{300}, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, path_exists_t) {
    clapp::value::path_exists_t pe;
    ASSERT_THAT(pe.append_restriction_text(), testing::StrEq("existing path"));
    EXPECT_NO_THROW(pe.validate(clapp::fs::path{"/tmp"}, "option"));
    ASSERT_THROW(
        pe.validate(clapp::fs::path{"/tmp/aba/sadf/aksk/annsp"}, "arg"),
        clapp::exception::path_does_not_exist_t);
}

TEST(value, found_func_t) {
    std::stringstream ss;
    clapp::value::found_func_t ff{[&ss]() { ss << "called func"; }};
    ff.found();
    ASSERT_THAT(ss.str(), testing::StrEq("called func"));
}
