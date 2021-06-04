###############
# Build tests
###############
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

IF(ER_LINK_STATIC)
   SET(er_lib er-static)
ELSE()
   SET(er_lib er)
ENDIF()

ADD_EXECUTABLE(test_config test_config.c)
TARGET_LINK_LIBRARIES(test_config ${er_lib})

ADD_EXECUTABLE(er_test test_er.c)
TARGET_LINK_LIBRARIES(er_test ${ER_EXTERNAL_LIBS} ${er_lib})

################
# Add tests to ctest
################

ER_ADD_TEST(er_test 256 "")
#ADD_TEST(NAME er_test COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 3 -N3 ./er_test)

ADD_TEST(NAME test_config COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 1 ./test_config)

####################
# make a verbose "test" target named "check"
####################

ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
