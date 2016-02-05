%module test_external_c
%{
note
    description: "Test class for Eiffel SWIG plug-in(C only)"
    author: "Louis Marchand"
    date: "Thu, 02 Apr 2015 02:40:10 +0000"
    revision: "2.0"

class
    TEST_EXTERNAL_C

feature -- C external

%}

%feature("h_file", "<test.h>");                         // Specifie the .h file to include in Eiffel external feature

typedef unsigned int my_uint;                           // Typedef are managed by swig
typedef bool my_bool;

#define NOT_PARSED                                      // Defined constant without values are not wrap

%feature("numeric_define_type", "int");                 // You can specified the type of numeric constant

#define MY_CONSTANT -2

%feature("numeric_define_type", "unsigned int");

#define MY_CONSTANT_2 0x4F3A

my_bool my_function(my_uint arg1, int arg2);            // Function are wrap

%feature("prefix", "c_");                               // You can specifie a prefix for generated Eiffel external features

void my_function_with_pointer(char* str);

%feature("sufix", "_ext");                              // You can specified a sufix for generated Eiffel external features

void my_function_with_array( const my_uint arg[16] );   // Array as function argument are transformed into pointer

%feature("prefix", "");
%feature("sufix", "");

int my_variable;                                        // Global variable are wrap with a getter and a setter

struct my_struct {                                      // Struct are wrap with a creator, a destructor and a sizeof feature
   int member1;                                         // struct member are wrap with a getter and a setter
   my_uint member2;
};

typedef struct my_other_struct {
   int member1;
   my_uint* member2;
} struct_alias;                                         // Typedef struct are managed by swig


union my_union {                                        // Union seems to be managed as a struct
    int     element1;
    my_uint element2;
    char*   element3;
};

typedef union my_other_union {
    int     element1;
    my_uint element2;
    char*   element3;
} union_alias;

%{
end
%}
