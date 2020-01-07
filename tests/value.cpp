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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("-1"),
                 std::out_of_range);
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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("-100"),
                 std::out_of_range);
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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("-10000"),
                 std::out_of_range);
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
        std::out_of_range);
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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::int8_t>("-129"),
                 std::out_of_range);
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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("-32769"),
                 std::out_of_range);
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
                 std::out_of_range);
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("-2147483649"),
                 std::out_of_range);
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
        std::out_of_range);
    ASSERT_THROW(
        clapp::value::convert_value<std::int64_t>("-9223372036854775809"),
        std::out_of_range);
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
    ASSERT_THROW(mmv.validate(5, "option"), std::out_of_range);
    ASSERT_THROW(mmv.validate(55, "option"), std::out_of_range);
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
    ASSERT_THROW(mmv.validate(-2000, "option"), std::out_of_range);
    ASSERT_THROW(mmv.validate(55, "option"), std::out_of_range);
}

TEST(value, path_exists_t) {
    clapp::value::path_exists_t pe;
    ASSERT_THAT(pe.append_restriction_text(), testing::StrEq("existing path"));
    EXPECT_NO_THROW(pe.validate(clapp::fs::path{"/tmp"}, "option"));
    ASSERT_THROW(
        pe.validate(clapp::fs::path{"/tmp/aba/sadf/aksk/annsp"}, "arg"),
        std::runtime_error);
}
