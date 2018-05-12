cmake_minimum_required(VERSION 3.2)

set(PROJECT_VERSION_PATCH $ENV{TRAVIS_BUILD_NUMBER})

project(yamr VERSION 0.0.${PROJECT_VERSION_PATCH})

configure_file(version.h.in version.h @ONLY)

set(SRC
    main.cpp
    map.cpp
    map_reduce.cpp
    reduce.cpp
    shuffle.cpp
    )

set(HDR
    data.h
    map.h
    map_reduce.h
    my_handler_map.h
    my_handler_reduce.h
    reduce.h
    shuffle.h
    utilites.h
    )

#Program yamr
add_executable(yamr ${SRC} ${HDR})

set_target_properties(yamr PROPERTIES
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}
)

#Дополнительный свойста компилятора для всех проектов
set_target_properties( yamr PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wall
)

if(UNIX)

set_target_properties( yamr PROPERTIES
    COMPILE_OPTIONS -pthread
)
target_link_libraries( yamr pthread )

endif(UNIX)

install(TARGETS yamr RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT aquila-2004@yandex.ru)

include(CPack)
