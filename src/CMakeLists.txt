# CMakeList.txt : CMake project for hello, include source and define project specific logic here.

add_executable(hello
	"hello.c"
	"util.c"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET hello PROPERTY CXX_STANDARD 20)
endif()
