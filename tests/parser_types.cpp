#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

TEST(helpEntry, Construct) {
    clapp::parser::types::help_entry_t help_entry{"option-string",
                                                  "description"};
}

TEST(helpEntry, ConstructAndCompare) {
    const std::string option_str{"option-string"};
    const std::string description_str{"desc"};
    const std::string option_str2{"os2"};
    const std::string description_str2{"ds2"};
    clapp::parser::types::help_entry_t help_entry{option_str, description_str};
    ASSERT_THAT(
        (clapp::parser::types::help_entry_t{option_str, description_str}),
        testing::Eq(help_entry));
    ASSERT_THAT(
        (clapp::parser::types::help_entry_t{option_str2, description_str}),
        testing::Ne(help_entry));
    ASSERT_THAT(
        (clapp::parser::types::help_entry_t{option_str2, description_str2}),
        testing::Ne(help_entry));
    ASSERT_THAT(
        (clapp::parser::types::help_entry_t{option_str, description_str2}),
        testing::Ne(help_entry));
}

TEST(purpose, ToStringView) {
    ASSERT_THAT(std::string{clapp::parser::types::to_string_view(
                                clapp::parser::types::purpose_t::mandatory)
                                .value()},
                testing::StrEq("mandatory"));
    ASSERT_THAT(std::string{clapp::parser::types::to_string_view(
                                clapp::parser::types::purpose_t::optional)
                                .value()},
                testing::StrEq("optional"));
    ASSERT_THAT(clapp::parser::types::to_string_view(
                    static_cast<clapp::parser::types::purpose_t>(128))
                    .has_value(),
                false);
}

class optConfT : public ::testing::Test {
   protected:
    using opt_no_param_conf_t = clapp::parser::types::opt_no_param_conf_t;
    using opt_no_param_short_opt_conf_t = opt_no_param_conf_t::short_opt_conf_t;
    using opt_no_param_long_opt_conf_t = opt_no_param_conf_t::long_opt_conf_t;
    using opt_scalar_param_conf_t =
        clapp::parser::types::opt_scalar_param_conf_t;
    using opt_scalar_param_short_opt_conf_t =
        opt_scalar_param_conf_t::short_opt_conf_t;
    using opt_scalar_param_long_opt_conf_t =
        opt_scalar_param_conf_t::long_opt_conf_t;
    using opt_vector_param_conf_t =
        clapp::parser::types::opt_vector_param_conf_t;
    using opt_vector_param_short_opt_conf_t =
        opt_vector_param_conf_t::short_opt_conf_t;
    using opt_vector_param_long_opt_conf_t =
        opt_vector_param_conf_t::long_opt_conf_t;
    using validate_func_t = clapp::parser::types::validate_func_t;
    using validate_value_func_t = clapp::parser::types::validate_value_func_t;
    using given_func_t = clapp::parser::types::given_func_t;
    using purpose_t = clapp::parser::types::purpose_t;
    using help_entry_t = clapp::parser::types::help_entry_t;
    static constexpr char short_option1{'s'};
    static constexpr char short_option2{'x'};
    static inline const std::string long_option1{"long"};
    static inline const std::string long_option2{"long-option"};
    static inline const std::string option_string{"opt-str"};
    static inline const std::string description{"desc"};
    opt_no_param_short_opt_conf_t np_soc1{
        short_option1,
        [](const char /*opt*/) { return clapp::value::found_func_t::ret_t{}; }};
    opt_no_param_short_opt_conf_t np_soc2{
        short_option2,
        [](const char /*opt*/) { return clapp::value::found_func_t::ret_t{}; }};
    opt_no_param_long_opt_conf_t np_loc1{
        long_option1, [](const std::string_view /*opt*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_no_param_long_opt_conf_t np_loc2{
        long_option2, [](const std::string_view /*opt*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_short_opt_conf_t sp_soc1{
        short_option1, [](const char /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_short_opt_conf_t sp_soc2{
        short_option2, [](const char /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_long_opt_conf_t sp_loc1{
        long_option1,
        [](const std::string_view /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_long_opt_conf_t sp_loc2{
        long_option2,
        [](const std::string_view /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_short_opt_conf_t vp_soc1{
        short_option1, [](const char /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_short_opt_conf_t vp_soc2{
        short_option2, [](const char /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_long_opt_conf_t vp_loc1{
        long_option1,
        [](const std::string_view /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    opt_scalar_param_long_opt_conf_t vp_loc2{
        long_option2,
        [](const std::string_view /*opt*/, const std::string_view /*val*/) {
            return clapp::value::found_func_t::ret_t{};
        }};
    validate_func_t valid{[]() {}};
    validate_value_func_t validate_value{[](const std::string& /*opt*/) {}};
    given_func_t given{[]() { return true; }};
    given_func_t not_given{[]() { return false; }};
    static constexpr std::string_view option_str{"opt-str"};
    purpose_t purpose_mandatory{purpose_t::mandatory};
    purpose_t purpose_optional{purpose_t::optional};

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(optConfT, OptNoParamConfCreateOptionStringShort) {
    ASSERT_THAT(opt_no_param_conf_t::create_option_string(short_option1),
                testing::StrEq(std::string{"-"} + short_option1));
}

TEST_F(optConfT, OptNoParamConfCreateOptionStringLong) {
    ASSERT_THAT(opt_no_param_conf_t::create_option_string(long_option1),
                testing::StrEq(std::string{"--"} + long_option1));
}

TEST_F(optConfT, OptNoParamConfCreateOptionStringShortAndLong) {
    ASSERT_THAT(opt_no_param_conf_t::create_option_string(
                    std::vector<char>{short_option1},
                    std::vector<std::string>{long_option1}),
                testing::StrEq(std::string{"-"} + short_option1 +
                               std::string{"|--"} + long_option1));
}

TEST_F(optConfT, ConstructOptNoParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_no_param_conf_t{{std::move(np_soc1), std::move(np_soc2)},
                            {std::move(np_loc1), std::move(np_loc2)},
                            std::move(given),
                            std::move(validate_value),
                            std::string{option_str},
                            description}));
}

TEST_F(optConfT, ConstructOptionalOptNoParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_no_param_conf_t{{std::move(np_soc1), std::move(np_soc2)},
                            {std::move(np_loc1), std::move(np_loc2)},
                            std::move(given),
                            std::move(validate_value),
                            std::string{option_str},
                            description,
                            purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptNoParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_no_param_conf_t{{std::move(np_soc1), std::move(np_soc2)},
                            {std::move(np_loc1), std::move(np_loc2)},
                            std::move(given),
                            std::move(validate_value),
                            std::string{option_str},
                            description,
                            purpose_t::mandatory}));
}

TEST_F(optConfT, ConstructOptNoParamConfCreateBasicOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(given),
                                   std::move(validate_value),
                                   std::string{option_str},
                                   description};
    ASSERT_THAT(
        onpc.create_basic_option_string(),
        testing::StrEq(std::string{"-"} + short_option1 + "|-" + short_option2 +
                       "|--" + long_option1 + "|--" + long_option2));
}

TEST_F(optConfT, ConstructOptionalOptNoParamConfCreateOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(given),
                                   std::move(validate_value),
                                   std::string{option_str},
                                   description};
    ASSERT_THAT(onpc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               onpc.create_basic_option_string() + "]"));
}

TEST_F(optConfT, ConstructMandatoryOptNoParamConfCreateOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(given),
                                   std::move(validate_value),
                                   std::string{option_str},
                                   description,
                                   purpose_t::mandatory};
    ASSERT_THAT(onpc.create_option_string(),
                testing::StrEq(onpc.create_basic_option_string()));
}

TEST_F(optConfT, ConstructOptNoParamConfGetOptionHelp) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(given),
                                   std::move(validate_value),
                                   std::string{option_str},
                                   description};
    ASSERT_THAT(onpc.get_option_help(),
                testing::Eq(help_entry_t{onpc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionLongOption1) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.find_option(long_option1),
                testing::Ne(onpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionLongOption2) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.find_option(long_option2),
                testing::Eq(onpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionShortOption1) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.find_option(short_option1),
                testing::Ne(onpc.short_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionShortOption2) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.find_option(short_option2),
                testing::Eq(onpc.short_options.end()));
}

TEST_F(optConfT,
       ConstructOptNoParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_no_param_conf_t onpc{{},
                                   {std::move(np_loc1)},
                                   std::move(given),
                                   std::move(validate_value),
                                   std::string{option_str},
                                   description};
    ASSERT_THAT(onpc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptNoParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},    {},
        std::move(given),        std::move(validate_value),
        std::string{option_str}, description};
    ASSERT_THAT(onpc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionLongOption1) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.contains_option(long_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionLongOption2) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.contains_option(long_option2), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionShortOption1) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.contains_option(short_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionShortOption2) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)},      {std::move(np_loc1)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(onpc.contains_option(short_option2), testing::Eq(false));
}

TEST_F(optConfT, OptScalarParamConfCreateOptionStringShort) {
    ASSERT_THAT(opt_scalar_param_conf_t::create_option_string(short_option2),
                testing::StrEq(std::string{"-"} + std::string{short_option2}));
}

TEST_F(optConfT, OptScalarParamConfCreateOptionStringLong) {
    ASSERT_THAT(opt_scalar_param_conf_t::create_option_string(long_option2),
                testing::StrEq(std::string{"--"} + long_option2));
}

TEST_F(optConfT, OptScalarParamConfCreateOptionStringShortAndLong) {
    ASSERT_THAT(opt_no_param_conf_t::create_option_string(
                    std::vector<char>{short_option2},
                    std::vector<std::string>{long_option2}),
                testing::StrEq(std::string{"-"} + short_option2 +
                               std::string{"|--"} + long_option2));
}

TEST_F(optConfT, ConstructOptScalarParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_scalar_param_conf_t{{std::move(sp_soc1), std::move(sp_soc2)},
                                {std::move(sp_loc1), std::move(sp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description}));
}

TEST_F(optConfT, ConstructOptionalOptScalarParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_scalar_param_conf_t{{std::move(sp_soc1), std::move(sp_soc2)},
                                {std::move(sp_loc1), std::move(sp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description,
                                purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptScalarParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_scalar_param_conf_t{{std::move(sp_soc1), std::move(sp_soc2)},
                                {std::move(sp_loc1), std::move(sp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description}));
}

TEST_F(optConfT, ConstructOptScalarParamConfCreateBasicOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ospc.create_basic_option_string(),
                testing::StrEq(std::string{"-"} + short_option1 + "|-" +
                               short_option2 + "|--" + long_option1 + "|--" +
                               long_option2 + "=<param>"));
}

TEST_F(optConfT, ConstructOptionalOptScalarParamConfCreateOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description,
                                       purpose_t::optional};
    ASSERT_THAT(ospc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               ospc.create_basic_option_string() + "]"));
}

TEST_F(optConfT, ConstructMandatoryOptScalarParamConfCreateOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description,
                                       purpose_t::mandatory};
    ASSERT_THAT(ospc.create_option_string(),
                testing::StrEq(ospc.create_basic_option_string()));
}

TEST_F(optConfT, ConstructOptScalarParamConfGetOptionHelp) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ospc.get_option_help(),
                testing::Eq(help_entry_t{ospc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionLongOption1) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.find_option(long_option1),
                testing::Eq(ospc.long_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionLongOption2) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.find_option(long_option2),
                testing::Ne(ospc.long_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionShortOption1) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.find_option(short_option1),
                testing::Eq(ospc.short_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionShortOption2) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.find_option(short_option2),
                testing::Ne(ospc.short_options.end()));
}

TEST_F(
    optConfT,
    ConstructOptScalarParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_scalar_param_conf_t ospc{{},
                                       {std::move(sp_loc1)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ospc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptScalarParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc1)},    {},
        std::move(given),        std::move(validate_value),
        std::string{option_str}, description};
    ASSERT_THAT(ospc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionLongOption1) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionLongOption2) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.contains_option(long_option2), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionShortOption1) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionShortOption2) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc2)},      {std::move(sp_loc2)},    std::move(given),
        std::move(validate_value), std::string{option_str}, description};
    ASSERT_THAT(ospc.contains_option(short_option2), testing::Eq(true));
}

TEST_F(optConfT, OptVectorParamConfCreateOptionStringShort) {
    ASSERT_THAT(opt_vector_param_conf_t::create_option_string(short_option2),
                testing::StrEq(std::string{"-"} + short_option2));
}

TEST_F(optConfT, OptVectorParamConfCreateOptionStringLong) {
    ASSERT_THAT(opt_vector_param_conf_t::create_option_string(long_option2),
                testing::StrEq(std::string{"--"} + long_option2));
}

TEST_F(optConfT, OptVectorParamConfCreateOptionStringShortAndLong) {
    ASSERT_THAT(
        opt_no_param_conf_t::create_option_string(
            std::vector<char>{short_option1, short_option2},
            std::vector<std::string>{long_option1, long_option2}),
        testing::StrEq(std::string{"-"} + short_option1 + std::string{"|-"} +
                       short_option2 + std::string{"|--"} + long_option1 +
                       std::string{"|--"} + long_option2));
}

TEST_F(optConfT, ConstructOptVectorParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_vector_param_conf_t{{std::move(vp_soc1), std::move(vp_soc2)},
                                {std::move(vp_loc1), std::move(vp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description}));
}

TEST_F(optConfT, ConstructOptionalOptVectorParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_vector_param_conf_t{{std::move(vp_soc1), std::move(vp_soc2)},
                                {std::move(vp_loc1), std::move(vp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description,
                                purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptVectorParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_vector_param_conf_t{{std::move(vp_soc1), std::move(vp_soc2)},
                                {std::move(vp_loc1), std::move(vp_loc2)},
                                std::move(given),
                                std::move(validate_value),
                                std::string{option_str},
                                description,
                                purpose_t::mandatory}));
}

TEST_F(optConfT, ConstructOptVectorParamConfCreateBasicOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.create_basic_option_string(),
                testing::StrEq(std::string{"-"} + short_option1 + "|-" +
                               short_option2 + "|--" + long_option1 + "|--" +
                               long_option2 + "=<param>"));
}

TEST_F(optConfT, ConstructOptionalOptVectorParamConfCreateOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               ovpc.create_basic_option_string() + "...]"));
}

TEST_F(optConfT, ConstructMandatoryOptVectorParamConfCreateOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description,
                                       purpose_t::mandatory};
    ASSERT_THAT(ovpc.create_option_string(),
                testing::StrEq(ovpc.create_basic_option_string() + "..."));
}

TEST_F(optConfT, ConstructOptVectorParamConfGetOptionHelp) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.get_option_help(),
                testing::Eq(help_entry_t{ovpc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionLongOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.find_option(long_option1),
                testing::Ne(ovpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionLongOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.find_option(long_option2),
                testing::Ne(ovpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionShortOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.find_option(short_option1),
                testing::Ne(ovpc.short_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionShortOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.find_option(short_option2),
                testing::Ne(ovpc.short_options.end()));
}

TEST_F(
    optConfT,
    ConstructOptVectorParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_vector_param_conf_t ovpc{{},
                                       {std::move(vp_loc1)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptVectorParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_vector_param_conf_t ovpc{
        {std::move(vp_soc1)},    {},
        std::move(given),        std::move(validate_value),
        std::string{option_str}, description};
    ASSERT_THAT(ovpc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionLongOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.contains_option(long_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionLongOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.contains_option(long_option2), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionShortOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.contains_option(short_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionShortOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(given),
                                       std::move(validate_value),
                                       std::string{option_str},
                                       description};
    ASSERT_THAT(ovpc.contains_option(short_option2), testing::Eq(true));
}

class argConfT : public ::testing::Test {
   protected:
    using single_arg_conf_t = clapp::parser::types::single_arg_conf_t;
    using variadic_arg_conf_t = clapp::parser::types::variadic_arg_conf_t;

    using argument_func_t = clapp::parser::types::argument_func_t;
    using validate_func_t = clapp::parser::types::validate_func_t;
    using purpose_t = clapp::parser::types::purpose_t;
    using help_entry_t = clapp::parser::types::help_entry_t;
    static inline const std::string arg_1{"arg"};
    static inline const std::string arg_2{"argument"};
    static inline const std::string description{"desc"};
    argument_func_t arg_func{[](const std::string_view /*arg*/) {
        return clapp::value::found_func_t::ret_t{};
    }};
    validate_func_t valid{[]() {}};
    purpose_t purpose_mandatory{purpose_t::mandatory};
    purpose_t purpose_optional{purpose_t::optional};

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(argConfT, ConstructSingleArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(single_arg_conf_t{
        std::move(arg_func), arg_1, description, std::move(valid)}));
}

TEST_F(argConfT, ConstructMandatorySingleArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(
        single_arg_conf_t{std::move(arg_func), arg_1, description,
                          std::move(valid), purpose_t::mandatory}));
}

TEST_F(argConfT, ConstructOptionalSingleArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(
        single_arg_conf_t{std::move(arg_func), arg_1, description,
                          std::move(valid), purpose_t::optional}));
}

TEST_F(argConfT, ConstructSingleArgConfTCreateBasicArgumentString) {
    const single_arg_conf_t sac{std::move(arg_func), arg_1, description,
                                std::move(valid)};
    ASSERT_THAT(sac.create_basic_argument_string(), testing::StrEq(arg_1));
}

TEST_F(argConfT, ConstructMandatorySingleArgConfTCreateArgumentString) {
    const single_arg_conf_t sac{std::move(arg_func), arg_1, description,
                                std::move(valid), purpose_t::mandatory};
    ASSERT_THAT(sac.create_argument_string(),
                testing::StrEq("<" + sac.create_basic_argument_string() + ">"));
}

TEST_F(argConfT, ConstructOptionalSingleArgConfTCreateArgumentString) {
    const single_arg_conf_t sac{std::move(arg_func), arg_1, description,
                                std::move(valid), purpose_t::optional};
    ASSERT_THAT(sac.create_argument_string(),
                testing::StrEq(std::string{"[<"} +
                               sac.create_basic_argument_string() + ">]"));
}

TEST_F(argConfT, ConstructSingleArgConfTGetArgumentHelp) {
    const single_arg_conf_t sac{std::move(arg_func), arg_1, description,
                                std::move(valid)};
    ASSERT_THAT(sac.get_argument_help(),
                testing::Eq(help_entry_t{sac.create_basic_argument_string(),
                                         description}));
}

TEST_F(argConfT, ConstructVariadicArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(variadic_arg_conf_t{
        std::move(arg_func), arg_2, description, std::move(valid)}));
}

TEST_F(argConfT, ConstructMandatoryVariadicArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(
        variadic_arg_conf_t{std::move(arg_func), arg_2, description,
                            std::move(valid), purpose_t::mandatory}));
}

TEST_F(argConfT, ConstructOptionalVariadicArgConfT) {
    ASSERT_NO_THROW(static_cast<void>(
        variadic_arg_conf_t{std::move(arg_func), arg_2, description,
                            std::move(valid), purpose_t::optional}));
}

TEST_F(argConfT, ConstructVariadicArgConfTCreateBasicArgumentString) {
    const variadic_arg_conf_t sac{std::move(arg_func), arg_2, description,
                                  std::move(valid)};
    ASSERT_THAT(sac.create_basic_argument_string(), testing::StrEq(arg_2));
}

TEST_F(argConfT, ConstructMandatoryVariadicArgConfTCreateArgumentString) {
    const variadic_arg_conf_t sac{std::move(arg_func), arg_2, description,
                                  std::move(valid), purpose_t::mandatory};
    ASSERT_THAT(
        sac.create_argument_string(),
        testing::StrEq("<" + sac.create_basic_argument_string() + ">..."));
}

TEST_F(argConfT, ConstructOptionalVariadicArgConfTCreateArgumentString) {
    const variadic_arg_conf_t sac{std::move(arg_func), arg_2, description,
                                  std::move(valid), purpose_t::optional};
    ASSERT_THAT(sac.create_argument_string(),
                testing::StrEq(std::string{"[<"} +
                               sac.create_basic_argument_string() + ">...]"));
}

TEST_F(argConfT, ConstructVariadicArgConfTGetArgumentHelp) {
    const variadic_arg_conf_t sac{std::move(arg_func), arg_2, description,
                                  std::move(valid)};
    ASSERT_THAT(sac.get_argument_help(),
                testing::Eq(help_entry_t{sac.create_basic_argument_string(),
                                         description}));
}
