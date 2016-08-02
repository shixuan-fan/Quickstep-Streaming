package(default_visibility = ["//visibility:public"])

cc_library(
    name = "basics-cxx",
    srcs = [
        "empty.cc",
    ],
    hdrs = [
        "Common.hpp",
    ],
    deps = [
        "//third_party/farmhash:farmhash-cxx",
        "//third_party/glog:glog-cxx",
        "//third_party/gperftools:tcmalloc-cxx",
        "//third_party/protobuf:protobuf-cxx",
    ],
    linkstatic = 1,
)
