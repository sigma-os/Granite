#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "core.hpp"

template<typename P>
void rasterize(const Viewport& view, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, P pixel){
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

// V vertex_program should be a lambda, capturing arbitrarily, taking 1 size_t in as vertex_index and returning glm::vec4 as clip_space coord
// F fragment_program should be a lambda, capturing arbitrarily, and returning a glm::vec4 vertex colour
template<typename V, typename F, typename I>
void draw(Framebuffer& fb, V vertex_program, F fragment_program, std::vector<I> indices){
	auto viewport_transform = [&fb](glm::vec3 p) -> glm::vec3 {
		//auto x = (viewport.w / 2.0f) * p.x + (viewport.x + (viewport.w / 2.0f));
		//auto y = (viewport.h / 2.0f) * p.y + (viewport.y + (viewport.h / 2.0f));
		//auto z = ((viewport.f - viewport.n) / 2.0f) * p.z + ((viewport.f + viewport.n) / 2.0f);

		auto x = (p.x + 1) / 2 * fb.viewport.w;
		auto y = (1 - p.y) / 2 * fb.viewport.h;
		auto z = -p.z;

		return glm::vec3{x, y, z};
	};

	assert((indices.size() % 3) == 0);
	for(size_t i = 0; i < indices.size(); i += 3){
		auto v0_clip = vertex_program(indices[i]); // Let user compute Clip-Space coords
		auto v0 = glm::vec3{v0_clip} / v0_clip.w; // Clip-Space -> NDC by Perspective Division
		v0 = viewport_transform(v0); // NDC -> Pixel-Space by Viewport Transform
		v0.z = 1 / v0.z; // Compute reciprocal of Depth coord

        // Repeat for the other vertices in the triangle
		auto v1_clip = vertex_program(indices[i + 1]);
		auto v1 = glm::vec3{v1_clip} / v1_clip.w;
		v1 = viewport_transform(v1);
		v1.z = 1 / v1.z;

		auto v2_clip = vertex_program(indices[i + 2]);
		auto v2 = glm::vec3{v2_clip} / v2_clip.w;
		v2 = viewport_transform(v2);
		v2.z = 1 / v2.z;

		rasterize(fb.viewport, v0, v1, v2, [&](size_t screen_x, size_t screen_y, float z){
			if(z < fb.depth[screen_y * (size_t)fb.viewport.w + screen_x]){ // Depth test
				fb.depth[screen_y * (size_t)fb.viewport.w + screen_x] = z;
					
				fb.write_pixel(screen_x, screen_y, fragment_program());
			}
		});
	}
}