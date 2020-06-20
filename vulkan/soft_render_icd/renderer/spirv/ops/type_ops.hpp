#pragma once

#include "jit.hpp"

void execute_OpTypeVoid(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeBool(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeInt(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeFloat(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeVector(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeArray(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpTypeFunction(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpConstant(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);