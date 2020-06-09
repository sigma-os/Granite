#pragma once

#include "../../../vulkan-headers/include/vulkan/vulkan.h"

template<typename T>
bool apply_compare_op(const VkCompareOp op, const T& lhs, const T& rhs){
	switch(op) {
		case VK_COMPARE_OP_NEVER: return false;
		case VK_COMPARE_OP_ALWAYS: return true;
		case VK_COMPARE_OP_LESS: return lhs < rhs;
		case VK_COMPARE_OP_LESS_OR_EQUAL: return lhs <= rhs;
		case VK_COMPARE_OP_GREATER: return lhs > rhs;
		case VK_COMPARE_OP_GREATER_OR_EQUAL: return lhs >= rhs;
		case VK_COMPARE_OP_EQUAL: return lhs == rhs;
		case VK_COMPARE_OP_NOT_EQUAL: return lhs != rhs;
		default: assert(!"Illegal VkCompareOp");
	}
}

template<typename T>
T apply_blend_op(const VkBlendOp op, const T& lhs, const T& rhs){
	switch(op) {
		case VK_BLEND_OP_ADD: return lhs + rhs;
		case VK_BLEND_OP_SUBTRACT: return lhs - rhs;
		case VK_BLEND_OP_REVERSE_SUBTRACT: return rhs - lhs;
		case VK_BLEND_OP_MIN: return std::min(lhs, rhs);
		case VK_BLEND_OP_MAX: return std::max(lhs, rhs);
		default: assert(!"Illegal VkBlendOp");
	}
}

