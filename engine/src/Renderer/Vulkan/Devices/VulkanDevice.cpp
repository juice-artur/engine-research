#include "VulkanDevice.h"


bool VulkanDevice::Create(VkInstance& instance, VkSurfaceKHR& surface)
{
	if(!SelectPhysicalDevice(instance, surface))
	{
		return false;
	}
	LOG_INFO("Physical device selected.");
	return true;
}

VulkanDevice::~VulkanDevice()
{
}

bool VulkanDevice::SelectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface)
{
	uint32 physicalDeviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, 0));
	if (physicalDeviceCount == 0) {
		LOG_CRITICAL("No devices which support Vulkan were found.");
		return false;
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
	for (uint32 i = 0; i < physicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

		VkPhysicalDeviceMemoryProperties memory;
		vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memory);

		VulkanPhysicalDeviceRequirements requirements = {};
		requirements.graphics = true;
		requirements.present = true;
		requirements.transfer = true;
		requirements.samplerAnisotropy = true;
		requirements.discreteGpu = true;
		requirements.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		VulkanPhysicalDeviceQueueFamilyInfo queueInfo = {};
		bool result = PhysicalDeviceMeetsRequirements(
			physicalDevices[i],
			surface,
			&properties,
			&features,
			&queueInfo,
			&requirements);

		if (result) {
			LOG_INFO("Selected device: {0}.", properties.deviceName);
			// GPU type, etc.
			switch (properties.deviceType) {
			default:
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				LOG_TRACE("GPU type is Unknown.");
				break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				LOG_TRACE("GPU type is Integrated.");
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				LOG_TRACE("GPU type is Descrete.");
				break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				LOG_TRACE("GPU type is Virtual.");
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				LOG_TRACE("GPU type is CPU.");
				break;
			}

			// Vulkan API version.
			LOG_TRACE(
				"Vulkan API version:{0}.{1}.{2}",
				VK_VERSION_MAJOR(properties.apiVersion),
				VK_VERSION_MINOR(properties.apiVersion),
				VK_VERSION_PATCH(properties.apiVersion));

			// Memory information
			for (uint32 j = 0; j < memory.memoryHeapCount; ++j) {
				float memory_size_gib = (((float)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
				if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
					LOG_TRACE("Local GPU memory: {0} GiB", memory_size_gib);
				}
				else {
					LOG_TRACE("Shared System memory: {0} GiB", memory_size_gib);
				}
			}

			_physicalDevice = physicalDevices[i];
			graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
			presentQueueIndex = queueInfo.presentFamilyIndex;
			transferQueueIndex = queueInfo.transferFamilyIndex;
			break;
		}
	}

	// Ensure a device was selected
	if (!_physicalDevice) {
		LOG_CRITICAL("No physical devices were found which meet the requirements.");
		return false;
	}

	return true;
}

bool VulkanDevice::PhysicalDeviceMeetsRequirements(VkPhysicalDevice& device, VkSurfaceKHR& surface, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features, VulkanPhysicalDeviceQueueFamilyInfo* queueInfo, const VulkanPhysicalDeviceRequirements* requirements)
{
	queueInfo->graphicsFamilyIndex = -1;
	queueInfo->presentFamilyIndex = -1;
	queueInfo->computeFamilyIndex = -1;
	queueInfo->transferFamilyIndex = -1;

	// Discrete GPU?
	if (requirements->discreteGpu) {
		if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			LOG_TRACE("Device is not a discrete GPU, and one is required. Skipping.");
			return false;
		}
	}

	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, 0);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	uint16 minTransferScore = 255;
	for (uint32 i = 0; i < queueFamilyCount; ++i) {
		uint16 currentTransferScore = 0;

		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueInfo->graphicsFamilyIndex = i;
			++currentTransferScore;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queueInfo->computeFamilyIndex = i;
			++currentTransferScore;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			if (currentTransferScore <= minTransferScore) {
				minTransferScore = currentTransferScore;
				queueInfo->transferFamilyIndex = i;
			}
		}

		VkBool32 supports_present = VK_FALSE;
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
		if (supports_present) {
			queueInfo->presentFamilyIndex = i;
		}
	}

	if (
		(!requirements->graphics || (requirements->graphics && queueInfo->graphicsFamilyIndex != -1)) &&
		(!requirements->present || (requirements->present && queueInfo->presentFamilyIndex != -1)) &&
		(!requirements->compute || (requirements->compute && queueInfo->computeFamilyIndex != -1)) &&
		(!requirements->transfer || (requirements->transfer && queueInfo->transferFamilyIndex != -1))) {

		LOG_TRACE("Graphics Family Index: {0}", queueInfo->graphicsFamilyIndex);
		LOG_TRACE("Present Family Index:  {0}", queueInfo->presentFamilyIndex);
		LOG_TRACE("Transfer Family Index: {0}", queueInfo->transferFamilyIndex);

		VulkanDeviceQuerySwapchainSupport(
			device,
			surface,
			&swapchainSupport);

		if (swapchainSupport.formatCount < 1 || swapchainSupport.presentModeCount < 1) {
			if (!swapchainSupport.formats.empty()) {
				swapchainSupport.formats.clear();
			}
			if (!swapchainSupport.presentModes.empty()) {
				swapchainSupport.presentModes.clear();
			}
			LOG_TRACE("Required swapchain support not present, skipping device.");
			return false;
		}

		// Device extensions.
		if (!requirements->device_extension_names.empty()) {
			uint32 availableExtensionCount = 0;
			std::vector<VkExtensionProperties> availableExtensions;
			VK_CHECK(vkEnumerateDeviceExtensionProperties(
				device,
				0,
				&availableExtensionCount,
				0));
			if (availableExtensionCount != 0) {
				availableExtensions.resize(availableExtensionCount);
				VK_CHECK(vkEnumerateDeviceExtensionProperties(
					device,
					0,
					&availableExtensionCount,
					availableExtensions.data()));

				uint32 required_extension_count =requirements->device_extension_names.size();
				for (uint32 i = 0; i < required_extension_count; ++i) {
					bool found = false;
					for (uint32 j = 0; j < availableExtensionCount; ++j) {
						if (std::strcmp(requirements->device_extension_names[i], availableExtensions[j].extensionName) == 0) {
							found = true;
							break;
						}
					}

					if (!found) {
						LOG_TRACE("Required extension not found: {0}, skipping device.", requirements->device_extension_names[i]);
						availableExtensions.clear();
						return false;
					}
				}
			}
			availableExtensions.clear();
		}

		// Sampler anisotropy
		if (requirements->samplerAnisotropy && !features->samplerAnisotropy) {
			LOG_INFO("Device does not support samplerAnisotropy, skipping.");
			return false;
		}

		return true;
	}

	return false;
}

void VulkanDevice::VulkanDeviceQuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanSwapchainSupportInfo* outSupportInfo)
{
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		physicalDevice,
		surface,
		&outSupportInfo->capabilities));

	// Surface formats
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
		physicalDevice,
		surface,
		&outSupportInfo->formatCount,
		0));

	if (outSupportInfo->formatCount != 0) {
		if (outSupportInfo->formats.empty()) {
			outSupportInfo->formats.resize(outSupportInfo->formatCount);
		}
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice,
			surface,
			&outSupportInfo->formatCount,
			outSupportInfo->formats.data()));

		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
			physicalDevice,
			surface,
			&outSupportInfo->presentModeCount,
			0));
		if (outSupportInfo->presentModeCount != 0) {
			if (outSupportInfo->presentModes.empty()) {
				outSupportInfo->presentModes.resize(outSupportInfo->presentModeCount);
			}
			VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice,
				surface,
				&outSupportInfo->presentModeCount,
				outSupportInfo->presentModes.data()));
		}
	}
}


