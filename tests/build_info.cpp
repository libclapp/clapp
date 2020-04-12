#include <clapp/build_info.h>
#include <gmock/gmock.h>

TEST(buildInfo, version) {
    std::stringstream ss;
    ss << clapp::build_info::version_major << "."
       << clapp::build_info::version_minor << "."
       << clapp::build_info::version_patch;
    ASSERT_THAT(clapp::build_info::version, testing::StrEq(ss.str()));
}

TEST(buildInfo, buildInfoString) {
    std::stringstream ss;
    ss << clapp::build_info::project_name << " v." << clapp::build_info::version
       << " " << clapp::build_info::build_type
       << "-Build: " << clapp::build_info::git_branch << ":"
       << clapp::build_info::git_hash << " ("
       << clapp::build_info::git_description << ")";
    ASSERT_THAT(clapp::build_info::build_info_string, testing::StrEq(ss.str()));
}
