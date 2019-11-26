
//Note vulkan test needs to come before perfdoc...
#include "vulkan_test.hpp"
#include "perfdoc.hpp"
#include "util.hpp"
#include <functional>
#include <memory>
using namespace MPD;
using namespace std;

class UncompressedTextureTest : public VulkanTestHelper
{
public:
	//test with uncompressed texture format
	bool testFormat(bool positiveTest, MessageCodes code)
	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 1;
		VkDescriptorPoolCreateInfo pci = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pci.maxSets = 1;
		pci.poolSizeCount = 1;
		pci.pPoolSizes = &poolSize;

		VkDescriptorPool pool;
		MPD_ASSERT_RESULT(vkCreateDescriptorPool(device, &pci, 0, &pool));

		VkDescriptorSetLayoutBinding dslb = {};
		dslb.binding = 0;
		dslb.descriptorCount = 1;
		dslb.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslb.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo dslci = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		dslci.bindingCount = 1;
		dslci.pBindings = &dslb;

		VkDescriptorSetLayout dsl;
		MPD_ASSERT_RESULT(vkCreateDescriptorSetLayout(device, &dslci, 0, &dsl));

		VkDescriptorSetAllocateInfo dsai = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		dsai.descriptorPool = pool;
		dsai.descriptorSetCount = 1;
		dsai.pSetLayouts = &dsl;

		VkDescriptorSet set;
		MPD_ASSERT_RESULT(vkAllocateDescriptorSets(device, &dsai, &set));

		VkImageCreateInfo ici = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		ici.arrayLayers = 1;
		ici.extent.width = 128;
		ici.extent.height = 128;
		ici.extent.depth = 1;
		ici.format = VK_FORMAT_R8G8B8A8_UNORM; //uncompressed format here
		ici.imageType = VK_IMAGE_TYPE_2D;
		ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ici.mipLevels = 1;
		ici.samples = VK_SAMPLE_COUNT_1_BIT;
		ici.tiling = VK_IMAGE_TILING_OPTIMAL;
		ici.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //usage flags don't indicate render target usage
		if (!positiveTest)
		{
			ici.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //add render target usage flag if we want a negative result
		}
		ici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ici.queueFamilyIndexCount = 0;

		VkImage image;
		MPD_ASSERT_RESULT(vkCreateImage(device, &ici, 0, &image));

		VkMemoryRequirements mr;
		vkGetImageMemoryRequirements(device, image, &mr);

		VkPhysicalDeviceMemoryProperties pdmp;
		vkGetPhysicalDeviceMemoryProperties(gpu, &pdmp);

		uint32_t index = -1;

		VkMemoryPropertyFlags prop = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		for (uint32_t c = 0; c < pdmp.memoryTypeCount; ++c)
		{
			if ((pdmp.memoryTypes[c].propertyFlags & prop) == prop && ((1 << c) & mr.memoryTypeBits))
			{
				index = c;
				break;
			}
		}

		VkMemoryAllocateInfo mai = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		mai.allocationSize = mr.size;
		mai.memoryTypeIndex = index;

		VkDeviceMemory mem;
		MPD_ASSERT_RESULT(vkAllocateMemory(device, &mai, 0, &mem));

		MPD_ASSERT_RESULT(vkBindImageMemory(device, image, mem, 0));

		VkImageViewCreateInfo ivci = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivci.image = image;
		ivci.format = ici.format;
		ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.layerCount = 1;
		ivci.subresourceRange.levelCount = 1;
		ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivci.components.r = VK_COMPONENT_SWIZZLE_R;
		ivci.components.g = VK_COMPONENT_SWIZZLE_G;
		ivci.components.b = VK_COMPONENT_SWIZZLE_B;
		ivci.components.a = VK_COMPONENT_SWIZZLE_A;

		VkImageView imageView;
		MPD_ASSERT_RESULT(vkCreateImageView(device, &ivci, 0, &imageView));

		setImageLayout(device, queue, VK_IMAGE_LAYOUT_UNDEFINED,
		               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image, 0, 1, 0, 1, VK_IMAGE_ASPECT_COLOR_BIT);

		VkSamplerCreateInfo sci = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		sci.addressModeU = sci.addressModeV = sci.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sci.anisotropyEnable = false;
		sci.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		sci.compareEnable = false;
		sci.magFilter = VK_FILTER_NEAREST;
		sci.minFilter = VK_FILTER_LINEAR;
		sci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSampler sampler;
		MPD_ASSERT_RESULT(vkCreateSampler(device, &sci, 0, &sampler));

		VkDescriptorImageInfo dii;
		dii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dii.imageView = imageView;
		dii.sampler = sampler;

		VkWriteDescriptorSet wds = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		wds.descriptorCount = 1;
		wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds.dstArrayElement = 0;
		wds.dstBinding = 0;
		wds.dstSet = set;
		wds.pImageInfo = &dii;
		vkUpdateDescriptorSets(device, 1, &wds, 0, 0);

		vkDestroyImageView(device, imageView, 0);
		vkDestroyImage(device, image, 0);
		vkFreeMemory(device, mem, 0);
		vkDestroySampler(device, sampler, 0);
		vkDestroyDescriptorSetLayout(device, dsl, 0);
		vkResetDescriptorPool(device, pool, 0);
		vkDestroyDescriptorPool(device, pool, 0);

		uint32_t count = getCount(code);
		resetCounts();

		if (positiveTest)
		{
			if (count != 1)
				return false;
		}
		else
		{
			if (count != 0)
				return false;
		}
		return true;
	}

	bool runTest() override
	{
		if (!testFormat(false, MESSAGE_CODE_UNCOMPRESSED_TEXTURE_USED))
			return false;
		
		if (!testFormat(true, MESSAGE_CODE_UNCOMPRESSED_TEXTURE_USED))
			return false;

		if (!testFormat(false, MESSAGE_CODE_NON_MIPMAPPED_TEXTURE_USED))
			return false;

		if (!testFormat(true, MESSAGE_CODE_NON_MIPMAPPED_TEXTURE_USED))
			return false;

		return true;
	}

	virtual bool initialize()
	{
		return true;
	}
};

VulkanTestHelper *MPD::createTest()
{
	return new UncompressedTextureTest;
}
