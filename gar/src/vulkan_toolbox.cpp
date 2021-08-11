// Based on the vulkan tutorial here: https://vulkan-tutorial.com/Introduction
#include <pch.hpp>

#include <vulkan_toolbox.h>

namespace gar {

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif

constexpr int32_t WINDOW_WIDTH{800};
constexpr int32_t WINDOW_HEIGHT{600};
constexpr int32_t MAX_FRAMES_IN_FLIGHT = 2;

struct Uniform_buffer_object {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct Queue_family_indices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool is_complete() { return graphics_family.has_value() && present_family.has_value(); }
};

struct Swap_chain_support_details {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

std::vector<char const*> const validation_layers = {"VK_LAYER_KHRONOS_validation"};

std::vector<char const*> const required_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

std::vector<char> Vulkan_app::read_file(std::string const& filename) {
  std::ifstream infile(filename, std::ios::ate | std::ios::binary);

  if (!infile.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t file_size = (size_t)infile.tellg();
  std::vector<char> buffer(file_size);
  infile.seekg(0);
  infile.read(buffer.data(), file_size);
  infile.close();

  return buffer;
}

bool draw_triangle() {
  try {
    Vulkan_app app;
    app.main_loop();
  } catch (std::exception const& err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

VkResult create_debug_utils_messenger_EXT(VkInstance instance,
                                          VkDebugUtilsMessengerCreateInfoEXT const* create_info,
                                          VkAllocationCallbacks const* p_allocator,
                                          VkDebugUtilsMessengerEXT* p_debug_messenger) {
  // Since we're using an extension to set up the error callback, we need to
  // look up the function to call (it's not present automatically)
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_EXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debug_messenger,
                                       VkAllocationCallbacks const* p_allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debug_messenger, p_allocator);
  }
}

void Vulkan_app::framebuffer_resize_callback(GLFWwindow* window, int /* width */, int /* height */) {
  auto app = reinterpret_cast<Vulkan_app*>(glfwGetWindowUserPointer(window));
  app->m_framebuffer_resized = true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_app::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
                                                          VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                                                          VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
                                                          void* /*pUserData*/) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;

  return VK_FALSE;
}

Vulkan_app::Vulkan_app() {
  init_window();
  init_vulkan();
}

Vulkan_app::~Vulkan_app() {
  cleanup();
}

void Vulkan_app::init_vulkan() {
  create_instance();
  setup_debug_messenger();
  create_surface();
  pick_physical_device();
  create_logical_device();
  load_model();
  create_swap_chain();
  create_image_views();
  create_render_pass();
  create_descriptor_set_layout();
  create_graphics_pipeline();
  create_command_pool();
  create_color_resources();
  create_depth_resources();
  create_frame_buffers();
  create_texture_image();
  create_texture_image_view();
  create_texture_sampler();
  create_vertex_buffer();
  create_index_buffer();
  create_uniform_buffers();
  create_descriptor_pool();
  create_descriptor_sets();
  create_command_buffers();
  create_sync_objects();
}

void Vulkan_app::create_surface() {
  if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void Vulkan_app::create_logical_device() {
  Queue_family_indices indices = find_queue_families(m_physical_device);

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queue_family;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = queue_create_infos.size();
  create_info.pQueueCreateInfos = queue_create_infos.data();

  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size());
  create_info.ppEnabledExtensionNames = required_device_extensions.data();

  if (enable_validation_layers) {
    // These fields are ignored by up-to-date implementations, since there are
    // no longer device and instance specific validation layers
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
  vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
}

bool Vulkan_app::check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  return ranges::all_of(validation_layers, [&](char const* name) {
    return ranges::find_if(available_layers, [&](auto const& properties) {
             return !strncmp(properties.layerName, name, strlen(name));
           }) != ranges::end(available_layers);
  });
}

void Vulkan_app::create_instance() {
  if (enable_validation_layers && !check_validation_layer_support()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  // Info about our application to let the driver optimize for what we're doing
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  // Desired extensions and validation layers
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  uint32_t glfwExtensionCount{0};
  char const** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  create_info.enabledExtensionCount = glfwExtensionCount;
  create_info.ppEnabledExtensionNames = glfwExtensions;
  if (enable_validation_layers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  auto req_extensions = get_required_extensions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(req_extensions.size());
  create_info.ppEnabledExtensionNames = req_extensions.data();

  uint32_t extension_count{0};
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

  std::cout << "available extensions:" << std::endl;
  for (auto const& extension : extensions) {
    std::cout << "\t" << extension.extensionName << std::endl;
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

  if (enable_validation_layers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();

    populate_debug_messenger_create_info(debugCreateInfo);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  } else {
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;
  }

  if (auto result = vkCreateInstance(&create_info, nullptr, &m_instance); result != VK_SUCCESS) {
    throw std::runtime_error(std::string{"Failed to create vulkan instance. VkResult: "} +
                             std::to_string(static_cast<int>(result)));
  }
}

void Vulkan_app::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
}

void Vulkan_app::setup_debug_messenger() {
  if (!enable_validation_layers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  populate_debug_messenger_create_info(create_info);

  if (create_debug_utils_messenger_EXT(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

std::vector<char const*> Vulkan_app::get_required_extensions() {
  uint32_t glfwExtensionCount = 0;
  char const** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<char const*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enable_validation_layers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void Vulkan_app::pick_physical_device() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

  auto res = ranges::find_if(devices, [&](auto const& device) { return is_device_suitable(device); });
  if (res != ranges::end(devices)) {
    m_physical_device = *res;
    m_msaa_samples = calc_max_usable_sample_count();
  }

  if (m_physical_device == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool Vulkan_app::is_device_suitable(VkPhysicalDevice device) {
  Queue_family_indices indices = find_queue_families(device);

  bool const extensions_supported = check_device_extension_support(device);

  bool swap_chain_adaquate{false};
  if (extensions_supported) {
    auto swap_chain_support = query_swap_chain_support(device);
    swap_chain_adaquate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
  }

  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);

  return indices.is_complete() && extensions_supported && swap_chain_adaquate && supported_features.samplerAnisotropy;
}

// Could just call this once and cache the results; code is simple with
// multiple calls though.
Queue_family_indices Vulkan_app::find_queue_families(VkPhysicalDevice device) {
  Queue_family_indices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  for (auto const&& [i, queue_family] : queue_families | ranges::view::enumerate) {
    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
    if (queue_family.queueCount > 0 && present_support) {
      indices.present_family = i;
    }

    if (indices.is_complete()) {
      break;
    }
  }

  return indices;
}

Swap_chain_support_details Vulkan_app::query_swap_chain_support(VkPhysicalDevice device) {
  Swap_chain_support_details details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.present_modes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.present_modes.data());
  }

  return details;
}

VkExtent2D Vulkan_app::choose_swap_extent(VkSurfaceCapabilitiesKHR const& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    // Our resolution is equal to the window we're drawing to
    return capabilities.currentExtent;
  } else {
    int32_t width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    // Choose the available resolution closest to our desired window size
    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

VkSurfaceFormatKHR Vulkan_app::choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> const& available_formats) {
  auto res = ranges::find_if(available_formats, [](auto const& format) {
    return format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });
  return (res != ranges::end(available_formats)) ? *res : available_formats[0];
}

VkPresentModeKHR Vulkan_app::choose_swap_present_mode(std::vector<VkPresentModeKHR> const& available_present_modes) {
  auto res =
      ranges::find_if(available_present_modes, [](auto const& mode) { return mode == VK_PRESENT_MODE_MAILBOX_KHR; });
  return (res != ranges::end(available_present_modes)) ? *res : VK_PRESENT_MODE_FIFO_KHR;
}

void Vulkan_app::create_swap_chain() {
  Swap_chain_support_details swap_chain_support = query_swap_chain_support(m_physical_device);

  VkSurfaceFormatKHR const surface_format = choose_swap_surface_format(swap_chain_support.formats);
  m_swap_chain_image_format = surface_format.format;
  VkPresentModeKHR const present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
  m_swap_chain_extent = choose_swap_extent(swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  // max_image_count==0 -> no max image count
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = m_surface;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = m_swap_chain_extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // Might be different for
  // post-processing

  auto const indices = find_queue_families(m_physical_device);
  uint32_t queueFamilyIndices[] = {indices.graphics_family.value(), indices.present_family.value()};

  // Concurrent mode doesn't require explicit ownership transfers, but
  // has slightly worse performance
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;      // Optional
    create_info.pQueueFamilyIndices = nullptr;  // Optional
  }

  create_info.preTransform = swap_chain_support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;

  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
  m_swap_chain_images.resize(image_count);
  vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());
}

void Vulkan_app::create_image_views() {
  m_swap_chain_image_views.resize(m_swap_chain_images.size());

  for (uint32_t i = 0; i < m_swap_chain_images.size(); i++) {
    m_swap_chain_image_views[i] =
        create_image_view(m_swap_chain_images[i], m_swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
  }
}

bool Vulkan_app::check_device_extension_support(VkPhysicalDevice device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

  return ranges::all_of(required_device_extensions, [&](char const* extension_name) {
    return ranges::find_if(available_extensions, [&](auto const& extension) {
             return !strncmp(extension_name, extension.extensionName, strlen(extension_name));
           }) != ranges::end(available_extensions);
  });

  /* std::set<std::string>
   * required_extensions(required_device_extensions.begin(),
   * required_device_extensions.end()); */
  /* for (auto const& extension : available_extensions) { */
  /*     required_extensions.erase(extension.extensionName); */
  /* } */

  /* return required_extensions.empty(); */
}

void Vulkan_app::init_window() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
}

void Vulkan_app::main_loop() {
  while (!glfwWindowShouldClose(m_window)) {
    glfwPollEvents();
    draw_frame();
  }

  vkDeviceWaitIdle(m_device);
}

void Vulkan_app::cleanup_swap_chain() {
  vkDestroyImageView(m_device, m_color_image_view, nullptr);
  vkDestroyImage(m_device, m_color_image, nullptr);
  vkFreeMemory(m_device, m_color_image_memory, nullptr);

  vkDestroyImageView(m_device, m_depth_image_view, nullptr);
  vkDestroyImage(m_device, m_depth_image, nullptr);
  vkFreeMemory(m_device, m_depth_image_memory, nullptr);

  for (size_t i = 0; i < m_swap_chain_framebuffers.size(); i++) {
    vkDestroyFramebuffer(m_device, m_swap_chain_framebuffers[i], nullptr);
  }

  vkFreeCommandBuffers(m_device, m_command_pool, static_cast<uint32_t>(m_command_buffers.size()),
                       m_command_buffers.data());

  vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
  vkDestroyRenderPass(m_device, m_render_pass, nullptr);

  for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
    vkDestroyImageView(m_device, m_swap_chain_image_views[i], nullptr);
  }

  vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);

  for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
    vkDestroyBuffer(m_device, m_uniform_buffers[i], nullptr);
    vkFreeMemory(m_device, m_uniform_buffers_memory[i], nullptr);
  }

  vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
}

void Vulkan_app::cleanup() {
  cleanup_swap_chain();

  vkDestroySampler(m_device, m_texture_sampler, nullptr);
  vkDestroyImageView(m_device, m_texture_image_view, nullptr);

  vkDestroyImage(m_device, m_texture_image, nullptr);
  vkFreeMemory(m_device, m_texture_image_memory, nullptr);

  vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);
  vkDestroyBuffer(m_device, m_index_buffer, nullptr);
  vkFreeMemory(m_device, m_index_buffer_memory, nullptr);

  vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
  vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(m_device, m_render_finished_semaphores[i], nullptr);
    vkDestroySemaphore(m_device, m_image_available_semaphores[i], nullptr);
    vkDestroyFence(m_device, m_inflight_fences[i], nullptr);
  }

  vkDestroyCommandPool(m_device, m_command_pool, nullptr);

  vkDestroyDevice(m_device, nullptr);

  if (enable_validation_layers) {
    destroy_debug_utils_messenger_EXT(m_instance, m_debug_messenger, nullptr);
  }

  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);

  glfwDestroyWindow(m_window);

  glfwTerminate();
}

void Vulkan_app::create_graphics_pipeline() {
  auto vert_shader_code = read_file("shaders/shader.vert.spv");
  auto frag_shader_code = read_file("shaders/shader.frag.spv");

  VkShaderModule vert_shader_module = create_shader_module(vert_shader_code);
  VkShaderModule frag_shader_module = create_shader_module(frag_shader_code);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr;  // Used for defining constants in shders

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  auto binding_description = Vertex::get_binding_description();
  auto attribute_descriptions = Vertex::get_attribute_descriptions();

  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_swap_chain_extent.width);
  viewport.height = static_cast<float>(m_swap_chain_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = m_swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  // Changing most of these requires turning on features on the GPU
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;  // In fragments
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
  rasterizer.depthBiasClamp = 0.0f;           // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;     // Optional
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = m_msaa_samples;
  multisampling.minSampleShading = 1.0f;           // Optional
  multisampling.pSampleMask = nullptr;             // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
  multisampling.alphaToOneEnable = VK_FALSE;       // Optional

  // Don't need depth and stencil buffers yet, but their structs would be
  // initialized here
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &colorBlendAttachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  // Can use a VkPipelineDynamicStateCreateInfo struct to change a few things
  // about the pipeline. Not used in this app though.

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &m_descriptor_set_layout;
  pipeline_layout_info.pushConstantRangeCount = 0;     // Optional
  pipeline_layout_info.pPushConstantRanges = nullptr;  // Optional
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.minDepthBounds = 0.0f;  // Optional
  depth_stencil.maxDepthBounds = 1.0f;  // Optional
  depth_stencil.stencilTestEnable = VK_FALSE;
  depth_stencil.front = {};  // Optional
  depth_stencil.back = {};   // Optional

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shaderStages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = nullptr;  // Optional
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = nullptr;  // Optional
  pipeline_info.layout = m_pipeline_layout;
  pipeline_info.renderPass = m_render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipeline_info.basePipelineIndex = -1;               // Optional
  pipeline_info.pDepthStencilState = &depth_stencil;

  if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  vkDestroyShaderModule(m_device, frag_shader_module, nullptr);
  vkDestroyShaderModule(m_device, vert_shader_module, nullptr);
}

VkShaderModule Vulkan_app::create_shader_module(std::vector<char> const& code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<uint32_t const*>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

void Vulkan_app::create_render_pass() {
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = m_swap_chain_image_format;
  color_attachment.samples = m_msaa_samples;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment = {};
  depth_attachment.format = find_depth_format();
  depth_attachment.samples = m_msaa_samples;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription color_attachment_resolve = {};
  color_attachment_resolve.format = m_swap_chain_image_format;
  color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_resolve_ref = {};
  color_attachment_resolve_ref.attachment = 2;
  color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;
  subpass.pResolveAttachments = &color_attachment_resolve_ref;

  std::array<VkAttachmentDescription, 3> attachments = {color_attachment, depth_attachment, color_attachment_resolve};
  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Vulkan_app::create_frame_buffers() {
  m_swap_chain_framebuffers.resize(m_swap_chain_image_views.size());

  for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
    std::array<VkImageView, 3> attachments = {
        m_color_image_view,
        m_depth_image_view,  // Only need one since we only ever run one subpass at a time
        m_swap_chain_image_views[i]};

    VkFramebufferCreateInfo frame_buffer_info = {};
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_info.renderPass = m_render_pass;
    frame_buffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    frame_buffer_info.pAttachments = attachments.data();
    frame_buffer_info.width = m_swap_chain_extent.width;
    frame_buffer_info.height = m_swap_chain_extent.height;
    frame_buffer_info.layers = 1;

    if (vkCreateFramebuffer(m_device, &frame_buffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Vulkan_app::create_command_pool() {
  auto queueFamilyIndices = find_queue_families(m_physical_device);

  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queueFamilyIndices.graphics_family.value();
  pool_info.flags = 0;  // Optional

  if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void Vulkan_app::create_command_buffers() {
  m_command_buffers.resize(m_swap_chain_framebuffers.size());

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = m_command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

  if (vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < m_command_buffers.size(); i++) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;                   // Optional
    begin_info.pInheritanceInfo = nullptr;  // Optional

    if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = m_render_pass;
    render_pass_info.framebuffer = m_swap_chain_framebuffers[i];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = m_swap_chain_extent;

    // Should be in the same order as the attachments
    std::array<VkClearValue, 2> clear_values = {};
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

    vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

    VkBuffer vertex_buffers[] = {m_vertex_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_command_buffers[i], 0, 1, vertex_buffers, offsets);

    vkCmdBindIndexBuffer(m_command_buffers[i], m_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1,
                            &m_descriptor_sets[i], 0, nullptr);
    vkCmdDrawIndexed(m_command_buffers[i], static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(m_command_buffers[i]);
    if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}

void Vulkan_app::create_sync_objects() {
  m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_inflight_fences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
        vkCreateFence(m_device, &fence_info, nullptr, &m_inflight_fences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

void Vulkan_app::draw_frame() {
  auto wait_for_all = VK_TRUE;  // false -> wait for any of the fences to signal
  vkWaitForFences(m_device, 1, &m_inflight_fences[current_frame], wait_for_all, std::numeric_limits<uint64_t>::max());

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(m_device, m_swap_chain, std::numeric_limits<uint64_t>::max(),
                                          m_image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreate_swap_chain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  update_uniform_buffer(image_index);

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {m_image_available_semaphores[current_frame]};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &m_command_buffers[image_index];

  VkSemaphore signalSemaphores[] = {m_render_finished_semaphores[current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signalSemaphores;

  vkResetFences(m_device, 1, &m_inflight_fences[current_frame]);

  if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_inflight_fences[current_frame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swap_chains[] = {m_swap_chain};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;  // Optional, used to store n output retvals
  // instead of just returning one

  result = vkQueuePresentKHR(m_present_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebuffer_resized) {
    m_framebuffer_resized = false;
    recreate_swap_chain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Vulkan_app::recreate_swap_chain() {
  int width = 0, height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(m_window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(m_device);

  cleanup_swap_chain();

  create_swap_chain();
  create_image_views();
  create_render_pass();
  create_graphics_pipeline();
  create_color_resources();
  create_depth_resources();
  create_frame_buffers();
  create_uniform_buffers();
  create_descriptor_pool();
  create_descriptor_sets();
  create_command_buffers();
}

void Vulkan_app::create_vertex_buffer() {
  VkDeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();

  // Buffer we can access, that the GPU can copy from into an inaccessible, high performance internal memory.
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer,
                staging_buffer_memory);

  void* data;
  vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, m_vertices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(m_device, staging_buffer_memory);

  create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertex_buffer, m_vertex_buffer_memory);

  copy_buffer(staging_buffer, m_vertex_buffer, buffer_size);

  vkDestroyBuffer(m_device, staging_buffer, nullptr);
  vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void Vulkan_app::create_index_buffer() {
  VkDeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer,
                staging_buffer_memory);

  void* data;
  vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, m_indices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(m_device, staging_buffer_memory);

  create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_index_buffer, m_index_buffer_memory);

  copy_buffer(staging_buffer, m_index_buffer, buffer_size);

  vkDestroyBuffer(m_device, staging_buffer, nullptr);
  vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void Vulkan_app::create_buffer(VkDeviceSize size,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               VkBuffer& buffer,
                               VkDeviceMemory& bufferMemory) {
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

  // The optimal way to do this is to call vkAllocateMemory as few times as possible,
  // and manually split its results between the different uses for the application.
  // Not necessary for a small app though.
  if (vkAllocateMemory(m_device, &alloc_info, nullptr, &bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void Vulkan_app::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer command_buffer = begin_single_time_commands();

  VkBufferCopy copyRegion = {};
  copyRegion.size = size;
  vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);

  end_single_time_commands(command_buffer);
}

uint32_t Vulkan_app::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void Vulkan_app::create_descriptor_set_layout() {
  VkDescriptorSetLayoutBinding ubo_layout_binding = {};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  ubo_layout_binding.pImmutableSamplers = nullptr;  // Optional

  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding, samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_info.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(m_device, &layout_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void Vulkan_app::create_uniform_buffers() {
  VkDeviceSize buffer_size = sizeof(Uniform_buffer_object);

  m_uniform_buffers.resize(m_swap_chain_images.size());
  m_uniform_buffers_memory.resize(m_swap_chain_images.size());

  for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
    create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniform_buffers[i],
                  m_uniform_buffers_memory[i]);
  }
}

void Vulkan_app::create_descriptor_pool() {
  std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = static_cast<uint32_t>(m_swap_chain_images.size());
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = static_cast<uint32_t>(m_swap_chain_images.size());

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = static_cast<uint32_t>(m_swap_chain_images.size());

  if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void Vulkan_app::update_uniform_buffer(uint32_t current_image) {
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();
  float elapsed_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

  Uniform_buffer_object ubo = {};
  ubo.model = glm::rotate(glm::mat4(1.0f), elapsed_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj =
      glm::perspective(glm::radians(45.0f), m_swap_chain_extent.width / (float)m_swap_chain_extent.height, 0.1f, 10.0f);

  // Y coordinate is inverted (OGL style) by default. Uninvert it here
  ubo.proj[1][1] *= -1;

  // Push data to shader. Another, possibly more efficient way of doing this is "push constants"
  void* data;
  vkMapMemory(m_device, m_uniform_buffers_memory[current_image], 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(m_device, m_uniform_buffers_memory[current_image]);
}

void Vulkan_app::create_descriptor_sets() {
  std::vector<VkDescriptorSetLayout> layouts(m_swap_chain_images.size(), m_descriptor_set_layout);
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = m_descriptor_pool;
  alloc_info.descriptorSetCount = static_cast<uint32_t>(m_swap_chain_images.size());
  alloc_info.pSetLayouts = layouts.data();

  m_descriptor_sets.resize(m_swap_chain_images.size());
  if (vkAllocateDescriptorSets(m_device, &alloc_info, m_descriptor_sets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = m_uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(Uniform_buffer_object);

    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = m_texture_image_view;
    image_info.sampler = m_texture_sampler;

    std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};

    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = m_descriptor_sets[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = m_descriptor_sets[i];
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0,
                           nullptr);
  }
}

void Vulkan_app::create_texture_image() {
  std::string const image_path{"textures/chalet.jpg"};

  int32_t tex_width;
  int32_t tex_height;
  int32_t tex_channels;
  stbi_uc* pixels = stbi_load(image_path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

  VkDeviceSize image_size = tex_width * tex_height * 4;
  m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(tex_width, tex_height)))) + 1;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer,
                staging_buffer_memory);

  void* data;
  vkMapMemory(m_device, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(m_device, staging_buffer_memory);

  stbi_image_free(pixels);

  create_image(tex_width, tex_height, m_mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_texture_image, m_texture_image_memory);

  transition_image_layout(m_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mip_levels);
  copy_buffer_to_image(staging_buffer, m_texture_image, static_cast<uint32_t>(tex_width),
                       static_cast<uint32_t>(tex_height));
  // Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

  generate_mipmaps(m_texture_image, VK_FORMAT_R8G8B8A8_UNORM, tex_width, tex_height, m_mip_levels);

  vkDestroyBuffer(m_device, staging_buffer, nullptr);
  vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void Vulkan_app::create_image(uint32_t width,
                              uint32_t height,
                              uint32_t mip_levels,
                              VkSampleCountFlagBits sample_count,
                              VkFormat format,
                              VkImageTiling tiling,
                              VkImageUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              VkImage& image,
                              VkDeviceMemory& image_memory) {
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = mip_levels;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = sample_count;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(m_device, &image_info, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(m_device, image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(m_device, image, image_memory, 0);
}

VkCommandBuffer Vulkan_app::begin_single_time_commands() {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = m_command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer);

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &begin_info);

  return command_buffer;
}

void Vulkan_app::end_single_time_commands(VkCommandBuffer command_buffer) {
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(m_graphics_queue);

  vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
}

void Vulkan_app::transition_image_layout(VkImage image,
                                         VkFormat format,
                                         VkImageLayout old_layout,
                                         VkImageLayout new_layout,
                                         uint32_t mip_levels) {
  VkCommandBuffer command_buffer = begin_single_time_commands();

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  barrier.image = image;
  if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (has_stencil_component(format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mip_levels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  end_single_time_commands(command_buffer);
}

void Vulkan_app::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
  VkCommandBuffer command_buffer = begin_single_time_commands();

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  end_single_time_commands(command_buffer);
}

VkImageView Vulkan_app::create_image_view(VkImage image,
                                          VkFormat format,
                                          VkImageAspectFlags aspect_flags,
                                          uint32_t mip_levels) {
  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = mip_levels;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
  view_info.subresourceRange.aspectMask = aspect_flags;

  VkImageView image_view;
  if (vkCreateImageView(m_device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return image_view;
}

void Vulkan_app::create_texture_image_view() {
  m_texture_image_view =
      create_image_view(m_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_mip_levels);
}

void Vulkan_app::create_texture_sampler() {
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_TRUE;
  sampler_info.maxAnisotropy = 16;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.minLod = 0;  // Optional
  sampler_info.maxLod = static_cast<float>(m_mip_levels);
  sampler_info.mipLodBias = 0;  // Optional
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

  if (vkCreateSampler(m_device, &sampler_info, nullptr, &m_texture_sampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

VkFormat Vulkan_app::find_supported_format(std::vector<VkFormat> const& candidates,
                                           VkImageTiling tiling,
                                           VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}

bool Vulkan_app::has_stencil_component(VkFormat format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat Vulkan_app::find_depth_format() {
  return find_supported_format({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void Vulkan_app::create_depth_resources() {
  VkFormat depth_format = find_depth_format();

  create_image(m_swap_chain_extent.width, m_swap_chain_extent.height, 1, m_msaa_samples, depth_format,
               VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_image_memory);
  m_depth_image_view = create_image_view(m_depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
  transition_image_layout(m_depth_image, depth_format, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

void Vulkan_app::load_model() {
  std::string const model_path{"models/chalet.obj"};

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Vertex, uint32_t> unique_vertices;

  for (auto const& shape : shapes) {
    for (auto const& index : shape.mesh.indices) {
      Vertex vertex = {};

      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0],
                          1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
        m_vertices.push_back(vertex);
      }
      m_indices.push_back(unique_vertices[vertex]);
    }
  }
}

void Vulkan_app::generate_mipmaps(VkImage image,
                                  VkFormat image_format,
                                  int32_t tex_width,
                                  int32_t tex_height,
                                  uint32_t mip_levels) {
  // Check if image format supports linear blitting
  VkFormatProperties format_properties;
  vkGetPhysicalDeviceFormatProperties(m_physical_device, image_format, &format_properties);
  if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error("texture image format does not support linear blitting!");
  }

  VkCommandBuffer command_buffer = begin_single_time_commands();

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;

  int32_t mip_width = tex_width;
  int32_t mip_height = tex_height;

  for (uint32_t i = 1; i < mip_levels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    VkImageBlit blit = {};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mip_width, mip_height, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);

    mip_width = std::max(1, mip_width / 2);
    mip_height = std::max(1, mip_height / 2);
  }

  barrier.subresourceRange.baseMipLevel = mip_levels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  end_single_time_commands(command_buffer);
}

VkSampleCountFlagBits Vulkan_app::calc_max_usable_sample_count() {
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(m_physical_device, &physical_device_properties);

  VkSampleCountFlags counts = std::min(physical_device_properties.limits.framebufferColorSampleCounts,
                                       physical_device_properties.limits.framebufferDepthSampleCounts);

  // Find the largest sample count that the device supports
  std::vector<VkSampleCountFlagBits> sample_counts{VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT,
                                                   VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT,
                                                   VK_SAMPLE_COUNT_4_BIT,  VK_SAMPLE_COUNT_2_BIT};
  auto result = ranges::find_if(sample_counts, [&](auto flag) { return flag & counts; });
  return (result != ranges::end(sample_counts)) ? *result : VK_SAMPLE_COUNT_1_BIT;
}

void Vulkan_app::create_color_resources() {
  VkFormat color_format = m_swap_chain_image_format;

  create_image(m_swap_chain_extent.width, m_swap_chain_extent.height, 1, m_msaa_samples, color_format,
               VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_color_image, m_color_image_memory);
  m_color_image_view = create_image_view(m_color_image, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

  transition_image_layout(m_color_image, color_format, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
}

}  // namespace gar
