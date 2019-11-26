/* Copyright (c) 2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "perfdoc.hpp"
#include <cstring>
#include <string>
#include <unordered_map>

namespace MPD
{

struct ConfigOptionMapHasher
{
	size_t operator()(const char *name) const
	{
		return std::hash<std::string>()(name);
	}
};

struct ConfigOptionMapCompare
{
	bool operator()(const char *a, const char *b) const
	{
		return strcmp(a, b) == 0;
	}
};

template <typename T>
struct ConfigOption
{
	T *ptrToValue;
	const char *name;
	const char *description;
};

template <typename T>
using ConfigOptionMap =
    std::unordered_map<const char *, ConfigOption<T>, ConfigOptionMapHasher, ConfigOptionMapCompare>;

/// Dummy class that initializes the map with something.
template <typename T>
struct DummyInitializer
{
	DummyInitializer(const char *name, const char *descr, T *ptrToVal, ConfigOptionMap<T> &map)
	{
		map[name] = ConfigOption<T>{ ptrToVal, name, descr };
	}
};

#define MPD_DEFINE_CFG_OPTIONB(name, defaultVal, descr) \
	bool name = defaultVal;                             \
	DummyInitializer<bool> name##d = { #name, descr, &name, bools }

#define MPD_DEFINE_CFG_OPTIONI(name, defaultVal, descr) \
	int64_t name = defaultVal;                          \
	DummyInitializer<int64_t> name##d = { #name, descr, &name, ints }

#define MPD_DEFINE_CFG_OPTIONU(name, defaultVal, descr) \
	uint64_t name = defaultVal;                         \
	DummyInitializer<uint64_t> name##d = { #name, descr, &name, uints }

#define MPD_DEFINE_CFG_OPTIONF(name, defaultVal, descr) \
	double name = defaultVal;                           \
	DummyInitializer<double> name##d = { #name, descr, &name, floats }

#define MPD_DEFINE_CFG_OPTION_STRING(name, defaultVal, descr) \
	std::string name = defaultVal;                            \
	DummyInitializer<std::string> name##d = { #name, descr, &name, strings }

/// A collection of configuration variables.
///
/// The config file has the following format (example):
/// @code
/// # This is a comment
/// maxSmallIndexedDrawcalls 666
///
/// # This is another comment
/// smallIndexedDrawcallIndices 667
/// #endcode
class Config
{
private:
	ConfigOptionMap<bool> bools;
	ConfigOptionMap<int64_t> ints;
	ConfigOptionMap<uint64_t> uints;
	ConfigOptionMap<double> floats;
	ConfigOptionMap<std::string> strings;

public:
	MPD_DEFINE_CFG_OPTIONU(maxSmallIndexedDrawcalls, 10,
	                       "How many small indexed drawcalls in a command buffer before a warning is thrown");

	MPD_DEFINE_CFG_OPTIONU(smallIndexedDrawcallIndices, 10, "How many indices make a small indexed drawcall");

	MPD_DEFINE_CFG_OPTIONU(depthPrePassMinVertices, 1,
	                       "Minimum number of vertices to take into account when doing depth pre-pass checks");
	MPD_DEFINE_CFG_OPTIONU(depthPrePassMinIndices, 1,
	                       "Minimum number of indices to take into account when doing depth pre-pass checks");
	MPD_DEFINE_CFG_OPTIONU(depthPrePassNumDrawCalls, 1,
	                       "Minimum number of drawcalls in order to trigger depth pre-pass");

	MPD_DEFINE_CFG_OPTIONU(minDeviceAllocationSize, 256 * 1024, "Recomended allocation size for vkAllocateMemory");

	MPD_DEFINE_CFG_OPTIONU(
	    minDedicatedAllocationSize, 2 * 1024 * 1024,
	    "If a buffer or image is allocated and it consumes an entire VkDeviceMemory, it should at least be this large. "
	    "This is slightly different from minDeviceAllocationSize since the 256K buffer can still be sensibly "
	    "suballocated from. If we consume an entire allocation with one image or buffer, it should at least be for a "
	    "very large allocation");

	MPD_DEFINE_CFG_OPTIONU(maxEfficientSamples, 2,
	                       "Maximum sample count for full throughput");

	MPD_DEFINE_CFG_OPTIONF(unclampedMaxLod, 32.0f, "The minimum LOD level which is equivalent to unclamped maxLod");

	MPD_DEFINE_CFG_OPTIONU(indexBufferScanMinIndexCount, 128,
	                       "Skip index buffer scanning of drawcalls with less than this limit");

	MPD_DEFINE_CFG_OPTIONF(indexBufferUtilizationThreshold, 0.5,
	                       "Only report indexbuffer fragmentation warning if utilization is below this threshold");

	MPD_DEFINE_CFG_OPTIONF(indexBufferCacheHitThreshold, 0.5,
	                       "Only report cache hit performance warnings if cache hit is below this threshold");

	MPD_DEFINE_CFG_OPTIONU(indexBufferVertexPostTransformCache, 32,
	                       "Size of post-transform cache used for estimating index buffer cache hit-rate");

	MPD_DEFINE_CFG_OPTIONU(maxInstancedVertexBuffers, 1,
	                       "Maximum number of instanced vertex buffers which should be used");

	MPD_DEFINE_CFG_OPTIONU(
	    threadGroupSize, 4,
	    "On Midgard, compute threads are dispatched in groups. On Bifrost, threads run in lock-step.");

	MPD_DEFINE_CFG_OPTIONU(maxEfficientWorkGroupThreads, 64, "Maximum number of threads which can efficiently be part "
	                                                         "of a compute workgroup when using thread group barriers");

	MPD_DEFINE_CFG_OPTIONU(minQueryCount, 10, "Minimum number of queries that should be operated on at once.");


	MPD_DEFINE_CFG_OPTIONB(
	    indexBufferScanningEnable, true,
	    "If enabled, scans the index buffer for every draw call in an attempt to find inefficiencies. "
	    "This is fairly expensive, so it should be disabled once index buffers have been validated.");

	MPD_DEFINE_CFG_OPTIONB(
	    indexBufferScanningInPlace, false,
	    "If enabled, scans the index buffer in place on vkCmdDrawIndexed. "
	    "This is useful to narrow down exactly which draw call is causing the issue as you can backtrace the debug "
	    "callback, "
	    "but scanning indices here will only work if the index buffer is actually valid when calling this function. "
	    "If not enabled, indices will be scanned on vkQueueSubmit.");

	MPD_DEFINE_CFG_OPTION_STRING(loggingFilename, "",
	                             "This setting specifies where to log output from the layer.\n"
	                             "# The setting does not impact VK_EXT_debug_report which will always be supported.\n"
	                             "# This filename represents a path on the file system, but special values include:\n"
	                             "#  stdout\n"
	                             "#  stderr\n"
	                             "#  logcat (Android only)\n"
	                             "#  debug_output (OutputDebugString, Windows only).");
								 
	MPD_DEFINE_CFG_OPTIONB(msgCommandBufferReset, true, "Toggle MESSAGE_CODE_COMMAND_BUFFER_RESET");
	MPD_DEFINE_CFG_OPTIONB(msgCommandBufferSimultaneousUse, true,
	                       "Toggle MESSAGE_CODE_COMMAND_BUFFER_SIMULTANEOUS_USE");
	MPD_DEFINE_CFG_OPTIONB(msgSmallAllocation, true,
	                                                   "Toggle MESSAGE_CODE_SMALL_ALLOCATION");
	MPD_DEFINE_CFG_OPTIONB(msgSmallDedicatedAllocation, true, "Toggle MESSAGE_CODE_SMALL_DEDICATED_ALLOCATION");
	MPD_DEFINE_CFG_OPTIONB(msgTooLargeSampleCount, true, "Toggle MESSAGE_CODE_TOO_LARGE_SAMPLE_COUNT");
	MPD_DEFINE_CFG_OPTIONB(msgNonLazyMultisampledImage, true, "Toggle MESSAGE_CODE_NON_LAZY_MULTISAMPLED_IMAGE");
	MPD_DEFINE_CFG_OPTIONB(msgNonLazyTransientImage, true, "Toggle MESSAGE_CODE_NON_LAZY_TRANSIENT_IMAGE");
	MPD_DEFINE_CFG_OPTIONB(msgMultisampledImageRequiresMemory, true,
	                       "Toggle MESSAGE_CODE_MULTISAMPLED_IMAGE_REQUIRES_MEMORY");
	MPD_DEFINE_CFG_OPTIONB(msgResolveImage, true, "Toggle MESSAGE_CODE_RESOLVE_IMAGE");
	MPD_DEFINE_CFG_OPTIONB(msgFramebufferAttachmentShouldBeTransient, true,
	                       "Toggle MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_BE_TRANSIENT");
	MPD_DEFINE_CFG_OPTIONB(msgFramebufferAttachmentShouldNotBeTransient, true,
	                       "Toggle MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_NOT_BE_TRANSIENT");
	MPD_DEFINE_CFG_OPTIONB(msgIndexBufferSparse, true, "Toggle MESSAGE_CODE_INDEX_BUFFER_SPARSE");
	MPD_DEFINE_CFG_OPTIONB(msgIndexBufferCacheThrashing, true, "Toggle MESSAGE_CODE_INDEX_BUFFER_CACHE_THRASHING");
	MPD_DEFINE_CFG_OPTIONB(msgTooManyInstancedVertexBuffers, true,
	                       "Toggle MESSAGE_CODE_TOO_MANY_INSTANCED_VERTEX_BUFFERS");
	MPD_DEFINE_CFG_OPTIONB(msgDissimilarWrapping, false, "Toggle MESSAGE_CODE_DISSIMILAR_WRAPPING");
	MPD_DEFINE_CFG_OPTIONB(msgNoPipelineCache, true, "Toggle MESSAGE_CODE_NO_PIPELINE_CACHE");
	MPD_DEFINE_CFG_OPTIONB(msgDescriptorSetAllocationChecks, true, "Toggle MESSAGE_CODE_DESCRIPTOR_SET_ALLOCATION_CHECKS");
	MPD_DEFINE_CFG_OPTIONB(msgComputeNoThreadGroupAlignment, false, "Toggle MESSAGE_CODE_COMPUTE_NO_THREAD_GROUP_ALIGNMENT");
	MPD_DEFINE_CFG_OPTIONB(msgComputeLargeWorkGroup, false, "Toggle MESSAGE_CODE_COMPUTE_LARGE_WORK_GROUP");
	MPD_DEFINE_CFG_OPTIONB(msgComputePoorSpatialLocality, true, "Toggle MESSAGE_CODE_COMPUTE_POOR_SPATIAL_LOCALITY");
	MPD_DEFINE_CFG_OPTIONB(msgPotentialPushConstant, true, "Toggle MESSAGE_CODE_POTENTIAL_PUSH_CONSTANT");
	MPD_DEFINE_CFG_OPTIONB(msgManySmallIndexedDrawcalls, true, "Toggle MESSAGE_CODE_MANY_SMALL_INDEXED_DRAWCALLS");
	MPD_DEFINE_CFG_OPTIONB(msgDepthPrePass, true, "Toggle MESSAGE_CODE_DEPTH_PRE_PASS");
	MPD_DEFINE_CFG_OPTIONB(msgPipelineBubble, true, "Toggle MESSAGE_CODE_PIPELINE_BUBBLE");
	MPD_DEFINE_CFG_OPTIONB(msgNotFullThroughputBlending, false, "Toggle MESSAGE_CODE_NOT_FULL_THROUGHPUT_BLENDING");
	MPD_DEFINE_CFG_OPTIONB(msgSamplerLodClamping, false, "Toggle MESSAGE_CODE_SAMPLER_LOD_CLAMPING");
	MPD_DEFINE_CFG_OPTIONB(msgSamplerLodBias, false, "Toggle MESSAGE_CODE_SAMPLER_LOD_BIAS");
	MPD_DEFINE_CFG_OPTIONB(msgSamplerBorderClampColor, false, "Toggle MESSAGE_CODE_SAMPLER_BORDER_CLAMP_COLOR"); //TODO do we care?
	MPD_DEFINE_CFG_OPTIONB(msgSamplerUnnormalizedCoords, false, "Toggle MESSAGE_CODE_SAMPLER_UNNORMALIZED_COORDS"); //TODO do we care?
	MPD_DEFINE_CFG_OPTIONB(msgSamplerAnisotropy, true, "Toggle MESSAGE_CODE_SAMPLER_ANISOTROPY");
	MPD_DEFINE_CFG_OPTIONB(msgTileReadback, true, "Toggle MESSAGE_CODE_TILE_READBACK");
	MPD_DEFINE_CFG_OPTIONB(msgClearAttachmentsAfterLoad, true, "Toggle MESSAGE_CODE_CLEAR_ATTACHMENTS_AFTER_LOAD");
	MPD_DEFINE_CFG_OPTIONB(msgClearAttachmentsNoDrawCall, true, "Toggle MESSAGE_CODE_CLEAR_ATTACHMENTS_NO_DRAW_CALL");
	MPD_DEFINE_CFG_OPTIONB(msgRedundantRenderpassStore, true, "Toggle MESSAGE_CODE_REDUNDANT_RENDERPASS_STORE");
	MPD_DEFINE_CFG_OPTIONB(msgRedundantImageClear, true, "Toggle MESSAGE_CODE_REDUNDANT_IMAGE_CLEAR");
	MPD_DEFINE_CFG_OPTIONB(msgInefficientClear, true, "Toggle MESSAGE_CODE_INEFFICIENT_CLEAR");
	MPD_DEFINE_CFG_OPTIONB(msgLazyTransientImageNotSupported, false, "Toggle MESSAGE_CODE_LAZY_TRANSIENT_IMAGE_NOT_SUPPORTED");
	
	///////////////////////////////////////////////
	//new messages from here
	///////////////////////////////////////////////
	
	///---uncompressed texture is used that is not a render target
	MPD_DEFINE_CFG_OPTIONB(msgUncompressedTextureUsed, true, "Toggle MESSAGE_CODE_UNCOMPRESSED_TEXTURE_USED");
	
	///---texture used that's not mipmapped and not a render target
	MPD_DEFINE_CFG_OPTIONB(msgNonMipmappedTextureUsed, true, "Toggle MESSAGE_CODE_NON_MIPMAPPED_TEXTURE_USED");
	
	///---non-indexed draw call
	MPD_DEFINE_CFG_OPTIONB(msgNonIndexedDrawCall, true, "Toggle MESSAGE_CODE_NON_INDEXED_DRAW_CALL");
	MPD_DEFINE_CFG_OPTIONU(maxSmallDrawcallVertices, 10,
	                       "How many vertices make a small drawcall");
	
	///---suboptimal texture format
	MPD_DEFINE_CFG_OPTIONB(msgSuboptimalTextureFormat, true, "Toggle MESSAGE_CODE_SUBOPTIMAL_TEXTURE_FORMAT");
	
	///---texture used with linear tiling layout
	MPD_DEFINE_CFG_OPTIONB(msgTextureLinearTiling, true, "Toggle MESSAGE_CODE_TEXTURE_LINEAR_TILING");
	
	///---render target won't have framebuffer compression
	MPD_DEFINE_CFG_OPTIONB(msgNoFBCDC, true, "Toggle MESSAGE_CODE_NO_FBCDC");
	
	///---robust buffer access might be expensive
	MPD_DEFINE_CFG_OPTIONB(msgRobustBufferAccessEnabled, true, "Toggle MESSAGE_CODE_ROBUST_BUFFER_ACCESS_ENABLED");
	
	///---subpass dependency flags? by region, device group, view local etc.
	MPD_DEFINE_CFG_OPTIONB(msgSuboptimalSubpassDependencyFlag, true, "Toggle MESSAGE_CODE_SUBOPTIMAL_SUBPASS_DEPENDENCY_FLAG");
	
	///---partial clears reduce performance
	MPD_DEFINE_CFG_OPTIONB(msgPartialClear, true, "Toggle MESSAGE_CODE_PARTIAL_CLEAR");
	
	///---disable pipeline optimisations?
	MPD_DEFINE_CFG_OPTIONB(msgPipelineOptimisationDisabled, true, "Toggle MESSAGE_CODE_PIPELINE_OPTIMISATION_DISABLED");
	
	///---we like workgroup sizes that are a multiple of 32
	MPD_DEFINE_CFG_OPTIONB(msgWorkgroupSizeDivisor, true, "Toggle MESSAGE_CODE_WORKGROUP_SIZE_DIVISOR");
	MPD_DEFINE_CFG_OPTIONU(workgroupSizeDivisor, 32, "On PowerVR workgroup sizes are optimal when they are a multiple of 32.");
	
	///---subsequent single subpass render passes using the same framebuffer should be turned into subpasses
	MPD_DEFINE_CFG_OPTIONB(msgPotentialSubpass, true, "Toggle MESSAGE_CODE_POTENTIAL_SUBPASS");
	
	///---stencil self dependencies
	///self dependency with stencil aspect mask
	///stencil buffer bound as an input attachment
	MPD_DEFINE_CFG_OPTIONB(msgSubpassStencilSelfDependency, true, "Toggle MESSAGE_CODE_SUBPASS_STENCIL_SELF_DEPENDENCY");
	
	MPD_DEFINE_CFG_OPTIONB(msgInefficientDepthStencilOps, true, "Toggle MESSAGE_CODE_INEFFICIENT_DEPTH_STENCIL_OPS");
	
	MPD_DEFINE_CFG_OPTIONB(msgQueryBundleTooSmall, true, "Toggle MESSAGE_CODE_QUERY_BUNDLE_TOO_SMALL");
	
	bool tryToLoadFromFile(const std::string &fname);

	void dumpToFile(const std::string &fname) const;
};
}
