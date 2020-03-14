include_directories(./inc)
AUX_SOURCE_DIRECTORY(./src BASE_SRCS)
ADD_LIBRARY(timer SHARED ${BASE_SRCS})