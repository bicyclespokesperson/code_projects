// Based on the vulkan tutorial here: https://vulkan-tutorial.com/Introduction

#ifndef VULKAN_TOOLBOX_H
#define VULKAN_TOOLBOX_H

#include <vertex.h>

namespace gar {

bool draw_triangle();

struct Queue_family_indices;
struct Swap_chain_support_details;

class Vulkan_app {
 public:
  Vulkan_app();
  ~Vulkan_app();

  void main_loop();

 private:
  void init_vulkan();
  void init_window();
  void cleanup();
  void create_instance();
  bool check_validation_layer_support();
  void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
  void setup_debug_messenger();
  std::vector<char const*> get_required_extensions();
  void pick_physical_device();
  bool is_device_suitable(VkPhysicalDevice device);
  Queue_family_indices find_queue_families(VkPhysicalDevice device);
  void create_logical_device();
  void create_surface();
  bool check_device_extension_support(VkPhysicalDevice device);
  VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> const& availableFormats);
  VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> const& availablePresentModes);
  Swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device);
  VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR const& capabilities);
  void create_swap_chain();
  void create_image_views();
  void create_graphics_pipeline();
  void create_render_pass();
  VkShaderModule create_shader_module(std::vector<char> const& code);
  void create_frame_buffers();
  void create_command_pool();
  void create_command_buffers();
  void draw_frame();
  void create_sync_objects();
  void recreate_swap_chain();
  void cleanup_swap_chain();
  void create_vertex_buffer();
  void create_index_buffer();
  uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
  void create_buffer(VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer,
                     VkDeviceMemory& buffer_memory);
  void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
  void create_descriptor_set_layout();
  void create_uniform_buffers();
  void update_uniform_buffer(uint32_t current_image);
  void create_descriptor_pool();
  void create_descriptor_sets();
  void create_texture_image();
  void create_color_resources();

  void create_image(uint32_t width,
                    uint32_t height,
                    uint32_t mip_levels,
                    VkSampleCountFlagBits sample_count,
                    VkFormat format,
                    VkImageTiling tiling,
                    VkImageUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkImage& image,
                    VkDeviceMemory& image_memory);
  VkCommandBuffer begin_single_time_commands();
  void end_single_time_commands(VkCommandBuffer commandBuffer);
  void transition_image_layout(VkImage image,
                               VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout,
                               uint32_t mip_levels);
  void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
  void create_texture_image_view();
  VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels);
  void create_texture_sampler();
  void create_depth_resources();
  VkFormat find_supported_format(std::vector<VkFormat> const& candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);
  bool has_stencil_component(VkFormat format);
  VkFormat find_depth_format();
  void load_model();
  void generate_mipmaps(VkImage image,
                        VkFormat image_format,
                        int32_t tex_width,
                        int32_t tex_height,
                        uint32_t mip_levels);
  VkSampleCountFlagBits calc_max_usable_sample_count();

  static std::vector<char> read_file(std::string const& filename);
  static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
                                                       VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                                                       VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
                                                       void* /*pUserData*/);
  static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

  GLFWwindow* m_window{nullptr};
  VkInstance m_instance{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
  VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
  VkQueue m_graphics_queue{VK_NULL_HANDLE};
  VkDevice m_device{VK_NULL_HANDLE};
  VkSurfaceKHR m_surface{VK_NULL_HANDLE};
  VkQueue m_present_queue{VK_NULL_HANDLE};
  VkSwapchainKHR m_swap_chain;
  std::vector<VkImage> m_swap_chain_images;
  VkFormat m_swap_chain_image_format;
  VkExtent2D m_swap_chain_extent;
  std::vector<VkImageView> m_swap_chain_image_views;
  VkRenderPass m_render_pass;
  VkPipelineLayout m_pipeline_layout;
  VkDescriptorSetLayout m_descriptor_set_layout;
  std::vector<VkDescriptorSet> m_descriptor_sets;
  VkPipeline m_graphics_pipeline;
  std::vector<VkFramebuffer> m_swap_chain_framebuffers;
  VkCommandPool m_command_pool;
  VkImage m_depth_image;
  VkDeviceMemory m_depth_image_memory;
  VkImageView m_depth_image_view;
  std::vector<VkCommandBuffer> m_command_buffers;
  VkBuffer m_vertex_buffer;
  VkDeviceMemory m_vertex_buffer_memory;
  VkBuffer m_index_buffer;
  VkDeviceMemory m_index_buffer_memory;
  std::vector<VkBuffer> m_uniform_buffers;
  std::vector<VkDeviceMemory> m_uniform_buffers_memory;
  VkDescriptorPool m_descriptor_pool;
  VkImage m_texture_image;
  VkDeviceMemory m_texture_image_memory;
  VkImageView m_texture_image_view;
  VkSampler m_texture_sampler;
  uint32_t m_mip_levels;
  VkSampleCountFlagBits m_msaa_samples{VK_SAMPLE_COUNT_1_BIT};

  VkImage m_color_image;
  VkDeviceMemory m_color_image_memory;
  VkImageView m_color_image_view;

  std::vector<VkSemaphore> m_image_available_semaphores;
  std::vector<VkSemaphore> m_render_finished_semaphores;
  std::vector<VkFence> m_inflight_fences;
  size_t current_frame{0};

  std::atomic<bool> m_framebuffer_resized{false};

  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
};

VkResult create_debug_utils_messenger_EXT(VkInstance instance,
                                          VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo,
                                          VkAllocationCallbacks const* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);

void destroy_debug_utils_messenger_EXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       VkAllocationCallbacks const* pAllocator);

}  // namespace gar

#endif  // VULKAN_TOOLBOX_H
