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

namespace MPD
{
enum MessageCodes
{
	MESSAGE_CODE_COMMAND_BUFFER_RESET = 1,
	MESSAGE_CODE_COMMAND_BUFFER_SIMULTANEOUS_USE = 2,
	MESSAGE_CODE_SMALL_ALLOCATION = 3,
	MESSAGE_CODE_SMALL_DEDICATED_ALLOCATION = 4,
	MESSAGE_CODE_TOO_LARGE_SAMPLE_COUNT = 5,
	MESSAGE_CODE_NON_LAZY_MULTISAMPLED_IMAGE = 6,
	MESSAGE_CODE_NON_LAZY_TRANSIENT_IMAGE = 7,
	MESSAGE_CODE_MULTISAMPLED_IMAGE_REQUIRES_MEMORY = 8,
	MESSAGE_CODE_RESOLVE_IMAGE = 9,
	MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_BE_TRANSIENT = 10,
	MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_NOT_BE_TRANSIENT = 11,
	MESSAGE_CODE_INDEX_BUFFER_SPARSE = 12,
	MESSAGE_CODE_INDEX_BUFFER_CACHE_THRASHING = 13,
	MESSAGE_CODE_TOO_MANY_INSTANCED_VERTEX_BUFFERS = 14,
	MESSAGE_CODE_DISSIMILAR_WRAPPING = 15,
	MESSAGE_CODE_NO_PIPELINE_CACHE = 16,
	MESSAGE_CODE_DESCRIPTOR_SET_ALLOCATION_CHECKS = 17,
	MESSAGE_CODE_COMPUTE_NO_THREAD_GROUP_ALIGNMENT = 18,
	MESSAGE_CODE_COMPUTE_LARGE_WORK_GROUP = 19,
	MESSAGE_CODE_COMPUTE_POOR_SPATIAL_LOCALITY = 20,
	MESSAGE_CODE_POTENTIAL_PUSH_CONSTANT = 21,
	MESSAGE_CODE_MANY_SMALL_INDEXED_DRAWCALLS = 22,
	MESSAGE_CODE_DEPTH_PRE_PASS = 23,
	MESSAGE_CODE_PIPELINE_BUBBLE = 24,
	MESSAGE_CODE_NOT_FULL_THROUGHPUT_BLENDING = 25,
	MESSAGE_CODE_SAMPLER_LOD_CLAMPING = 26,
	MESSAGE_CODE_SAMPLER_LOD_BIAS = 27,
	MESSAGE_CODE_SAMPLER_BORDER_CLAMP_COLOR = 28,
	MESSAGE_CODE_SAMPLER_UNNORMALIZED_COORDS = 29,
	MESSAGE_CODE_SAMPLER_ANISOTROPY = 30,
	MESSAGE_CODE_TILE_READBACK = 31,
	MESSAGE_CODE_CLEAR_ATTACHMENTS_AFTER_LOAD = 32,
	MESSAGE_CODE_CLEAR_ATTACHMENTS_NO_DRAW_CALL = 33,
	MESSAGE_CODE_REDUNDANT_RENDERPASS_STORE = 34,
	MESSAGE_CODE_REDUNDANT_IMAGE_CLEAR = 35,
	MESSAGE_CODE_INEFFICIENT_CLEAR = 36,
	MESSAGE_CODE_LAZY_TRANSIENT_IMAGE_NOT_SUPPORTED = 37,
	
	///////////////////////////////////////////////
	//new messages from here
	///////////////////////////////////////////////
	
	MESSAGE_CODE_UNCOMPRESSED_TEXTURE_USED = 38,
	MESSAGE_CODE_NON_MIPMAPPED_TEXTURE_USED = 39,
	MESSAGE_CODE_NON_INDEXED_DRAW_CALL = 40,
	MESSAGE_CODE_SUBOPTIMAL_TEXTURE_FORMAT = 41,
	MESSAGE_CODE_TEXTURE_LINEAR_TILING = 42,
	MESSAGE_CODE_NO_FBCDC = 43, 
	MESSAGE_CODE_ROBUST_BUFFER_ACCESS_ENABLED = 44,
	MESSAGE_CODE_SUBOPTIMAL_SUBPASS_DEPENDENCY_FLAG = 45,
	MESSAGE_CODE_PARTIAL_CLEAR = 46,
	MESSAGE_CODE_PIPELINE_OPTIMISATION_DISABLED = 47,
	MESSAGE_CODE_WORKGROUP_SIZE_DIVISOR = 48,
	MESSAGE_CODE_POTENTIAL_SUBPASS = 49,
	MESSAGE_CODE_SUBPASS_STENCIL_SELF_DEPENDENCY = 50,
	MESSAGE_CODE_INEFFICIENT_DEPTH_STENCIL_OPS = 51,
	MESSAGE_CODE_QUERY_BUNDLE_TOO_SMALL = 52,

	MESSAGE_CODE_COUNT
};
}
