
//Note vulkan test needs to come before perfdoc...
#include "vulkan_test.hpp"
#include "perfdoc.hpp"
#include "util.hpp"
#include <functional>
#include <memory>
using namespace MPD;
using namespace std;

class PipelineTest : public VulkanTestHelper
{
public:
	bool testFlag(bool positiveTest)
	{
		VkAttachmentDescription attachments[1] = {};
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
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

		VkGraphicsPipelineCreateInfo gpci = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		gpci.renderPass = renderPass;
		gpci.pColorBlendState = &blend;
		gpci.flags = positiveTest ? VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT : 0;

		auto pipeline = make_shared<Pipeline>(device);
		pipeline->initGraphics(vertCode, sizeof(vertCode), fragCode, sizeof(fragCode), &gpci);

		vkDestroyRenderPass(device, renderPass, 0);

		uint32_t count = getCount(MESSAGE_CODE_PIPELINE_OPTIMISATION_DISABLED);
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
		if (!testFlag(false))
			return false;

		if (!testFlag(true))
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
	return new PipelineTest;
}
