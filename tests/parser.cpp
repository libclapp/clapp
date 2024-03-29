#include <clapp/argument.h>
#include <clapp/main_parser.h>
#include <clapp/option.h>
#include <clapp/parser.h>
#include <clapp/sub_parser.h>
#include <gmock/gmock.h>

TEST(helpEntry, Construct) {
    clapp::parser::basic_parser_t::help_entry_t he{"option-string",
                                                   "description"};
}

TEST(helpEntry, ConstructAndCompare) {
    const std::string option_str{"option-string"};
    const std::string description_str{"desc"};
    const std::string option_str2{"os2"};
    const std::string description_str2{"ds2"};
    clapp::parser::basic_parser_t::help_entry_t he{option_str, description_str};
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str,
                                                             description_str}),
                testing::Eq(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str2,
                                                             description_str}),
                testing::Ne(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str2,
                                                             description_str2}),
                testing::Ne(he));
    ASSERT_THAT((clapp::parser::basic_parser_t::help_entry_t{option_str,
                                                             description_str2}),
                testing::Ne(he));
}

TEST(purpose, ToStringView) {
    ASSERT_THAT(
        std::string{clapp::parser::basic_parser_t::to_string_view(
                        clapp::parser::basic_parser_t::purpose_t::mandatory)
                        .value()},
        testing::StrEq("mandatory"));
    ASSERT_THAT(
        std::string{clapp::parser::basic_parser_t::to_string_view(
                        clapp::parser::basic_parser_t::purpose_t::optional)
                        .value()},
        testing::StrEq("optional"));
    ASSERT_THAT(clapp::parser::basic_parser_t::to_string_view(
                    static_cast<clapp::parser::basic_parser_t::purpose_t>(128))
                    .has_value(),
                false);
}

class optConfT : public ::testing::Test {
   protected:
    using opt_no_param_conf_t =
        clapp::parser::basic_parser_t::opt_no_param_conf_t;
    using opt_no_param_short_opt_conf_t = opt_no_param_conf_t::short_opt_conf_t;
    using opt_no_param_long_opt_conf_t = opt_no_param_conf_t::long_opt_conf_t;
    using opt_scalar_param_conf_t =
        clapp::parser::basic_parser_t::opt_scalar_param_conf_t;
    using opt_scalar_param_short_opt_conf_t =
        opt_scalar_param_conf_t::short_opt_conf_t;
    using opt_scalar_param_long_opt_conf_t =
        opt_scalar_param_conf_t::long_opt_conf_t;
    using opt_vector_param_conf_t =
        clapp::parser::basic_parser_t::opt_vector_param_conf_t;
    using opt_vector_param_short_opt_conf_t =
        opt_vector_param_conf_t::short_opt_conf_t;
    using opt_vector_param_long_opt_conf_t =
        opt_vector_param_conf_t::long_opt_conf_t;
    using validate_func_t = clapp::parser::basic_parser_t::validate_func_t;
    using purpose_t = clapp::parser::basic_parser_t::purpose_t;
    using help_entry_t = clapp::parser::basic_parser_t::help_entry_t;
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
                            std::move(valid),
                            description}));
}

TEST_F(optConfT, ConstructOptionalOptNoParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_no_param_conf_t{{std::move(np_soc1), std::move(np_soc2)},
                            {std::move(np_loc1), std::move(np_loc2)},
                            std::move(valid),
                            description,
                            purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptNoParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_no_param_conf_t{{std::move(np_soc1), std::move(np_soc2)},
                            {std::move(np_loc1), std::move(np_loc2)},
                            std::move(valid),
                            description,
                            purpose_t::mandatory}));
}

TEST_F(optConfT, ConstructOptNoParamConfCreateBasicOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(
        onpc.create_basic_option_string(),
        testing::StrEq(std::string{"-"} + short_option1 + "|-" + short_option2 +
                       "|--" + long_option1 + "|--" + long_option2));
}

TEST_F(optConfT, ConstructOptionalOptNoParamConfCreateOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               onpc.create_basic_option_string() + "]"));
}

TEST_F(optConfT, ConstructMandatoryOptNoParamConfCreateOptionString) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(valid),
                                   description,
                                   purpose_t::mandatory};
    ASSERT_THAT(onpc.create_option_string(),
                testing::StrEq(onpc.create_basic_option_string()));
}

TEST_F(optConfT, ConstructOptNoParamConfGetOptionHelp) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1), std::move(np_soc2)},
                                   {std::move(np_loc1), std::move(np_loc2)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.get_option_help(),
                testing::Eq(help_entry_t{onpc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionLongOption1) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.find_option(long_option1),
                testing::Ne(onpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionLongOption2) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.find_option(long_option2),
                testing::Eq(onpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionShortOption1) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.find_option(short_option1),
                testing::Ne(onpc.short_options.end()));
}

TEST_F(optConfT, ConstructOptNoParamConfFindOptionShortOption2) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.find_option(short_option2),
                testing::Eq(onpc.short_options.end()));
}

TEST_F(optConfT,
       ConstructOptNoParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_no_param_conf_t onpc{
        {}, {std::move(np_loc1)}, std::move(valid), description};
    ASSERT_THAT(onpc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptNoParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_no_param_conf_t onpc{
        {std::move(np_soc1)}, {}, std::move(valid), description};
    ASSERT_THAT(onpc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionLongOption1) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.contains_option(long_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionLongOption2) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.contains_option(long_option2), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionShortOption1) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
    ASSERT_THAT(onpc.contains_option(short_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptNoParamConfContainsOptionShortOption2) {
    const opt_no_param_conf_t onpc{{std::move(np_soc1)},
                                   {std::move(np_loc1)},
                                   std::move(valid),
                                   description};
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
                                std::move(valid),
                                description}));
}

TEST_F(optConfT, ConstructOptionalOptScalarParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_scalar_param_conf_t{{std::move(sp_soc1), std::move(sp_soc2)},
                                {std::move(sp_loc1), std::move(sp_loc2)},
                                std::move(valid),
                                description,
                                purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptScalarParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_scalar_param_conf_t{{std::move(sp_soc1), std::move(sp_soc2)},
                                {std::move(sp_loc1), std::move(sp_loc2)},
                                std::move(valid),
                                description}));
}

TEST_F(optConfT, ConstructOptScalarParamConfCreateBasicOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.create_basic_option_string(),
                testing::StrEq(std::string{"-"} + short_option1 + "|-" +
                               short_option2 + "|--" + long_option1 + "|--" +
                               long_option2 + "=<param>"));
}

TEST_F(optConfT, ConstructOptionalOptScalarParamConfCreateOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(valid),
                                       description,
                                       purpose_t::optional};
    ASSERT_THAT(ospc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               ospc.create_basic_option_string() + "]"));
}

TEST_F(optConfT, ConstructMandatoryOptScalarParamConfCreateOptionString) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(valid),
                                       description,
                                       purpose_t::mandatory};
    ASSERT_THAT(ospc.create_option_string(),
                testing::StrEq(ospc.create_basic_option_string()));
}

TEST_F(optConfT, ConstructOptScalarParamConfGetOptionHelp) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc1), std::move(sp_soc2)},
                                       {std::move(sp_loc1), std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.get_option_help(),
                testing::Eq(help_entry_t{ospc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionLongOption1) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.find_option(long_option1),
                testing::Eq(ospc.long_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionLongOption2) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.find_option(long_option2),
                testing::Ne(ospc.long_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionShortOption1) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.find_option(short_option1),
                testing::Eq(ospc.short_options.end()));
}

TEST_F(optConfT, ConstructOptScalarParamConfFindOptionShortOption2) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.find_option(short_option2),
                testing::Ne(ospc.short_options.end()));
}

TEST_F(
    optConfT,
    ConstructOptScalarParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_scalar_param_conf_t ospc{
        {}, {std::move(sp_loc1)}, std::move(valid), description};
    ASSERT_THAT(ospc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptScalarParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_scalar_param_conf_t ospc{
        {std::move(sp_soc1)}, {}, std::move(valid), description};
    ASSERT_THAT(ospc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionLongOption1) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionLongOption2) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.contains_option(long_option2), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionShortOption1) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ospc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptScalarParamConfContainsOptionShortOption2) {
    const opt_scalar_param_conf_t ospc{{std::move(sp_soc2)},
                                       {std::move(sp_loc2)},
                                       std::move(valid),
                                       description};
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
                                std::move(valid),
                                description}));
}

TEST_F(optConfT, ConstructOptionalOptVectorParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_vector_param_conf_t{{std::move(vp_soc1), std::move(vp_soc2)},
                                {std::move(vp_loc1), std::move(vp_loc2)},
                                std::move(valid),
                                description,
                                purpose_t::optional}));
}

TEST_F(optConfT, ConstructMandatoryOptVectorParamConf) {
    ASSERT_NO_THROW(static_cast<void>(
        opt_vector_param_conf_t{{std::move(vp_soc1), std::move(vp_soc2)},
                                {std::move(vp_loc1), std::move(vp_loc2)},
                                std::move(valid),
                                description,
                                purpose_t::mandatory}));
}

TEST_F(optConfT, ConstructOptVectorParamConfCreateBasicOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.create_basic_option_string(),
                testing::StrEq(std::string{"-"} + short_option1 + "|-" +
                               short_option2 + "|--" + long_option1 + "|--" +
                               long_option2 + "=<param>"));
}

TEST_F(optConfT, ConstructOptionalOptVectorParamConfCreateOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.create_option_string(),
                testing::StrEq(std::string{"["} +
                               ovpc.create_basic_option_string() + "...]"));
}

TEST_F(optConfT, ConstructMandatoryOptVectorParamConfCreateOptionString) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description,
                                       purpose_t::mandatory};
    ASSERT_THAT(ovpc.create_option_string(),
                testing::StrEq(ovpc.create_basic_option_string() + "..."));
}

TEST_F(optConfT, ConstructOptVectorParamConfGetOptionHelp) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.get_option_help(),
                testing::Eq(help_entry_t{ovpc.create_basic_option_string(),
                                         description}));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionLongOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.find_option(long_option1),
                testing::Ne(ovpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionLongOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.find_option(long_option2),
                testing::Ne(ovpc.long_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionShortOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.find_option(short_option1),
                testing::Ne(ovpc.short_options.end()));
}

TEST_F(optConfT, ConstructOptVectorParamConfFindOptionShortOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.find_option(short_option2),
                testing::Ne(ovpc.short_options.end()));
}

TEST_F(
    optConfT,
    ConstructOptVectorParamConfContainsOptionShortOption1WithoutShortOptions) {
    const opt_vector_param_conf_t ovpc{
        {}, {std::move(vp_loc1)}, std::move(valid), description};
    ASSERT_THAT(ovpc.contains_option(short_option1), testing::Eq(false));
}

TEST_F(optConfT,
       ConstructOptVectorParamConfContainsOptionLongOption1WithoutLongOptions) {
    const opt_vector_param_conf_t ovpc{
        {std::move(vp_soc1)}, {}, std::move(valid), description};
    ASSERT_THAT(ovpc.contains_option(long_option1), testing::Eq(false));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionLongOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.contains_option(long_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionLongOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.contains_option(long_option2), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionShortOption1) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.contains_option(short_option1), testing::Eq(true));
}

TEST_F(optConfT, ConstructOptVectorParamConfContainsOptionShortOption2) {
    const opt_vector_param_conf_t ovpc{{std::move(vp_soc1), std::move(vp_soc2)},
                                       {std::move(vp_loc1), std::move(vp_loc2)},
                                       std::move(valid),
                                       description};
    ASSERT_THAT(ovpc.contains_option(short_option2), testing::Eq(true));
}

class argConfT : public ::testing::Test {
   protected:
    using single_arg_conf_t = clapp::parser::basic_parser_t::single_arg_conf_t;
    using variadic_arg_conf_t =
        clapp::parser::basic_parser_t::variadic_arg_conf_t;

    using argument_func_t = clapp::parser::basic_parser_t::argument_func_t;
    using validate_func_t = clapp::parser::basic_parser_t::validate_func_t;
    using purpose_t = clapp::parser::basic_parser_t::purpose_t;
    using help_entry_t = clapp::parser::basic_parser_t::help_entry_t;
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

class print_and_exit_t {
   public:
    MOCK_METHOD2(print_and_exit,
                 clapp::value::exit_t(const std::string_view print_msg,
                                      int exit_code));
};

template <class T, size_t N>
inline clapp::parser::arg_t parser_make_arg_t(T (&arg)[N]) {
    return clapp::parser::arg_t{static_cast<const char* const*>(arg), N - 1};
}

class empty_basic_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~empty_basic_parser_t() override;
    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

empty_basic_parser_t::~empty_basic_parser_t() = default;

[[nodiscard]] std::string empty_basic_parser_t::gen_short_line_prefix() const {
    return "arg-test-parser" + gen_short_line();
}

class simple_test_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::int32_param_option_t int_option{
        *this, "int", 'i', "Int option.",
        clapp::value::min_max_value_t<std::int32_t>{min_int, max_int}};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser_t::~simple_test_parser_t() = default;

[[nodiscard]] std::string simple_test_parser_t::gen_short_line_prefix() const {
    return "simple-test-parser" + gen_short_line();
}

class simple_test_parser2_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser2_t() override;

    clapp::option::bool_option_t count_option{
        *this, "count", 'c', "Count option.",
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::argument::string_argument_t string_arg{
        *this, "arg-name", "Arg desc",
        clapp::basic_parser_t::purpose_t::optional};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser2_t::~simple_test_parser2_t() = default;

[[nodiscard]] std::string simple_test_parser2_t::gen_short_line_prefix() const {
    return "simple-test-parser2" + gen_short_line();
}

class simple_test_parser3_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser3_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    clapp::option::vector_int64_param_option_t int_option{
        *this,
        "int",
        'i',
        "Int option.",
        clapp::value::min_max_value_t<std::int64_t>{min_int, max_int},
        clapp::basic_parser_t::purpose_t::mandatory};

    clapp::option::vector_string_param_option_t string_option{
        *this, "str", std::vector<char>{'s', 'o'}, "String option.",
        clapp::basic_parser_t::purpose_t::optional};

    clapp::argument::variadic_string_argument_t variadic_string_arg{
        *this, "variadic-arg-name", "Variadic arg desc",
        clapp::basic_parser_t::purpose_t::mandatory};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser3_t::~simple_test_parser3_t() = default;

[[nodiscard]] std::string simple_test_parser3_t::gen_short_line_prefix() const {
    return "simple-test-parser3" + gen_short_line();
}

class simple_test_parser4_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser4_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    static clapp::value::found_func_t::ret_t found_cb(
        const std::string_view option) {
        std::cout << "found cb: " << option << std::endl;
        return clapp::value::exit_t::exit(EXIT_SUCCESS);
    }

    clapp::option::bool_option_t bool_opt{
        *this,
        "bo",
        'b',
        "Arg desc",
        clapp::basic_parser_t::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    clapp::option::string_param_option_t string_opt{
        *this,
        "opt",
        'o',
        "Arg desc",
        clapp::basic_parser_t::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    clapp::argument::string_argument_t string_arg{
        *this, "arg-name", "Arg desc",
        clapp::basic_parser_t::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser4_t::~simple_test_parser4_t() = default;

[[nodiscard]] std::string simple_test_parser4_t::gen_short_line_prefix() const {
    return "simple-test-parser4" + gen_short_line();
}

class simple_test_parser5_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~simple_test_parser5_t() override;

    static constexpr std::int32_t min_int{10};
    static constexpr std::int32_t max_int{200};

    static clapp::value::found_func_t::ret_t found_cb(
        const std::string_view option) {
        static std::size_t i{0U};
        std::cout << "found cb: " << option << std::endl;
        i++;
        if (i == 1U) {
            return clapp::value::found_func_t::ret_t{};
        }
        return clapp::value::exit_t::exit(EXIT_FAILURE);
    }

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    clapp::argument::variadic_string_argument_t var_string_arg{
        *this, "var-arg-name", "Var arg desc",
        clapp::basic_parser_t::purpose_t::optional,
        clapp::value::found_func_t{found_cb}};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

simple_test_parser5_t::~simple_test_parser5_t() = default;

[[nodiscard]] std::string simple_test_parser5_t::gen_short_line_prefix() const {
    return "simple-test-parser5" + gen_short_line();
}

class sub_parser_container_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~sub_parser_container_t() override;

    [[nodiscard]] std::string gen_short_line_prefix() const override;

    clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                             "Bool option."};

    clapp::option::bool_option_t bool_option2{*this, "second", '2',
                                              "Second bool option."};

    clapp::argument::string_argument_t string_arg{*this, "arg-name",
                                                  "Arg desc"};

    class simple_sub_parser_t : public clapp::parser::basic_sub_parser_t {
       public:
        ~simple_sub_parser_t() override;
        clapp::option::bool_option_t bool_option{*this, "bool", 'b',
                                                 "Bool option."};

        using clapp::parser::basic_sub_parser_t::basic_sub_parser_t;
        clapp::option::string_param_option_t string_option{*this, "string", 's',
                                                           "String option."};

        clapp::argument::string_argument_t string_arg{
            *this, "sub-arg-name", "Sub arg desc",
            clapp::basic_parser_t::purpose_t::optional};
    };

    simple_sub_parser_t sub_parser{*this, "sub-parser", "Sub parser desc"};
};

sub_parser_container_t::simple_sub_parser_t::~simple_sub_parser_t() = default;
sub_parser_container_t::~sub_parser_container_t() = default;

[[nodiscard]] std::string sub_parser_container_t::gen_short_line_prefix()
    const {
    return "sub_parser_container" + gen_short_line();
}

class help_parser_t : public clapp::basic_parser_t {
   public:
    using clapp::basic_parser_t::basic_parser_t;
    ~help_parser_t() override;

    clapp::option::help_option_t help_option{
        *this, std::vector<std::string>{"h", "help"},
        std::vector<char>{'h', '?'}, "Print help and exit."};

    [[nodiscard]] std::string gen_short_line_prefix() const override;
};

help_parser_t::~help_parser_t() = default;

[[nodiscard]] std::string help_parser_t::gen_short_line_prefix() const {
    return "help-parser" + gen_short_line();
}

TEST(parser, constructEmptyBasicParser) { empty_basic_parser_t ebp; }

TEST(parser, constructEmptyBasicParserAndParseEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
}

TEST(parser, constructEmptyBasicParserAndParseAndValidateEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, constructEmptyBasicParserAndGenUsagePrefix) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_usage_prefix(), testing::StrEq("Usage:\n"));
}

TEST(parser, constructEmptyBasicParserAndGenHelpMessage) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.gen_help_msg(255), testing::StrEq("arg-test-parser\n"));
}

TEST(parser, constructEmptyBasicParserIsActiveIsTrue) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(ebp.is_active(), testing::Eq(true));
}

TEST(parser, constructEmptyBasicParserGetActiveReturnsThisRef) {
    empty_basic_parser_t ebp;
    ASSERT_THAT(&ebp.get_active_parser(), testing::Eq(&ebp));
}

TEST(parser, callDefaultPrintAndExitWithStringAndExitCode) {
    const std::string text{"text"};
    static constexpr int exit_code{2};
    testing::internal::CaptureStdout();
    const clapp::value::exit_t ret{
        empty_basic_parser_t::default_print_and_exit(text, exit_code)};
    const std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq(text));
    ASSERT_THAT(ret.get_exit_code(), ::testing::Eq(exit_code));
}

static constexpr const char help_str[] =
    "Usage:\nhelp-parser [-h|-?|--h|--help]\n\n  Options:\n    "
    "-h|-?|--h|--help Print help and exit. (optional)\n";

TEST(parser, constructHelpParserAndParseEmptyArguments) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t hp;
    ASSERT_THAT(hp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
}

TEST(parser, constructHelpParserAndParseHelpOptionLong1) {
    constexpr const char* const argv[]{"--help", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t hp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        hp.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionLong2) {
    constexpr const char* const argv[]{"--h", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t hp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        hp.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionShort1) {
    constexpr const char* const argv[]{"-h", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t hp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        hp.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructHelpParserAndParseHelpOptionShort2) {
    constexpr const char* const argv[]{"-?", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    help_parser_t hp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        hp.parse(arg.begin(), arg.end())};
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_THAT(output, testing::StrEq(help_str));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseArgument) {
    constexpr const char* const argv[]{"myarg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: arg-name\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseLongOpt) {
    constexpr const char* const argv[]{"--opt", "string", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: --opt\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseShortOpt) {
    constexpr const char* const argv[]{"-o", "string", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: -o\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser4AndParseShortOpt2) {
    constexpr const char* const argv[]{"-bo", "string", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser4_t stp;
    static constexpr int exit_code{EXIT_SUCCESS};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("found cb: -b\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, constructSimpleTestParser5AndParseArgument) {
    constexpr const char* const argv[]{"myarg", "myarg2", "myarg3", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser5_t stp;
    static constexpr int exit_code{EXIT_FAILURE};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{
        stp.parse(arg.begin(), arg.end())};
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(
        output,
        testing::StrEq("found cb: var-arg-name\nfound cb: var-arg-name\n"));
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, setAndCallPrintAndExitWithStringAndExitCode) {
    print_and_exit_t pae{};
    simple_test_parser_t stp;
    stp.set_print_and_exit_func(
        [&pae](const std::string_view text, const int exit_code) {
            return pae.print_and_exit(text, exit_code);
        });

    static constexpr std::string_view text{"text-string"};
    static constexpr int exit_code{10};
    static constexpr clapp::value::exit_t stop{
        clapp::value::exit_t::exit(exit_code)};
    EXPECT_CALL(pae, print_and_exit(text, exit_code))
        .Times(1)
        .WillOnce(testing::Return(stop));
    const clapp::value::exit_t ret{
        stp.get_print_and_exit_func()(text, exit_code)};
    ASSERT_THAT(ret.get_exit_code(), testing::Eq(exit_code));
}

TEST(parser, genFuncPrintHelpAndExit) {
    empty_basic_parser_t ebp;
    static constexpr int ret_val{EXIT_SUCCESS};
    clapp::value::found_func_t ff{
        ebp.gen_func_print_help_and_req_exit(ret_val)};
    testing::internal::CaptureStdout();
    const clapp::value::found_func_t::ret_t ret{ff.found("name")};
    ASSERT_THAT(ret.has_value(), testing::Eq(true));
    ASSERT_THAT(ret.value().get_exit_code(), testing::Eq(ret_val));
    std::string output{testing::internal::GetCapturedStdout()};
    ASSERT_THAT(output, testing::StrEq("Usage:\narg-test-parser\n"));
}

TEST(parser, constructSimpleTestParserAndGenHelpMessage) {
    simple_test_parser_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser [-b|--bool] [-i|--int=<param>] <arg-name> "
            "[<variadic-arg-name>...]\n\n  Arguments:\n    "
            "arg-name          Arg desc (mandatory)\n    "
            "variadic-arg-name Variadic arg desc (optional, variadic "
            "argument)\n\n  Options:\n    -b|--bool         "
            "Bool option. (optional)\n    -i|--int=<param>  Int option. "
            "(optional, constraint: [10,200])\n"));
}

TEST(parser, constructSimpleTestParser2AndGenHelpMessage) {
    simple_test_parser2_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq("simple-test-parser2 -c|--count [<arg-name>]\n\n  "
                       "Arguments:\n    arg-name   Arg desc (optional)\n\n  "
                       "Options:\n    -c|--count Count option. (mandatory)\n"));
}

TEST(parser, constructSimpleTestParser3AndGenHelpMessage) {
    simple_test_parser3_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser3 -i|--int=<param>... [-s|-o|--str=<param>...] "
            "<variadic-arg-name>...\n\n  Arguments:\n    "
            "variadic-arg-name   Variadic arg desc (mandatory, variadic "
            "argument)\n\n  Options:\n    -i|--int=<param>    Int option. "
            "(mandatory, vector option, constraint: [10,200])\n  "
            "  -s|-o|--str=<param> String "
            "option. (optional, vector option)\n"));
}

TEST(parser, constructSimpleTestParser4AndGenHelpMessage) {
    simple_test_parser4_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser4 [-b|--bo] [-o|--opt=<param>] [<arg-name>]\n\n "
            " Arguments:\n    arg-name         Arg desc (optional)\n\n  "
            "Options:\n    -b|--bo          Arg desc (optional)\n    "
            "-o|--opt=<param> Arg desc (optional)\n"));
}

TEST(parser, constructSimpleTestParser5AndGenHelpMessage) {
    simple_test_parser5_t stp;
    ASSERT_THAT(
        stp.gen_help_msg(255),
        testing::StrEq(
            "simple-test-parser5 <arg-name> [<var-arg-name>...]\n\n  "
            "Arguments:\n    arg-name     Arg desc (mandatory)\n    "
            "var-arg-name Var arg desc (optional, variadic argument)\n"));
}

TEST(parser, constructSubParserContainerAndGenHelpMessage) {
    sub_parser_container_t spc;
    ASSERT_THAT(
        spc.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n"
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name>\n\n"
            "  Arguments:\n    arg-name    "
            "Arg desc (mandatory)\n\n  Options:\n    -b|--bool   "
            "Bool option. (optional)\n    -2|--second Second bool "
            "option. (optional)\n\n"
            "  Subparser:\n    sub-parser  Sub "
            "parser desc\n      Arguments:\n        sub-arg-name      "
            "  Sub arg desc (optional)\n\n      Options:\n        -b|--bool    "
            "       Bool option. (optional)\n        -s|--string=<param> "
            "String "
            "option. (optional)\n\n"));
}

TEST(parser, constructSubParserContainerAndGenSubParserHelpMessage) {
    sub_parser_container_t spc;
    ASSERT_THAT(
        spc.sub_parser.gen_help_msg(255),
        testing::StrEq(
            "sub_parser_container [-b|--bool] [-2|--second] <arg-name> "
            "sub-parser [-b|--bool] [-s|--string=<param>] [<sub-arg-name>]\n\n "
            " "
            "Arguments:\n    sub-arg-name        Sub arg "
            "desc (optional)\n\n  Options:\n    -b|--bool           "
            "Bool option. (optional)\n    -s|--string=<param> String "
            "option. (optional)\n"));
}

TEST(parser, constructEmptyBasicParserAndParseUnknownArgumentsThrows) {
    constexpr const char* const argv[]{"unknown-argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::clapp_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructEmptyBasicParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    empty_basic_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownLongOptionThrows) {
    constexpr const char* const argv[]{"--long-option", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseUnknownShortOptionThrows) {
    constexpr const char* const argv[]{"-s", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongBoolOptionWithParamThrows) {
    constexpr const char* const argv[]{"--bool=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseShortOptionWithParamThrows) {
    constexpr const char* const argv[]{"-b=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser, constructSimpleTestParserAndParseLongIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"--int", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows) {
    constexpr const char* const argv[]{"-i", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserAndParseShortIntOptionWithoutParamThrows2) {
    constexpr const char* const argv[]{"-ib", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::exception::option_param_exception_t);
}

TEST(parser,
     constructSimpleTestParserParseWithoutArgumentAndValidateRecursiveThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(0));
    ASSERT_THROW(ebp.validate_recursive(),
                 clapp::exception::argument_exception_t);
}

TEST(parser, constructSimpleTestParserParseArgumentAndValidateRecursive) {
    constexpr const char* const argv[]{"argument", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::Eq("argument"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    constructSimpleTestParserParseArgumentAndShortOptionWithoutParamsAndValidateRecursive) {
    constexpr const char* const argv[]{"-b", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(parser,
     constructSimpleTestParserParseArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int", "123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(false));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(
    parser,
    constructSimpleTestParserParseArgumentVariadicArgumentAndLongOptionWithParamAndValidate) {
    constexpr const char* const argv[]{"--int=123", "arg", "varg0", "varg1",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(false));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(static_cast<bool>(ebp.variadic_string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.variadic_string_arg.value().size(), testing::Eq(2));
    ASSERT_THAT(ebp.variadic_string_arg.value()[0], testing::StrEq("varg0"));
    ASSERT_THAT(ebp.variadic_string_arg.value()[1], testing::StrEq("varg1"));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(3));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(parser, constructSimpleTestParserParseArgumentAndShortOptionsAndValidate) {
    constexpr const char* const argv[]{"-bi=123", "arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.bool_option), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(ebp.int_option), testing::Eq(true));
    ASSERT_THAT(ebp.int_option.value(), testing::Eq(123));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("arg"));
    ASSERT_THAT(ebp.get_num_processed_arguments(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate_recursive());
}

TEST(parser, constructSimpleTestParser2AndParseLongCountOptionWithParamThrows) {
    constexpr const char* const argv[]{"--count=param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THROW(static_cast<void>(ebp.parse(arg.begin(), arg.end())),
                 clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseWithoutMandatoryOptionThrows) {
    constexpr const char* const argv[]{nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(false));
    ASSERT_THAT(ebp.count_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(0));
    ASSERT_THROW(ebp.validate(), clapp::option_param_exception_t);
}

TEST(parser, constructSimpleTestParser2ParseOptionWithoutParamAndValidate) {
    constexpr const char* const argv[]{"-c", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.has_value(), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(
    parser,
    constructSimpleTestParser2ParseOptionWithoutParamOptionalArgumentAndValidate) {
    constexpr const char* const argv[]{"-c", "opt-arg", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    simple_test_parser2_t ebp;
    ASSERT_THAT(ebp.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(ebp.count_option), testing::Eq(true));
    ASSERT_THAT(ebp.count_option.value(), testing::Eq(1));
    ASSERT_THAT(static_cast<bool>(ebp.string_arg), testing::Eq(true));
    ASSERT_THAT(ebp.string_arg.value(), testing::StrEq("opt-arg"));
    ASSERT_NO_THROW(ebp.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate) {
    constexpr const char* const argv[]{"string-arg", "sub-parser", "-bs=param",
                                       nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_THAT(spc.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(false));
    ASSERT_THAT(spc.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(false));
    ASSERT_THAT(spc.bool_option2.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}

TEST(parser, constructSubParserContainerParseSubparserAndValidate2) {
    constexpr const char* const argv[]{
        "string-arg", "-b", "-2", "sub-parser", "-s", "param", nullptr};
    const clapp::parser::arg_t arg{parser_make_arg_t(argv)};
    sub_parser_container_t spc;
    ASSERT_THAT(spc.parse(arg.begin(), arg.end()).has_value(),
                testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.string_arg), testing::Eq(true));
    ASSERT_THAT(spc.string_arg.value(), testing::StrEq("string-arg"));
    ASSERT_THAT(static_cast<bool>(spc.bool_option), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.bool_option2), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.bool_option2.value(), testing::Eq(true));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.bool_option),
                testing::Eq(false));
    ASSERT_THAT(spc.sub_parser.bool_option.has_value(), testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.bool_option.value(), testing::Eq(false));
    ASSERT_THAT(static_cast<bool>(spc.sub_parser.string_option),
                testing::Eq(true));
    ASSERT_THAT(spc.sub_parser.string_option.value(), testing::StrEq("param"));
    ASSERT_NO_THROW(spc.validate());
}
