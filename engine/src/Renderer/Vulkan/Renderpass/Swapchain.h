#pragma once
#include <vulkan\vulkan.h>
#include <Platform\Common\Types.h>
#include <vector>
#include <Renderer\Vulkan\Images\Image.h>
#include <glm\ext\vector_int2.hpp>
#include "Renderer\Vulkan\Devices\VulkanDevice.h"
#include "Framebuffer.h"
class Swapchain
{
public:
	void Create(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend);
	bool AcquireNextImageIndex(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend, uint64 timeout, VkSemaphore imageAvailableSemaphore, VkFence fence, uint32* outImageIndex);
	void Destroy(VulkanDevice& device);
	void Present(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend, VkSemaphore renderCompleteSemaphore, uint32 presentImageIndex, VkQueue graphicsQueue,
		VkQueue presentQueue);
	void Recreate(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend);
	Swapchain() {};
	uint32 GetImageCount();
	std::vector<VkImage> images;
	std::vector <VkImageView> views;
	std::vector<Framebuffer> framebuffers;
	Image depthAttachment;
	uint16 maxFramesInFlight;
	VkSurfaceFormatKHR GetImageFormat();
private:
	VkSurfaceFormatKHR imageFormat;
	VkSwapchainKHR handle;
	uint32 imageCount;
	uint32 curentFrame = 0;

};
