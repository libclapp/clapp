#include <clapp/exception.h>
#include <clapp/value.h>
#include <gmock/gmock.h>

TEST(value, convertValueString) {
    ASSERT_THAT(clapp::value::convert_value<std::string>("aba"),
                testing::Eq(std::string{"aba"}));
    ASSERT_THAT(clapp::value::convert_value<std::string>("abba"),
                testing::Eq(std::string{"abba"}));
}

TEST(value, convertValuePath) {
    ASSERT_THAT(clapp::value::convert_value<clapp::fs::path>("/tmp/test"),
                testing::Eq(clapp::fs::path{"/tmp/test"}));
}

TEST(value, convertValueBool) {
    ASSERT_THAT(clapp::value::convert_value<bool>("0"), testing::Eq(false));
    ASSERT_THAT(clapp::value::convert_value<bool>("false"), testing::Eq(false));
    ASSERT_THAT(clapp::value::convert_value<bool>("FALSE"), testing::Eq(false));
    ASSERT_THAT(clapp::value::convert_value<bool>("1"), testing::Eq(true));
    ASSERT_THAT(clapp::value::convert_value<bool>("true"), testing::Eq(true));
    ASSERT_THAT(clapp::value::convert_value<bool>("TRUE"), testing::Eq(true));
    ASSERT_THROW(clapp::value::convert_value<bool>("falSe"),
                 clapp::exception::invalid_value_t);
    ASSERT_THROW(clapp::value::convert_value<bool>("TRue"),
                 clapp::exception::invalid_value_t);
    ASSERT_THROW(clapp::value::convert_value<bool>("abc"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueUint8T) {
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0"),
                testing::Eq(std::uint8_t{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("100"),
                testing::Eq(std::uint8_t{100U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("255"),
                testing::Eq(std::uint8_t{255U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("2.555"),
                testing::Eq(std::uint8_t{2U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0xab"),
                testing::Eq(std::uint8_t{0xabU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("0xff"),
                testing::Eq(std::uint8_t{0xffU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint8_t>("077"),
                testing::Eq(std::uint8_t{077U}));
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("256"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("-1"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint8_t>("zzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueUint16T) {
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0"),
                testing::Eq(std::uint16_t{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("1000"),
                testing::Eq(std::uint16_t{1000U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("65535"),
                testing::Eq(std::uint16_t{65535U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("2222.555"),
                testing::Eq(std::uint16_t{2222U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0xffab"),
                testing::Eq(std::uint16_t{0xffabU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("0xffff"),
                testing::Eq(std::uint16_t{0xffffU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint16_t>("01234"),
                testing::Eq(std::uint16_t{01234U}));
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("65536"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("-100"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint16_t>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueUint32T) {
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0"),
                testing::Eq(std::uint32_t{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("10000"),
                testing::Eq(std::uint32_t{10000U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("4294967295"),
                testing::Eq(std::uint32_t{4294967295U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0xffab"),
                testing::Eq(std::uint32_t{0xffabU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0xffffffff"),
                testing::Eq(std::uint32_t{0xffffffffU}));
    ASSERT_THAT(clapp::value::convert_value<std::uint32_t>("0123456"),
                testing::Eq(std::uint32_t{0123456U}));
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("4294967296"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("-10000"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint32_t>("zz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueUint64T) {
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("0"),
                testing::Eq(std::uint64_t{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("1000000"),
                testing::Eq(std::uint64_t{1000000U}));
    ASSERT_THAT(
        clapp::value::convert_value<std::uint64_t>("18446744073709551615"),
        testing::Eq(std::uint64_t{18446744073709551615ULL}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("0xffab"),
                testing::Eq(std::uint64_t{0xffabU}));
    ASSERT_THAT(
        clapp::value::convert_value<std::uint64_t>("0xffffffffffffffff"),
        testing::Eq(std::uint64_t{0xffffffffffffffffULL}));
    ASSERT_THAT(clapp::value::convert_value<std::uint64_t>("012345671234"),
                testing::Eq(std::uint64_t{012345671234ULL}));
    ASSERT_THROW(
        clapp::value::convert_value<std::uint64_t>("18446744073709551616"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::uint64_t>("zzzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueInt8T) {
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

TEST(value, convertValueInt16T) {
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
                testing::Eq(std::int16_t{-01234LL}));
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("32768"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("-32769"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int16_t>("zzz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueInt32T) {
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
                testing::Eq(std::int32_t{0xffab}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-0x3fffffff"),
                testing::Eq(std::int32_t{-0x3fffffff}));
    ASSERT_THAT(clapp::value::convert_value<std::int32_t>("-0123456"),
                testing::Eq(std::int32_t{-0123456LL}));
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("2147483648"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("-2147483649"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int32_t>("zz"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueInt64T) {
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("0"),
                testing::Eq(std::int64_t{0}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("10000000000"),
                testing::Eq(std::int64_t{10000000000LL}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("9223372036854775807"),
        testing::Eq(std::int64_t{9223372036854775807LL}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-9223372036854775807"),
        testing::Eq(std::int64_t{-9223372036854775807LL}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-9223372036854775808"),
        testing::Eq(std::int64_t{-9223372036854775807LL - 1LL}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("0x3fffffffffffff"),
                testing::Eq(std::int64_t{0x3fffffffffffffLL}));
    ASSERT_THAT(
        clapp::value::convert_value<std::int64_t>("-0x3fffffffffffffff"),
        testing::Eq(std::int64_t{-0x3fffffffffffffffLL}));
    ASSERT_THAT(clapp::value::convert_value<std::int64_t>("-012345671234"),
                testing::Eq(std::int64_t{-012345671234LL}));
    ASSERT_THROW(
        clapp::value::convert_value<std::int64_t>("9223372036854775808"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(
        clapp::value::convert_value<std::int64_t>("-9223372036854775809"),
        clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<std::int64_t>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueDouble) {
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

TEST(value, convertValueFloat) {
    ASSERT_THAT(clapp::value::convert_value<float>("0"),
                testing::Eq(float{0.F}));
    ASSERT_THAT(clapp::value::convert_value<float>("10000000000"),
                testing::Eq(float{10000000000.F}));
    ASSERT_THAT(clapp::value::convert_value<float>("0.125"),
                testing::Eq(float{0.125F}));
    ASSERT_THAT(clapp::value::convert_value<float>("5e-1"),
                testing::Eq(float{.5F}));
    ASSERT_THROW(clapp::value::convert_value<float>("10e-600"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(clapp::value::convert_value<float>("z"),
                 clapp::exception::invalid_value_t);
}

TEST(value, convertValueChronoNanoseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::nanoseconds>("0"),
                testing::Eq(std::chrono::nanoseconds{0U}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::nanoseconds>("1000000"),
        testing::Eq(std::chrono::nanoseconds{1000000U}));
}

TEST(value, convertValueChronoMicroseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::microseconds>("0"),
                testing::Eq(std::chrono::microseconds{0U}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::microseconds>("1000000"),
        testing::Eq(std::chrono::microseconds{1000000U}));
}

TEST(value, convertValueChronoMilliseconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::milliseconds>("0"),
                testing::Eq(std::chrono::milliseconds{0U}));
    ASSERT_THAT(
        clapp::value::convert_value<std::chrono::milliseconds>("1000000"),
        testing::Eq(std::chrono::milliseconds{1000000U}));
}

TEST(value, convertValueChronoSeconds) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::seconds>("0"),
                testing::Eq(std::chrono::seconds{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::seconds>("1000000"),
                testing::Eq(std::chrono::seconds{1000000U}));
}

TEST(value, convertValueChronoMinutes) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::minutes>("0"),
                testing::Eq(std::chrono::minutes{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::minutes>("1000000"),
                testing::Eq(std::chrono::minutes{1000000U}));
}

TEST(value, convertValueChronoHours) {
    ASSERT_THAT(clapp::value::convert_value<std::chrono::hours>("0"),
                testing::Eq(std::chrono::hours{0U}));
    ASSERT_THAT(clapp::value::convert_value<std::chrono::hours>("1000000"),
                testing::Eq(std::chrono::hours{1000000U}));
}

TEST(value, getChronoPostfix) {
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

TEST(value, defaultValueUint8T) {
    constexpr std::uint8_t value{10};
    clapp::value::default_value_t<std::uint8_t> dval{value};
    std::stringstream stringstr;
    stringstr << "default-value: " << static_cast<std::uint16_t>(value);
    ASSERT_THAT(dval.append_restriction_text(),
                testing::StrEq(stringstr.str()));
    ASSERT_THAT(dval.default_value(), testing::Eq(std::uint8_t{value}));
}

TEST(value, defaultValueInt8T) {
    constexpr std::int8_t value{-10};
    clapp::value::default_value_t<std::int8_t> dval{value};
    std::stringstream stringstr;
    stringstr << "default-value: " << static_cast<std::int16_t>(value);
    ASSERT_THAT(dval.append_restriction_text(),
                testing::StrEq(stringstr.str()));
    ASSERT_THAT(dval.default_value(), testing::Eq(std::int8_t{value}));
}

TEST(value, defaultValueInt32T) {
    constexpr std::int32_t value{100'000};
    clapp::value::default_value_t<std::int32_t> dval{value};
    std::stringstream stringstr;
    stringstr << "default-value: " << value;
    ASSERT_THAT(dval.append_restriction_text(),
                testing::StrEq(stringstr.str()));
    ASSERT_THAT(dval.default_value(), testing::Eq(value));
}

TEST(value, defaultValueString) {
    const std::string value{"value"};
    clapp::value::default_value_t<std::string> dval{value};
    std::stringstream stringstr;
    stringstr << "default-value: " << value;
    ASSERT_THAT(dval.append_restriction_text(),
                testing::StrEq(stringstr.str()));
    ASSERT_THAT(dval.default_value(), testing::Eq(value));
}

TEST(value, defaultValueMilliseconds) {
    constexpr std::uint64_t value{30};
    clapp::value::default_value_t<std::chrono::milliseconds> dval{
        std::chrono::milliseconds{value}};
    std::stringstream stringstr;
    stringstr << "default-value: " << value << "ms";
    ASSERT_THAT(dval.append_restriction_text(),
                testing::StrEq(stringstr.str()));
    ASSERT_THAT(dval.default_value(),
                testing::Eq(std::chrono::milliseconds{value}));
}

TEST(value, minMaxValueUint8T) {
    constexpr std::uint8_t min{10};
    constexpr std::uint8_t max{50};
    clapp::value::min_max_value_t<std::uint8_t> mmv{min, max};
    std::stringstream stringstr;
    stringstr << "constraint: [" << static_cast<std::int16_t>(min) << ","
              << static_cast<std::int16_t>(max) << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(stringstr.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(30, "option"));
    ASSERT_THROW(mmv.validate(5, "option"), clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(55, "option"), clapp::exception::out_of_range_t);
}

TEST(value, minMaxValueInt32T) {
    constexpr std::int32_t min{-1024};
    constexpr std::int32_t max{50};
    clapp::value::min_max_value_t<std::int32_t> mmv{min, max};
    std::stringstream stringstr;
    stringstr << "constraint: [" << min << "," << max << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(stringstr.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(-100, "option"));
    ASSERT_THROW(mmv.validate(-2000, "option"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(55, "option"), clapp::exception::out_of_range_t);
}

TEST(value, minMaxValueDoubleT) {
    const double min{100.};
    const double max{200.5};
    const double above{max + 0.1};
    clapp::value::min_max_value_t<double> mmv{min, max};
    std::stringstream stringstr;
    stringstr << "constraint: [" << min << "," << max << "]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(stringstr.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(130, "option"));
    ASSERT_THROW(mmv.validate(5, "option"), clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(above, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, minMaxValueMillisecondsT) {
    const std::chrono::milliseconds min{std::chrono::milliseconds{100}};
    const std::chrono::milliseconds max{std::chrono::milliseconds{200}};
    clapp::value::min_max_value_t<std::chrono::milliseconds> mmv{min, max};
    std::stringstream stringstr;
    stringstr << "constraint: [" << min.count() << "ms," << max.count()
              << "ms]";
    ASSERT_THAT(mmv.append_restriction_text(), testing::StrEq(stringstr.str()));
    EXPECT_NO_THROW(mmv.validate(min, "option"));
    EXPECT_NO_THROW(mmv.validate(max, "option"));
    EXPECT_NO_THROW(mmv.validate(std::chrono::milliseconds{130}, "option"));
    ASSERT_THROW(mmv.validate(std::chrono::milliseconds{50}, "option"),
                 clapp::exception::out_of_range_t);
    ASSERT_THROW(mmv.validate(std::chrono::milliseconds{300}, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, notNullValueBool) {
    constexpr bool one{true};
    constexpr bool zero{false};
    clapp::value::not_null_value_t<bool> nnv;
    ASSERT_THAT(nnv.append_restriction_text(),
                testing::StrEq("constraint: not null"));
    EXPECT_NO_THROW(nnv.validate(one, "option"));
    ASSERT_THROW(nnv.validate(zero, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, notNullValueUint8T) {
    constexpr std::uint8_t two{2};
    constexpr std::uint8_t one{1};
    constexpr std::uint8_t zero{0};
    clapp::value::not_null_value_t<std::uint8_t> nnv;
    ASSERT_THAT(nnv.append_restriction_text(),
                testing::StrEq("constraint: not null"));
    EXPECT_NO_THROW(nnv.validate(two, "option"));
    EXPECT_NO_THROW(nnv.validate(one, "option"));
    ASSERT_THROW(nnv.validate(zero, "option"),
                 clapp::exception::out_of_range_t);
}

TEST(value, pathExistsT) {
    clapp::value::path_exists_t path_exists;
    ASSERT_THAT(path_exists.append_restriction_text(),
                testing::StrEq("existing path"));
    EXPECT_NO_THROW(path_exists.validate(clapp::fs::path{"/tmp"}, "option"));
    ASSERT_THROW(path_exists.validate(
                     clapp::fs::path{"/tmp/aba/sadf/aksk/annsp"}, "arg"),
                 clapp::exception::path_does_not_exist_t);
}

TEST(value, exitTExit) {
    static constexpr int exit_code{0x58};
    static constexpr clapp::value::exit_t exit{
        clapp::value::exit_t::exit(exit_code)};
    ASSERT_THAT(exit.get_exit_code(), testing::Eq(exit_code));
}

TEST(value, foundFuncTReturnsNothing) {
    std::stringstream stringstr;
    clapp::value::found_func_t found_func{
        [&stringstr](const std::string& name) {
            stringstr << "called func '" << name << "'";
            return clapp::value::found_func_t::ret_t{};
        }};
    ASSERT_THAT(found_func.found("name").has_value(), testing::Eq(false));
    ASSERT_THAT(stringstr.str(), testing::StrEq("called func 'name'"));
}

TEST(value, foundFuncTReturnsExit) {
    static constexpr int exit_code{77};
    std::stringstream stringstr;
    clapp::value::found_func_t found_func{
        [&stringstr](const std::string& name) {
            stringstr << "called func-" << name;
            return clapp::value::found_func_t::ret_t{
                clapp::value::exit_t::exit(exit_code)};
        }};
    const clapp::value::found_func_t::ret_t ret{found_func.found("name2")};
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
    ASSERT_THAT(stringstr.str(), testing::StrEq("called func-name2"));
}

TEST(value, concatStr) {
    ASSERT_THAT(clapp::value::concat_str("", "b"), testing::StrEq("b"));
    ASSERT_THAT(clapp::value::concat_str("a", "b"), testing::StrEq("a b"));
    ASSERT_THAT(clapp::value::concat_str("a", ""), testing::StrEq("a "));
}

TEST(value, stringifyNulloptThrows) {
    const std::optional<std::vector<std::string>> opt_str_vec{std::nullopt};
    ASSERT_THROW(clapp::value::stringify(opt_str_vec),
                 std::bad_optional_access);
}

TEST(value, stringifyEmptyVec) {
    const std::optional<std::vector<std::string>> opt_str_vec{
        std::vector<std::string>{}};
    ASSERT_THAT(clapp::value::stringify(opt_str_vec), testing::StrEq(""));
}

TEST(value, stringifyVecWithOneElement) {
    const std::optional<std::vector<std::string>> opt_str_vec{
        std::vector<std::string>{"one"}};
    ASSERT_THAT(clapp::value::stringify(opt_str_vec), testing::StrEq("one"));
}

TEST(value, stringifyVecWithTwoElements) {
    const std::optional<std::vector<std::string>> opt_str_vec{
        std::vector<std::string>{"one", "two"}};
    ASSERT_THAT(clapp::value::stringify(opt_str_vec),
                testing::StrEq("one two"));
}
