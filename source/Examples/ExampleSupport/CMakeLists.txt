# Shared plumbing for the examples: backend selection and data lookup.
# Not part of the engine -- nothing outside source/Examples links this.

longbow_add_library(ExampleSupport STATIC
    PUBLIC_DEPS
        LongBow::RenderDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
    FOLDER "Examples"
)

target_compile_definitions(ExampleSupport
    PRIVATE LONGBOW_EXAMPLE_DATA_DIR="${PROJECT_SOURCE_DIR}/data"
)
