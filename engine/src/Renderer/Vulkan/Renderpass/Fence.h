#include "vulkan\vulkan.h"
#include <Renderer\Vulkan\Devices\VulkanDevice.h>
class Fence
{
public:
    VkFence handle;
    bool isSignaled;

    void Create(VulkanDevice& device, bool createSignaled);
    void Destroy(VulkanDevice& device);
    bool Wait(VulkanDevice& device, uint64 timeoutNs);
    void Reset(VulkanDevice& device);
private:

};
