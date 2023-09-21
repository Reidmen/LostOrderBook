#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace FileSystem {

class Encoding {
   public:
    Encoding() = delete;
    Encoding(const Encoding&) = delete;
    Encoding(Encoding&&) = delete;
    ~Encoding() = delete;

    Encoding& operator=(const Encoding&) = delete;
    Encoding& operator=(Encoding&&) = delete;

    // Convert system wide-string to UTF-8 encoding string
    static std::string toUTF8(std::wstring_view wstr);
};

// TODO: Path, Reader, Writer

class Path {
   protected:
    // path string
    std::string _path;

   public:
    Path() : _path(){};
    Path(const char* path) : _path(path){};
    Path(const std::string& path) : _path(path){};
    Path(const wchar_t* path) : _path(Encoding::toUTF8(path)){};
    Path(const Path&) = default;
    Path(Path&&) = default;
    ~Path() = default;

    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) = default;

    // Check if the path is not empty
    // explicit operator bool() const noexcept { return !empty(); }
};
// Filesystem
// It wraps file management operations
// (create, open, read, write, flush, close)
// Not thread-safe

// TODO: File inherits from Path, Reader, Writer
class File : public Path {
   public:
    static const size_t DEFAULT_BUFFER;

    File();
    File(const Path& path);
    File(const File& file);
    File(File&& file) noexcept;
    virtual ~File();

    File& operator=(const File& file);
    File& operator=(const File&& file) noexcept;
};

}  // namespace FileSystem

#endif
