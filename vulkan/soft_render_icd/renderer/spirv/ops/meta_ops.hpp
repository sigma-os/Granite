#pragma once

#include "jit.hpp"

void execute_OpSource(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpSourceExtension(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpName(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpMemberName(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpExtension(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpExtInstImport(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpMemoryModel(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpEntryPoint(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpCapability(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);

void execute_OpDecorate(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);
void execute_OpMemberDecorate(SpirvJit& code, uint32_t instruction_len, const uint32_t* data);