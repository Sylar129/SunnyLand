# Compiler and compiler options configuration

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

if(MSVC)
  set(PROJECT_WARNINGS
      /W4
      /w14242 # 'identifier': conversion from 'type1' to 'type2', possible loss
              # of data
      /w14254 # 'operator': conversion from 'type1:field_bits' to
              # 'type2:field_bits', possible loss of data
      /w14263 # 'function': member function does not override any base class
              # virtual member function
      /w14265 # 'classname': class has virtual functions, but destructor is not
              # virtual
      /w14287 # 'operator': unsigned/negative constant mismatch
      /we4289 # nonstandard extension: loop control variable used outside the
              # for-loop scope
      /w14296 # 'operator': expression is always 'boolean_value'
      /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
      /w14545 # expression before comma evaluates to a function which is missing
              # an argument list
      /w14546 # function call before comma missing argument list
      /w14547 # 'operator': operator before comma has no effect; expected
              # operator with side-effect
      /w14549 # 'operator': operator before comma has no effect; did you intend
              # 'operator'?
      /w14555 # expression has no effect; expected expression with side-effect
      /w14619 # pragma warning: there is no warning number 'number'
      /w14640 # enable warning on thread un-safe static member initialization
      /w14826 # conversion from 'type1' to 'type2' is sign-extended
      /w14905 # wide string literal cast to 'LPSTR'
      /w14906 # string literal cast to 'LPWSTR'
      /w14928 # illegal copy-initialization; more than one user-defined
              # conversion has been implicitly applied
      /permissive- # standards conformance mode
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(PROJECT_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn if a variable declaration shadows one from a parent
               # context
      -Wnon-virtual-dtor # warn if a class with virtual functions has a
                         # non-virtual destructor
      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      -Woverloaded-virtual # warn if you overload (not override) a virtual
                           # function
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicitly promoted to double
      -Wformat=2 # warn on security issues around functions that format output
      -Wimplicit-fallthrough # warn on statements that fallthrough without an
                             # explicit annotation
  )
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(PROJECT_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn if a variable declaration shadows one from a parent
               # context
      -Wnon-virtual-dtor # warn if a class with virtual functions has a
                         # non-virtual destructor
      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      -Woverloaded-virtual # warn if you overload (not override) a virtual
                           # function
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicitly promoted to double
      -Wformat=2 # warn on security issues around functions that format output
      -Wimplicit-fallthrough # warn on statements that fallthrough without an
                             # explicit annotation
      -Wmisleading-indentation # warn if indentation implies blocks where blocks
                               # do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations used where bitwise were
                   # probably wanted
      -Wuseless-cast # warn if you perform a cast to the same type
      -Wsuggest-override # warn if an overridden member function is not marked
                         # 'override' or 'final'
  )
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
  message(
    AUTHOR_WARNING
      "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
endif()
