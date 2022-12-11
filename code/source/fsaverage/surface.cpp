// Author: cute-giggle@outlook.com

#include "fsaverage/surface.h"

#include <fstream>
#include <map>
#include <functional>

#include "utils/mlog.h"

namespace fsaverage
{

bool Surface::load(const std::filesystem::path& path) noexcept
{
    if (!std::filesystem::exists(path))
    {
        MLOG_ERROR(path, " does not exists.");
        return false;
    }

    using LoadFuncT = std::function<bool(const std::filesystem::path&)>;
    static const std::map<std::string, LoadFuncT> mapper
    {
        {".lut", [this](const std::filesystem::path& path) -> bool { return this->loadLut(path); }},
        {".mesh", [this](const std::filesystem::path& path) -> bool { return this->loadMesh(path); }},
        {".label", [this](const std::filesystem::path& path) -> bool { return this->loadLabel(path); }}
    };
    auto iter = mapper.find(path.extension().string());
    if (iter == mapper.end())
    {
        MLOG_ERROR(path, " has invalid extension, only support [.mesh/.label/.lut] files.");
        return false;
    }
    return iter->second(path);
}

bool Surface::loadMesh(const std::filesystem::path& path) noexcept
{
    std::ifstream input(path, std::ios_base::binary);
    if (!input.is_open())
    {
        MLOG_ERROR(path, " open failed.");
        return false;
    }
    auto [point, face] = loadMesh(input);
    if (input.fail())
    {
        MLOG_ERROR(path, " load failed.");
        return false;
    }
    this->point = std::move(point);
    this->face = std::move(face);
    return true;
}

bool Surface::loadLabel(const std::filesystem::path& path) noexcept
{
    std::ifstream input(path, std::ios_base::binary);
    if (!input.is_open())
    {
        MLOG_ERROR(path, " open failed.");
        return false;
    }
    auto label = loadLabel(input);
    if (input.fail())
    {
        MLOG_ERROR(path, " load failed.");
        return false;
    }
    this->label = std::move(label);
    return true;
}

bool Surface::loadLut(const std::filesystem::path& path) noexcept
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        MLOG_ERROR(path, " open failed.");
        return false;
    }
    this->lut = loadLut(input);
    return true;
}

std::pair<std::vector<float>, std::vector<uint32_t>> Surface::loadMesh(std::ifstream& input) noexcept
{
    uint32_t count = 0U;
    input.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    std::vector<float> point(count * 3U, 0.f);
    input.read(reinterpret_cast<char*>(point.data()), point.size() * sizeof(float));
    input.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    std::vector<uint32_t> face(count * 3U, 0U);
    input.read(reinterpret_cast<char*>(face.data()), face.size() * sizeof(uint32_t));
    return {std::move(point), std::move(face)};
}

std::vector<uint32_t> Surface::loadLabel(std::ifstream& input) noexcept
{
    uint32_t count = 0U;
    input.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    std::vector<uint32_t> label(count, 0U);
    input.read(reinterpret_cast<char*>(label.data()), count * sizeof(uint32_t));
    return label;
}

std::vector<Color> Surface::loadLut(std::ifstream& input) noexcept
{
    std::string buffer;
    std::vector<Color> lut;
    while(std::getline(input, buffer))
    {
        Color color;
        std::stringstream ss(buffer);
        ss >> color.r >> color.g >> color.b >> color.a >> color.name;
        lut.emplace_back(std::move(color));
    }
    return lut;
}

}

