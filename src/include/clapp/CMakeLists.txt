configure_file(build_info.h.in build_info.h)
install(FILES  "${CMAKE_CURRENT_SOURCE_DIR}/argument.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/argument.hpp"
               "${CMAKE_CURRENT_BINARY_DIR}/build_info.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/exception.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/filesystem.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/main_parser.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/option.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/option.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/option_container.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/option_container.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser_container.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser_container.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser_types.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/parser_types.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/sub_parser.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/sub_parser.hpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/type_traits.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/value.h"
               "${CMAKE_CURRENT_SOURCE_DIR}/value.hpp"
               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clapp/)
