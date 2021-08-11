#include <pch.hpp>

#include <image_writer.h>
#include <model.h>
#include <pga3d.h>
#include <ppm_writer.h>
#include <vulkan_toolbox.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

int main() {
  gar::draw_triangle();

  Model m{"models/chalet.obj"};

  return 0;
}
