project(
  'Velox Engine',
  'cpp',
  meson_version: '>= 1.3.0',
  version: '0.1',
  default_options: ['warning_level=0', 'cpp_std=c++20'],
)

sdl_dep = dependency('sdl3')
image_dep = dependency('sdl3-image')
ttf_dep = dependency('sdl3-ttf')

velox_inc = include_directories('include')

dependencies = [sdl_dep, image_dep, ttf_dep]

src = [
  'src/renderWindow.cpp',
  'src/assetManager.cpp',
  'src/renderSystem.cpp',
  'src/engine.cpp',
  'src/collisionSystem.cpp',
  'src/animation.cpp',
  'src/input.cpp'
]

velox = library(
  'velox',
  src,
  include_directories: velox_inc,
  dependencies: [sdl_dep, image_dep, ttf_dep],
  install: true,
)

velox_dep = declare_dependency(include_directories: velox_inc, link_with: velox)
