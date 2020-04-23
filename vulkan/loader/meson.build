cc = meson.get_compiler('cpp')
dl_dep = cc.find_library('dl', required: false)

loader = shared_library('vulkan_loader', 'loader.cpp', dependencies: dl_dep)
loader_dep = declare_dependency(link_with: loader)
