#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

#include <cstdint>
#include <string>

#include <color.h>

namespace gar {

class Image_writer {
 public:
  /// \brief Write an iamge
  /// \param data The color of each pixel, from left to right, then top to
  /// bottom \param output_filename The output file to write to
  virtual bool write_image(std::vector<Color> const& data, std::string_view output_filename) = 0;
};

}  // namespace gar

#endif  // IMAGE_WRITER_H
