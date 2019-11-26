
//Note vulkan test needs to come before perfdoc...
#include "vulkan_test.hpp"
#include "perfdoc.hpp"
#include "util.hpp"
#include <functional>
#include <memory>
using namespace MPD;
using namespace std;

class QueryTest : public VulkanTestHelper
{
public:
	bool testQueryPoolReset(bool positiveTest)
	{
		auto cmdbuf = make_shared<CommandBuffer>(device);
		cmdbuf->initPrimary();

		VkQueryPoolCreateInfo qpci = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
		qpci.queryCount = positiveTest ? 1 : 100;
		qpci.queryType = VK_QUERY_TYPE_OCCLUSION;
		qpci.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

		VkQueryPool pool;
		vkCreateQueryPool(device, &qpci, 0, &pool);

		VkCommandBufferBeginInfo cbbi = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		vkBeginCommandBuffer(cmdbuf->commandBuffer, &cbbi);

		vkCmdResetQueryPool(cmdbuf->commandBuffer, pool, 0, positiveTest ? 1 : 100);

		vkEndCommandBuffer(cmdbuf->commandBuffer);

		vkDestroyQueryPool(device, pool, 0);

		uint32_t count = getCount(MESSAGE_CODE_QUERY_BUNDLE_TOO_SMALL);
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
		if (!testQueryPoolReset(false))
			return false;

		if (!testQueryPoolReset(true))
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
	return new QueryTest;
}
