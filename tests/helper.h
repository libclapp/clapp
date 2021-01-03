#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

template <typename ARG, typename T>
bool compare_value(const ARG& arg, T value,
                   ::testing::MatchResultListener* result_listener) {
    if constexpr (std::is_same<T, double>::value) {
        ::testing::Matcher<double> matcher{
            ::testing::DoubleNear(value, 0.0001)};
        ::testing::StringMatchResultListener smrl;
        if (!matcher.MatchAndExplain(arg, &smrl)) {
            *result_listener << "false (" << smrl.str() << ")";
            return false;
        }
        *result_listener << "true (approx)";
        return true;
    } else if constexpr (std::is_same<T, float>::value) {
        ::testing::Matcher<float> matcher{::testing::FloatNear(value, 0.01f)};
        ::testing::StringMatchResultListener smrl;
        if (!matcher.MatchAndExplain(arg, &smrl)) {
            *result_listener << "false (" << smrl.str() << ")";
            return false;
        }
        *result_listener << "true (approx)";
        return true;
    } else {
        *result_listener << (arg == value ? "true" : "false");
        return arg == value;
    }
}

template <typename ARG, typename T>
bool compare_value_vector(const std::vector<ARG>& arg, std::vector<T> value,
                          ::testing::MatchResultListener* result_listener) {
    if (arg.size() != value.size()) {
        *result_listener
            << "false (actual and expected vectors do not match in size (" +
                   std::to_string(arg.size()) + " vs " +
                   std::to_string(arg.size()) + ")).";
        return false;
    }
    for (std::size_t i{0}; i < arg.size(); i++) {
        if (!compare_value(arg[i], value[i], result_listener)) {
            return false;
        }
    }
    return true;
}

#endif
