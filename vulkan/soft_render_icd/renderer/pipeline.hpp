#pragma once

#include "../../../common/print.hpp"
#include "core.hpp"
#include "../../../vulkan-headers/include/vulkan/vulkan.h"

#include "operations.hpp"

#include <glm/gtx/vec_swizzle.hpp>

struct Pipeline;


class Blender {
    public:
    Blender() = default;
    Blender(const VkPipelineColorBlendStateCreateInfo& state): _state{state} { 
        assert(state.sType == VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
        assert(state.logicOpEnable == false); // TODO: Support Logical ops
        assert(state.flags == 0); // Spec hasn't defined any yet

        _attachments.resize(state.attachmentCount);
        for(size_t i = 0; i < state.attachmentCount; i++)
            _attachments[i] = state.pAttachments[i];

        _state.pAttachments = _attachments.data();

        // TODO: Multiple attachments
        assert(_attachments[0].colorWriteMask == VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT); // TODO: Support Write Masks
    }
    
    glm::vec4 operator()(glm::vec4 c_new, glm::vec4 c_old){
        if(!_attachments[0].blendEnable)
            return c_new;
        
        auto factor = [this, &c_old, &c_new](VkBlendFactor factor) -> glm::vec4 {
            switch (factor)
            {
            case VK_BLEND_FACTOR_ZERO: return glm::vec4{0, 0, 0, 0};
            case VK_BLEND_FACTOR_ONE: return glm::vec4{1, 1, 1, 1};
            case VK_BLEND_FACTOR_SRC_COLOR: return c_new;
            case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return glm::vec4{1 - c_new.r, 1 - c_new.g, 1 - c_new.b, 1 - c_new.a};
            
            case VK_BLEND_FACTOR_SRC_ALPHA: return glm::vec4{c_new.a};

            default:
                print("Granite/Blender: Unknown RGB VkBlendFactor: {:#x}\n", factor);
                assert(!"Granite/Blender: Error");
            }
        };

        auto src_colour_factor = glm::xyz(factor(_attachments[0].srcColorBlendFactor));
        auto dst_colour_factor = glm::xyz(factor(_attachments[0].dstColorBlendFactor));

        auto src_alpha_factor = factor(_attachments[0].srcAlphaBlendFactor).a;
        auto dst_alpha_factor = factor(_attachments[0].dstAlphaBlendFactor).a;

        glm::vec3 out_rgb = apply_blend_op(_attachments[0].colorBlendOp, src_colour_factor * glm::xyz(c_new), dst_colour_factor * glm::xyz(c_old));
        float out_a = apply_blend_op(_attachments[0].alphaBlendOp, src_alpha_factor * c_new.w, dst_alpha_factor * c_old.w);

        return glm::vec4{out_rgb, out_a};
    }

    private:
    VkPipelineColorBlendStateCreateInfo _state;
    std::vector<VkPipelineColorBlendAttachmentState> _attachments;
};

struct Rasterizer {
    Rasterizer() = default;
    Rasterizer(const VkPipelineRasterizationStateCreateInfo& info): _info{info} {
        assert(_info.sType == VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
        assert(_info.flags == 0);

        assert(_info.rasterizerDiscardEnable == false); // TODO: Support Rasterizer Discard
        assert(_info.depthClampEnable == false); // TODO: Support Depth Clamp
        assert(_info.depthBiasEnable == false); // TODO: Depth Bias support

        assert(_info.polygonMode == VK_POLYGON_MODE_FILL); // TODO: Support non-fill polygon modes
        assert(_info.cullMode == VK_CULL_MODE_NONE); // TODO: Implement Back Face Culling
        assert(_info.frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE); // TODO: Implement Clockwise Front Face
    }

    template<typename P>
    void operator()(const VkViewport& view, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, P pixel){
        // Calculate bouding box
        auto min_y = std::min(v0.y, std::min(v1.y, v2.y));
        auto max_y = std::max(v0.y, std::max(v1.y, v2.y));

        auto min_x = std::min(v0.x, std::min(v1.x, v2.x));
        auto max_x = std::max(v0.x, std::max(v1.x, v2.x));

        // Calculate screen space region
        size_t x0 = std::max(size_t{0}, (size_t)std::floor(min_x));
        size_t x1 = std::min(view.w - 1, (size_t)std::floor(max_x));
        size_t y0 = std::max(size_t{0}, (size_t)std::floor(min_y));
        size_t y1 = std::min(view.h - 1, (size_t)std::floor(max_y));

	    auto edge = [](glm::vec2 a, glm::vec2 b, glm::vec2 c) -> float {
    		return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    	};

	    auto area = edge(v0, v1, v2);

	    for(size_t y = y0; y <= y1; y++){
		    for(size_t x = x0; x <= x1; x++){
    			auto pixel_coord = glm::vec2{x + 0.5, y + 0.5};

	    		auto w0 = edge(v1, v2, pixel_coord);
		    	auto w1 = edge(v2, v0, pixel_coord);
    			auto w2 = edge(v0, v1, pixel_coord);

	    		if(w0 >= 0 && w1 >= 0 && w2 >= 0){
		    		w0 /= area;
			    	w1 /= area;
				    w2 /= area;
				
				    auto z = 1 / ((v0.z * w0) + (v1.z * w1) + (v2.z * w2));
				    pixel(x, y, z);
			    }
		    }
	    }
    }

    private:
    VkPipelineRasterizationStateCreateInfo _info;
};

struct Pipeline {
    public:
    Pipeline(const VkGraphicsPipelineCreateInfo& info) {
        assert(info.sType == VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);

        assert(info.pColorBlendState);
        assert(info.pRasterizationState);
        assert(info.pMultisampleState);
        assert(info.pDepthStencilState);
        assert(info.pViewportState);
        assert(info.pDynamicState);

        blender = Blender{*info.pColorBlendState};
        rasterizer = Rasterizer{*info.pRasterizationState};

        const auto& dynamic = *info.pDynamicState;
        assert(dynamic.sType == VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
        assert(dynamic.flags == 0);

        assert(dynamic.dynamicStateCount == 0); // TODO: Implement Dynamic States

        const auto& viewport = *info.pViewportState;
        assert(viewport.sType == VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
        assert(viewport.flags == 0);

        // TODO: Ignore these if dynamic state is enabled
        for(size_t i = 0; i < viewport.viewportCount; i++)
            viewports.push_back(viewport.pViewports[i]);

        for(size_t i = 0; i < viewport.scissorCount; i++)
            scissors.push_back(viewport.pScissors[i]);

        const auto& multisample = *info.pMultisampleState;
        assert(multisample.sType == VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
        assert(multisample.flags == 0);

        assert(multisample.alphaToOneEnable == false);
        assert(multisample.alphaToCoverageEnable == false);
        assert(multisample.sampleShadingEnable == false);
        assert(multisample.rasterizationSamples == VK_SAMPLE_COUNT_1_BIT);

        const auto& depth_stencil = *info.pDepthStencilState;
        assert(depth_stencil.sType == VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
        assert(depth_stencil.flags == 0);

        assert(depth_stencil.depthTestEnable == true); // TODO: Support disabling depth test
        depth_test_op = depth_stencil.depthCompareOp;
        assert(depth_stencil.depthWriteEnable == true); // TODO: Support disabling depth write
        assert(depth_stencil.depthBoundsTestEnable == false); // TODO: Support Depth Bound test
        assert(depth_stencil.stencilTestEnable == false); // TODO: Support Stencil test
    }

    void draw(/* TODO */){
        auto viewport_transform = [](glm::vec3 ndc) -> glm::vec3 { return glm::vec3{0}; /* TODO */ };

        auto v0_clip = glm::vec4{0}; // TODO: Invoke Vertex Shader
        auto v0_ndc = glm::xyz(v0_clip) / v0_clip.w; // Clip Space -> NDC space by perspective division
        auto v0_screen = viewport_transform(v0_ndc); // NDC -> Screen space by Viewport Transform

        // Same as above
        auto v1_clip = glm::vec4{0};
        auto v1_ndc = glm::xyz(v1_clip) / v1_clip.w;
        auto v1_screen = viewport_transform(v1_ndc);

        auto v2_clip = glm::vec4{0};
        auto v2_ndc = glm::xyz(v2_clip) / v2_clip.w;
        auto v2_screen = viewport_transform(v2_ndc);

        rasterizer(viewports[0], v0_screen, v1_screen, v2_screen, [this](size_t screen_x, size_t screen_y, float z){
            if(apply_compare_op(depth_test_op, z, 0.0f /* Read from FB */)){
                /* Write Z to FB */

                auto fragment = glm::vec4{}; // Invoke Fragment Shader
                auto blended = blender(fragment, {} /* Read from FB */);
                // Write `blended` to FB
            }
	    });

    }

    
    private:
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;

    VkCompareOp depth_test_op;

    Rasterizer rasterizer;
    Blender blender;
};
