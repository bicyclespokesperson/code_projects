#ifndef VERTEX_H
#define VERTEX_H

namespace gar {

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 tex_coord;

  static VkVertexInputBindingDescription get_binding_description();
  static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions();

  bool operator==(Vertex const& other) const;
};

}  // namespace gar

namespace std {
template <>
struct hash<gar::Vertex> {
  size_t operator()(gar::Vertex const& vertex) const {
    return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
           (hash<glm::vec2>()(vertex.tex_coord) << 1);
  }
};
}  // namespace std

#endif  // VERTEX_H
