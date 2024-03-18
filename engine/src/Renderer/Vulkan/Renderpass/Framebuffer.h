#include <Platform\Common\Types.h>
#include "vulkan\vulkan.h"
#include <vector>
#include "Renderpass.h"
#include <glm\ext\vector_int2.hpp>
#include "Renderer\Vulkan\Devices\VulkanDevice.h"

class Framebuffer
{
public:
	void Create(std::vector <VkImageView>& attachments, VulkanDevice& device, Renderpass* renderpass, uint32  attachmentCount, glm::ivec2 extend);
    void Destroy(VulkanDevice& device);
    VkFramebuffer handle;
    uint32 attachmentCount;
    std::vector <VkImageView> attachments;
    Renderpass* renderpass;
private:

};
