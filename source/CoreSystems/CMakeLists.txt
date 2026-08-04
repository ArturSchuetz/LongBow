# Math, logging, timing, geometry and vertex-attribute primitives.
# The bottom of the dependency graph -- everything else builds on this.

longbow_add_library(CoreSystems
    PUBLIC_DEPS
        LongBow::Profiler
    FOLDER "Engine"
)
