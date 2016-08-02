package(default_visibility = ["//visibility:public"])

load("/tools/rules/genproto",  "proto_library")

proto_library(
    name = "proto_type",
    src = "Type.proto",
    includes = ["heron/query/src/cpp/types"],
    gen_cc = 1,
)

proto_library(
    name = "proto_typed_value",
    src = "TypedValue.proto",
    includes = ["heron/query/src/cpp/types"],
    deps = [":proto_type"],
    gen_cc = 1,
)

proto_library(
    name = "proto_operation",
    src = "operations/Operation.proto",
    includes = ["heron/query/src/cpp/types"],
    deps = [":proto_type"],
    gen_cc = 1,
)

cc_library(
    name = "port-cxx",
    srcs =  glob(["port/*.cpp"]),
    hdrs =  glob(["port/*.hpp"]) + glob(["port/*.h"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        "//heron/query/src/cpp/utility:utility-cxx",
    ],
)
 
cc_library(
    name = "types-cxx",
    srcs =  glob(["*.cpp"]),
    hdrs =  glob(["*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":proto_type_cc",
        ":proto_typed_value_cc",
        ":port-cxx",
        "//heron/query/src/cpp/common:common-cxx",
    ],
    linkstatic = 1,
)


cc_library(
    name = "containers-cxx",
    srcs =  glob(["containers/*.cpp"]),
    hdrs =  glob(["containers/*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":types-cxx",
    ],
    linkstatic = 1,
)

cc_library(
    name = "operations-cxx",
    srcs = ["operations/Operation.cpp"],
    hdrs = ["operations/Operation.hpp"],
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":proto_operation_cc",
        "//heron/query/src/cpp/utility:utility-cxx",
        "//heron/query/src/cpp/common:common-cxx",
    ],
    linkstatic = 1,
)

cc_library(
    name = "binary_operations-cxx",
    srcs =  glob(["operations/binary_operations/*.cpp"]),
    hdrs =  glob(["operations/binary_operations/*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":operations-cxx",
        ":containers-cxx", 
        ":types-cxx",
        "//heron/query/src/cpp/utility:utility-cxx",
        "//heron/query/src/cpp/common:common-cxx",
    ],
    linkstatic = 1,
)

cc_library(
    name = "comparisons-cxx",
    srcs =  glob(["operations/comparisons/*.cpp"]),
    hdrs =  glob(["operations/comparisons/*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":operations-cxx",
        ":containers-cxx", 
        ":types-cxx",
        "//heron/query/src/cpp/utility:utility-cxx",
        "//heron/query/src/cpp/common:common-cxx",
        "//third_party/re2:re2-cxx",
    ],
    linkstatic = 1,
)

cc_library(
    name = "unary_operations-cxx",
    srcs =  glob(["operations/unary_operations/*.cpp"]),
    hdrs =  glob(["operations/unary_operations/*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/types",
        "-Ithird_party",
    ],
    deps = [
        ":operations-cxx",
        ":containers-cxx", 
        ":types-cxx",
        "//heron/query/src/cpp/utility:utility-cxx",
        "//heron/query/src/cpp/common:common-cxx",
    ],
    linkstatic = 1,
)
