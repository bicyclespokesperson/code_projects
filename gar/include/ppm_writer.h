#ifndef PPM_WRITER_H
#define PPM_WRITER_H

#include <image_writer.h>

namespace gar {

class Ppm_writer : public Image_writer {
 public:
  bool write_image(std::vector<Color> const& data, std::string_view output_filename) override;
};

}  // namespace gar

#endif  // PPM_WRITER_H
