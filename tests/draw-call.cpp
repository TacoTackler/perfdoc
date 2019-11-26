
//Note vulkan test needs to come before perfdoc...
#include "vulkan_test.hpp"
#include "perfdoc.hpp"
#include "util.hpp"
#include <functional>
#include <memory>
using namespace MPD;
using namespace std;

class DrawCallTest : public VulkanTestHelper
{
public:
	//test with uncompressed texture format
	bool testNonIndexed(bool positiveTest)
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

		auto pipeline = make_shared<Pipeline>(device);
		pipeline->initGraphics(vertCode, sizeof(vertCode), fragCode, sizeof(fragCode), &gpci);

		auto cmdBuf = make_shared<CommandBuffer>(device);
		cmdBuf->initPrimary();

		auto tex = make_shared<Texture>(device);
		tex->initRenderTarget2D(128, 128, VK_FORMAT_R8G8B8A8_UNORM);

		setImageLayout(device, queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, tex->image,
		               0, 1, 0, 1, VK_IMAGE_ASPECT_COLOR_BIT);

		VkFramebufferCreateInfo fbinf = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbinf.renderPass = renderPass;
		fbinf.attachmentCount = 1;
		fbinf.pAttachments = &tex->view;
		fbinf.width = tex->width;
		fbinf.height = tex->height;
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

		vkCmdDraw(cmdBuf->commandBuffer, positiveTest ? 30 : 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdBuf->commandBuffer);

		MPD_ASSERT_RESULT(vkEndCommandBuffer(cmdBuf->commandBuffer));

		VkSubmitInfo submit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &cmdBuf->commandBuffer;
		vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkDestroyRenderPass(device, renderPass, 0);
		vkDestroyFramebuffer(device, fbo, 0);

		uint32_t count = getCount(MESSAGE_CODE_NON_INDEXED_DRAW_CALL);
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
		if (!testNonIndexed(false))
			return false;

		if (!testNonIndexed(true))
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
	return new DrawCallTest;
}
