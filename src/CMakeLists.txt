FILE (GLOB_RECURSE project_HEADERS *.h *.hpp)
SET (project_LIBS ${LIBCRAFTER_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(whosup ${project_HEADERS} main.cpp ARPTableReader.cpp)
TARGET_LINK_LIBRARIES(whosup ${project_LIBS})

ADD_EXECUTABLE(whosup_collectd ${project_HEADERS} collectd.cpp ARPTableReader.cpp)
TARGET_LINK_LIBRARIES(whosup_collectd ${project_LIBS})

INSTALL(TARGETS whosup DESTINATION bin)
INSTALL(TARGETS whosup_collectd DESTINATION bin)
