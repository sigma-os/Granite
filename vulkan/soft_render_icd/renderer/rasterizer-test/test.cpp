#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

#include "rasterizer.hpp"

#include <glm/gtc/matrix_transform.hpp>

template<typename F, typename... Args>
std::chrono::duration<float, std::milli> timed(F f, Args&&... args){
	auto start = std::chrono::high_resolution_clock::now();

	f(std::forward<Args>(args)...);

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::milli>(end - start);
}


int main(){
    Viewport viewport;

	viewport.x = 0;
	viewport.y = 0;
	viewport.w = 640;
	viewport.h = 480;
	viewport.n = 0.0f;
	viewport.f = 1.0f;

    Framebuffer fb{viewport};

	std::vector<glm::vec3> triangles = {
		{-0.5f, -0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{0.0f, 0.5f, 0.5f}
	};

	std::vector<uint8_t> indices {
		0, 1, 2
	};

	auto time = timed([&](){
		auto projection = glm::perspective(glm::radians(45.0f), (float)viewport.w / viewport.h, 0.01f, 10000.0f);
		auto view = glm::lookAt(glm::vec3{0, 1, 2}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

		draw(fb, [&](size_t i) -> glm::vec4 {
				return projection * view * glm::vec4{triangles[i], 1.0};
		  	},
			[]() -> glm::vec4 {
				return {1, 0, 0, 1};
		  	},
		  	indices);

		auto model = glm::translate(glm::mat4{1.0f}, glm::vec3{0, 0, -0.5});
		//model = glm::scale(model, glm::vec3{1.4, 1.4, 1.4});

        draw(fb, [&](size_t i) -> glm::vec4 {
				return projection * view * model * glm::vec4{triangles[i], 1.0};
		  	},
		  	[]() -> glm::vec4 {
				return {0, 1, 0, 1};
			},
		  	indices);
	}).count();

	std::cout << "Rasterization took " << time << "ms (" << (1000 / time) << " FPS)" << std::endl;

	auto write_ppm = [](std::vector<uint8_t>& fb, const std::string& filename, size_t width, size_t height, size_t bpp){
		std::ofstream stream{};
		stream.open(filename);

		stream << "P6\n"
		       << width << " " << height << "\n"
		       << bpp << "\n";
		stream.write((const char*)fb.data(), fb.size());
		stream.close();
	};
	write_ppm(fb.colour, "colour.ppm", viewport.w, viewport.h, 255);

	std::vector<uint8_t> depth_colour_buffer{};
	for(size_t i = 0; i < fb.depth.size(); i++){
		depth_colour_buffer.push_back(fb.depth[i] * 255);
		depth_colour_buffer.push_back(fb.depth[i] * 255);
		depth_colour_buffer.push_back(fb.depth[i] * 255);
	}

	write_ppm(depth_colour_buffer, "depth.ppm", viewport.w, viewport.h, 255);
}