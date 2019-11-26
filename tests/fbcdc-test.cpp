
//Note vulkan test needs to come before perfdoc...
#include "vulkan_test.hpp"
#include "perfdoc.hpp"
#include "util.hpp"
#include <functional>
#include <memory>
using namespace MPD;
using namespace std;

class FBCDCTest : public VulkanTestHelper
{
public:
	bool testFBCDCDepthTex(bool positiveTest)
	{
		VkAttachmentDescription attachments[1] = {};
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].format = positiveTest ? VK_FORMAT_D16_UNORM : VK_FORMAT_D32_SFLOAT;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		const VkAttachmentReference attachment = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass = {};
		subpass.colorAttachmentCount = 0;
		subpass.pDepthStencilAttachment = &attachment;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkRenderPassCreateInfo info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		info.attachmentCount = 1;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.pAttachments = &attachments[0];

		VkRenderPass renderPass;
		MPD_ASSERT_RESULT(vkCreateRenderPass(device, &info, nullptr, &renderPass));

		static const uint32_t vertCode[] =
#include "quad_no_attribs.vert.inc"
		    ;

		static const uint32_t fragCode[] =
#include "quad_no_output.frag.inc"
		    ;

		VkPipelineColorBlendStateCreateInfo blend = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		blend.attachmentCount = 0;

		VkGraphicsPipelineCreateInfo gpci = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		gpci.renderPass = renderPass;
		gpci.pColorBlendState = &blend;

		auto pipeline = make_shared<Pipeline>(device);
		pipeline->initGraphics(vertCode, sizeof(vertCode), fragCode, sizeof(fragCode), &gpci);

		auto cmdBuf = make_shared<CommandBuffer>(device);
		cmdBuf->initPrimary();

		VkImageCreateInfo ici = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		ici.arrayLayers = 1;
		ici.extent.width = 128;
		ici.extent.height = 128;
		ici.extent.depth = 1;
		ici.format = positiveTest ? VK_FORMAT_D16_UNORM : VK_FORMAT_D32_SFLOAT;
		ici.imageType = VK_IMAGE_TYPE_2D;
		ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ici.mipLevels = 1;
		ici.samples = VK_SAMPLE_COUNT_1_BIT;
		ici.tiling = VK_IMAGE_TILING_OPTIMAL;
		ici.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; 
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
		ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
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

		setImageLayout(device, queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, image, 0, 1,
		               0, 1, VK_IMAGE_ASPECT_DEPTH_BIT);

		VkFramebufferCreateInfo fbinf = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbinf.renderPass = renderPass;
		fbinf.attachmentCount = 1;
		fbinf.pAttachments = &imageView;
		fbinf.width = 128;
		fbinf.height = 128;
		fbinf.layers = 1;

		VkFramebuffer fbo;
		MPD_ASSERT_RESULT(vkCreateFramebuffer(device, &fbinf, nullptr, &fbo));

		VkCommandBufferBeginInfo cbbi = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		MPD_ASSERT_RESULT(vkBeginCommandBuffer(cmdBuf->commandBuffer, &cbbi));

		vkCmdBindPipeline(cmdBuf->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

		VkClearValue cv = {};

		VkRenderPassBeginInfo rpbi = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbi.renderPass = renderPass;
		rpbi.renderArea.offset.x = 0;
		rpbi.renderArea.offset.y = 0;
		rpbi.renderArea.extent.width = 128;
		rpbi.renderArea.extent.height = 128;
		rpbi.framebuffer = fbo;
		rpbi.clearValueCount = 1;
		rpbi.pClearValues = &cv;

		vkCmdBeginRenderPass(cmdBuf->commandBuffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = 128;
		viewport.height = 128;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;

		vkCmdSetViewport(cmdBuf->commandBuffer, 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = 128;
		scissor.extent.height = 128;

		vkCmdSetScissor(cmdBuf->commandBuffer, 0, 1, &scissor);

		vkCmdDraw(cmdBuf->commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdBuf->commandBuffer);

		MPD_ASSERT_RESULT(vkEndCommandBuffer(cmdBuf->commandBuffer));

		VkSubmitInfo submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &cmdBuf->commandBuffer;
		vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkDestroyImageView(device, imageView, 0);
		vkDestroyImage(device, image, 0);
		vkFreeMemory(device, mem, 0);
		vkDestroyRenderPass(device, renderPass, 0);
		vkDestroyFramebuffer(device, fbo, 0);

		uint32_t count = getCount(MESSAGE_CODE_NO_FBCDC);
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

	bool testFBCDCColorTex(bool positiveTest)
	{
		VkAttachmentDescription attachments[1] = {};
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].samples = positiveTest ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_1_BIT;
		attachments[0].format = VK_FORMAT_R16_SFLOAT;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		const VkAttachmentReference attachment = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass = {};
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachment;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkRenderPassCreateInfo info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		info.attachmentCount = 1;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.pAttachments = &attachments[0];

		VkRenderPass renderPass;
		MPD_ASSERT_RESULT(vkCreateRenderPass(device, &info, nullptr, &renderPass));

		static const uint32_t vertCode[] =
#include "quad_no_attribs.vert.inc"
		    ;

		static const uint32_t fragCode[] =
#include "quad.frag.inc"
		    ;

		VkPipelineColorBlendAttachmentState att = {};
		att.blendEnable = true;
		att.colorWriteMask = 0xf;

		VkPipelineColorBlendStateCreateInfo blend = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		blend.attachmentCount = 1;
		blend.pAttachments = &att;

		VkPipelineMultisampleStateCreateInfo msci = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		msci.rasterizationSamples = positiveTest ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_1_BIT;

		VkGraphicsPipelineCreateInfo gpci = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		gpci.renderPass = renderPass;
		gpci.pColorBlendState = &blend;
		gpci.pMultisampleState = &msci;

		auto pipeline = make_shared<Pipeline>(device);
		pipeline->initGraphics(vertCode, sizeof(vertCode), fragCode, sizeof(fragCode), &gpci);

		auto cmdBuf = make_shared<CommandBuffer>(device);
		cmdBuf->initPrimary();

		VkImageCreateInfo ici = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		ici.arrayLayers = 1;
		ici.extent.width = 128;
		ici.extent.height = 128;
		ici.extent.depth = 1;
		ici.format = VK_FORMAT_R16_SFLOAT;
		ici.imageType = VK_IMAGE_TYPE_2D;
		ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ici.mipLevels = 1;
		ici.samples = positiveTest ? VK_SAMPLE_COUNT_4_BIT : VK_SAMPLE_COUNT_1_BIT;
		ici.tiling = VK_IMAGE_TILING_OPTIMAL;
		ici.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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

		setImageLayout(device, queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		               image, 0, 1, 0, 1, VK_IMAGE_ASPECT_COLOR_BIT);

		VkFramebufferCreateInfo fbinf = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbinf.renderPass = renderPass;
		fbinf.attachmentCount = 1;
		fbinf.pAttachments = &imageView;
		fbinf.width = 128;
		fbinf.height = 128;
		fbinf.layers = 1;

		VkFramebuffer fbo;
		MPD_ASSERT_RESULT(vkCreateFramebuffer(device, &fbinf, nullptr, &fbo));

		VkCommandBufferBeginInfo cbbi = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		MPD_ASSERT_RESULT(vkBeginCommandBuffer(cmdBuf->commandBuffer, &cbbi));

		vkCmdBindPipeline(cmdBuf->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

		VkClearValue cv = {};

		VkRenderPassBeginInfo rpbi = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbi.renderPass = renderPass;
		rpbi.renderArea.offset.x = 0;
		rpbi.renderArea.offset.y = 0;
		rpbi.renderArea.extent.width = 128;
		rpbi.renderArea.extent.height = 128;
		rpbi.framebuffer = fbo;
		rpbi.clearValueCount = 1;
		rpbi.pClearValues = &cv;

		vkCmdBeginRenderPass(cmdBuf->commandBuffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = 128;
		viewport.height = 128;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;

		vkCmdSetViewport(cmdBuf->commandBuffer, 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = 128;
		scissor.extent.height = 128;

		vkCmdSetScissor(cmdBuf->commandBuffer, 0, 1, &scissor);

		vkCmdDraw(cmdBuf->commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdBuf->commandBuffer);

		MPD_ASSERT_RESULT(vkEndCommandBuffer(cmdBuf->commandBuffer));

		VkSubmitInfo submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &cmdBuf->commandBuffer;
		vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkDestroyImageView(device, imageView, 0);
		vkDestroyImage(device, image, 0);
		vkFreeMemory(device, mem, 0);
		vkDestroyRenderPass(device, renderPass, 0);
		vkDestroyFramebuffer(device, fbo, 0);

		uint32_t count = getCount(MESSAGE_CODE_NO_FBCDC);
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
		if (!testFBCDCDepthTex(false))
			return false;

		if (!testFBCDCDepthTex(true))
			return false;

		if (!testFBCDCColorTex(false))
			return false;

		if (!testFBCDCColorTex(true))
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
	return new FBCDCTest;
}
