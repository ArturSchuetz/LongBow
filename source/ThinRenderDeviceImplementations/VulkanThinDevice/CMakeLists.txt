# Vulkan behind the thin interface.
#
# Vulkan 1.3 is the floor: dynamic rendering and synchronisation2 are core
# there and the interface is built on both, and timeline semaphores from 1.2
# are its only fence type.

find_package(Vulkan REQUIRED)

longbow_add_plugin(VulkanThinDevice
    PRIVATE_DEPS
        LongBow::CoreSystems
        LongBow::ThinRenderDevice
        Vulkan::Vulkan
    FOLDER "Backends/Thin"
)
