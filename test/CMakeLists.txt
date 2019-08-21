###############
# Build tests
###############


################
# Add tests to ctest
################

ADD_EXECUTABLE(er_test test_er.c)
TARGET_LINK_LIBRARIES(er_test ${RANKSTR_EXTERNAL_LIBS} er)
ER_ADD_TEST(er_test 256 "")
#ADD_TEST(NAME er_test COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 3 -N3 ./er_test)


####################
# make a verbose "test" target named "check"
####################

ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
