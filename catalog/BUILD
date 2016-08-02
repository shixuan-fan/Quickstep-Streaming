package(default_visibility = ["//visibility:public"])

load("/tools/rules/genproto", "proto_library")

proto_library(
    name = "proto_catalog",
    src = "Catalog.proto",
    includes = ["heron/query/src/cpp/catalog:heron/query/src/cpp/types"],
    deps = [":proto_type"],
    gen_cc = 1,
)

cc_library(
    name = "catalog-cxx",
    srcs =  glob(["*.cpp"]),
    hdrs =  glob(["*.hpp"]),
    copts = [
        "-Iheron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp",
        "-I$(GENDIR)/heron/query/src/cpp/catalog",
        "-Ithird_party",
    ],
    deps = [
        ":proto_catalog_cc",
        "//heron/query/src/cpp/basics:basics-cxx",
        "//heron/query/src/cpp/common:common-cxx",
    ],
    linkstatic = 1,
)
