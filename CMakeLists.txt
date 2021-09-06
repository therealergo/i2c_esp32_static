# Component source directories
set(COMPONENT_SRCDIRS
)
# Component source files
set(COMPONENT_SRCS
    "i2c_esp32_static.c"
)
# Component include directories
set(COMPONENT_ADD_INCLUDEDIRS
    .
)

# ESP-IDF components this component depends on
set(COMPONENT_REQUIRES
    driver
)
set(COMPONENT_PRIV_REQUIRES
)

# Setup this component with ESP-IDF
register_component()