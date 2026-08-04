# Images, meshes, materials and point clouds, plus the loaders for them
# (BMP/PNG/TGA/HDR, OBJ/PLY, XYZ/XCN/XTC/BIN) and their resource managers.

longbow_add_library(Resources
    PUBLIC_DEPS
        LongBow::CoreSystems
    PRIVATE_DEPS
        LongBow::Platform
        LongBow::LoadPNG
    FOLDER "Engine"
)
