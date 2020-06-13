#spirv_vm_files = files(
#    'libs/spirv_vm/src/ext/GLSL450.c',
#    'libs/spirv_vm/src/context.c',
#    'libs/spirv_vm/src/image.c',
#    'libs/spirv_vm/src/opcode_execute.c',
#    'libs/spirv_vm/src/opcode_setup.c',
#    'libs/spirv_vm/src/program.c',
#    'libs/spirv_vm/src/result.c',
#    'libs/spirv_vm/src/state.c',
#    'libs/spirv_vm/src/types.c',
#    'libs/spirv_vm/src/value.c')

#spirv_vm_includes = include_directories('libs/spirv_vm/inc/')
#spirv_vm = static_library('spriv-vm', spirv_vm_files, include_directories: include_directories)

shared_library('vulkan_granite', 'instance.cpp')
subdir('renderer/rasterizer-test')
subdir('renderer/spirv')