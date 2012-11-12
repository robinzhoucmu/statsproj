cmake_minimum_required(VERSION 2.8)


FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
FIND_PACKAGE(X11 REQUIRED)

set(HOG_DIR "HOG_linux")

file(GLOB HOG_SRC 
  ${HOG_DIR}/src/hog_wrappers.cpp
  ${HOG_DIR}/src/hog_extractor.cpp
  ${HOG_DIR}/src/integral_histogram.cpp
)
#file(GLOB HOG_INCLUDE ${HOG_DIR}/include/*.h)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${HOG_DIR}/include)



#ADD_LIBRARY(compiled_src_files STATIC
# AnnotationReader.cpp ${HOG_SRC}
# )

add_executable(reader testAnnotationReader.cpp AnnotationReader.cpp ${HOG_SRC})

TARGET_LINK_LIBRARIES( reader ${Boost_LIBRARIES} ${X11_LIBRARIES})
