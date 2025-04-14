#pragma once
#include "alias.h"
#include "geometry.h"

#include <fstream>
#include <optional>
#include <string>

namespace engine {

class OBJParser {
public:
    OBJParser(const FilePath& path);

    std::optional<Polygon> GetPolygon();

    void Reset();

private:
    Index ParseId(const std::string& obj_id, Index size);
    std::pair<Index, Index> ParseEntry(const std::string& entry);

    std::fstream file_;
    std::vector<std::string> entries_;
    std::vector<Vector3> verticies_;
    std::vector<Vector3> normals_;
};

}  // namespace engine
