// Author: cute-giggle@outlook.com

#include <vector>
#include <string>
#include <filesystem>

namespace fsaverage
{

struct Color
{
    int r{};
    int g{};
    int b{};
    int a{};
    std::string name;
};

class Surface
{
public:
    virtual ~Surface() = default;

    bool load(const std::filesystem::path& path) noexcept;

    std::vector<float> point;
    std::vector<uint32_t> face;
    std::vector<uint32_t> label;
    std::vector<Color> lut;

private:
    static std::pair<std::vector<float>, std::vector<uint32_t>> loadMesh(std::ifstream& input) noexcept;
    static std::vector<uint32_t> loadLabel(std::ifstream& input) noexcept;
    static std::vector<Color> loadLut(std::ifstream& input) noexcept;

    bool loadMesh(const std::filesystem::path& path) noexcept;
    bool loadLabel(const std::filesystem::path& path) noexcept;
    bool loadLut(const std::filesystem::path& path) noexcept;
};

}