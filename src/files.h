#pragma once
#include "alias.h"
#include "geometry.h"

#include <fstream>
#include <string>

namespace engine {

class OBJParser {
public:
    OBJParser(const std::string& path);

    const std::vector<std::vector<Vector3>>& GetFaces();

private:
    Index ParseId(const std::string& obj_id, Index size);

    void ReadVerticies();
    void ReadNormals();
    void ReadFaces();

    std::fstream file_;
    std::vector<Vector3> verticies_;
    std::vector<Vector3> normals_;
    std::vector<std::vector<Vector3>> data_;
};

}  // namespace engine
