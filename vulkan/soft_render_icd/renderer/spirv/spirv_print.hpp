#pragma once

#include "spirv.hpp"
#include "../../../../common/print.hpp"

template<>
struct format::formatter<spv::ExecutionModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::ExecutionModel item){
        switch (item) {
            case spv::ExecutionModel::Vertex: it.write("Vertex"); break;
            case spv::ExecutionModel::Fragment: it.write("Fragment"); break;
            case spv::ExecutionModel::TessellationControl: it.write("Tesselation Control"); break;
            case spv::ExecutionModel::TessellationEvaluation: it.write("Tesselation Eval"); break;
            case spv::ExecutionModel::Geometry: it.write("Geometry"); break;
            case spv::ExecutionModel::GLCompute: it.write("Compute"); break;

            case spv::ExecutionModel::Kernel: it.write("OpenCL Kernel"); break;

            case spv::ExecutionModel::MeshNV: it.write("NV Mesh"); break;
            case spv::ExecutionModel::TaskNV: it.write("NV Task"); break;

            case spv::ExecutionModel::RayGenerationKHR: it.write("KHR Ray Generation"); break;
            case spv::ExecutionModel::MissKHR: it.write("KHR Miss"); break;
            case spv::ExecutionModel::IntersectionKHR: it.write("KHR Intersection"); break;
            case spv::ExecutionModel::ClosestHitKHR: it.write("KHR Closest Hit"); break;
            case spv::ExecutionModel::CallableKHR: it.write("KHR Callable"); break;
            case spv::ExecutionModel::AnyHitKHR: it.write("KHR Any Hit"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::AddressingModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::AddressingModel item){
        switch (item) {
            case spv::AddressingModel::Logical: it.write("Logical"); break;
            case spv::AddressingModel::Physical32: it.write("32 Bit Physical"); break;
            case spv::AddressingModel::Physical64: it.write("64 Bit Physical"); break;
            case spv::AddressingModel::PhysicalStorageBuffer64: it.write("Physical Storage Buffer 64 Bit"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::MemoryModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::MemoryModel item){
        switch (item) {
            case spv::MemoryModel::Simple: it.write("Simple"); break;
            case spv::MemoryModel::GLSL450: it.write("GLSL 4.50"); break;
            case spv::MemoryModel::OpenCL: it.write("OpenCL"); break;
            case spv::MemoryModel::Vulkan: it.write("Vulkan"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::SourceLanguage> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::SourceLanguage item){
        switch (item) {
            case spv::SourceLanguage::ESSL: it.write("ESSL (OpenGL ES Shading Language)"); break;
            case spv::SourceLanguage::GLSL: it.write("GLSL (OpenGL Shading Language)"); break;
            case spv::SourceLanguage::HLSL: it.write("HLSL (High Level Shader Language)"); break;
            case spv::SourceLanguage::OpenCL_C: it.write("C (OpenCL Kernel)"); break;
            case spv::SourceLanguage::OpenCL_CPP: it.write("C++ (OpenCL Kernel)"); break;
            default: break;
        }
    }
};