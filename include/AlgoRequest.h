#ifndef ALGO_REQUEST_H
#define ALGO_REQUEST_H

#include <memory>
#include <string>
#include <vector>

// Enum to represent supported image formats
enum class ImageFormat { YUV420, YUV422, YUV444, RGB, GRAYSCALE, UNKNOWN };

// Struct to represent an individual image
struct ImageData {
  ImageFormat format;              // Format of the image (e.g., YUV, RGB)
  std::vector<unsigned char> data; // Raw image data
  int width;                       // Width of the image
  int height;                      // Height of the image
  int fd;                          // File descriptor, -1 if not available

  // Constructor
  ImageData(ImageFormat fmt, int w, int h, int fileDesc = -1)
      : format(fmt), width(w), height(h), fd(fileDesc) {}
};

class AlgoRequest {
private:
  std::vector<std::shared_ptr<ImageData>> images; // Collection of images

public:
  // Add an image to the collection
  void AddImage(ImageFormat format, int width, int height,
                const std::vector<unsigned char> &rawData, int fd = -1);

  // Get the total number of images
  size_t GetImageCount() const;

  // Get an image by index
  std::shared_ptr<ImageData> GetImage(size_t index) const;

  // Retrieve all images in YUV format
  std::vector<std::shared_ptr<ImageData>> GetYUVImages() const;

  // Clear all stored images
  void ClearImages();

  // Destructor
  ~AlgoRequest() = default;

  size_t mProcessCnt = 0;
  int mRequestId; /*request id assoisiated*/

private:
};

#endif // ALGO_REQUEST_H