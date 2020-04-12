project('Granite', 'cpp')

add_global_arguments('-O2', '-std=c++2a', '-Wall', '-Wextra', language: 'cpp')

# add_global_arguments('-fsanitize=undefined', '-fsanitize=address', language: 'cpp')
# add_global_link_arguments('-fsanitize=undefined', '-fsanitize=address', language: 'cpp')


subdir('soft_render_icd/')
subdir('loader/')

executable('granite-test', 'test.cpp', dependencies: loader_dep)
