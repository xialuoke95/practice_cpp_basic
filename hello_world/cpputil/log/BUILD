cc_library(
    name="log",
    srcs=["log.cpp", "info_log.cpp"],
    deps=[
        "cpp3rdlib/log4cxx:0.10.0@//cpp3rdlib/log4cxx:log4cxx",
    ],
    optimize=["-O3", "-g", "-Wall", "-Wextra", "-Werror"],
)

cc_library(
    name="databus_logger",
    srcs=["databus_appender.cpp", "databus_writer.cpp"],
    deps=[
        ":log",
        "cpputil/databusclient:1.1.0@//cpputil/databusclient:databus",
    ],
    optimize=["-O3", "-g", "-Wall", "-Wextra", "-Werror"],
    # to make sure the symbols are linked into binary
    link_all_symbols=True,
)
