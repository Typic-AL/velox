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
mixer_dep = dependency('sdl3-mixer')

velox_deps = [sdl_dep, image_dep, ttf_dep, mixer_dep]

velox_inc = include_directories('include')

src = [
  'src/renderWindow.cpp',
  'src/assetManager.cpp',
  'src/renderSystem.cpp',
  'src/engine.cpp',
  'src/collisionSystem.cpp',
  'src/animation.cpp',
  'src/input.cpp',
  'src/ui.cpp',
  'src/space.cpp',
  'src/tilemap.cpp',
  'src/audioMixer.cpp',
]

velox = static_library(
  'velox',
  src,
  include_directories: velox_inc,
  dependencies: velox_deps,
)

velox_dep = declare_dependency(
  include_directories: velox_inc,
  link_with: velox,
  dependencies: velox_deps,
)
