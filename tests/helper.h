#ifndef TESTS_HELPER_H
#define TESTS_HELPER_H

template <typename ARG, typename T>
bool compare_value(const ARG& arg, const T& value,
                   ::testing::MatchResultListener* result_listener) {
    if constexpr (std::is_same<T, double>::value) {
        const double epsilon{0.0001};
        const bool ret{std::fabs(arg - value) < epsilon};
        *result_listener << (ret ? "true" : "false") << " (" << arg
                         << " == " << value << " (approx, " << epsilon << "))";
        return ret;
    } else if constexpr (std::is_same<T, float>::value) {
        const float epsilon{0.01f};
        const bool ret{std::fabs(arg - value) < epsilon};
        *result_listener << (ret ? "true" : "false") << " (" << arg
                         << " == " << value << " (approx, " << epsilon << "))";
        return ret;
    } else {
        *result_listener << (arg == value ? "true" : "false");
        return arg == value;
    }
}

template <typename ARG, typename T>
bool compare_value_vector(const std::vector<ARG>& arg,
                          const std::vector<T>& value,
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
