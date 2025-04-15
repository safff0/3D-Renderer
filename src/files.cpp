#include "files.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "alias.h"
#include "geometry.h"

namespace engine {

namespace {

const std::string kVertexPrefix = "v ";
const std::string kNormalPrefix = "vn ";
const std::string kFacePrefix = "f ";

std::vector<std::string> StrSplit(const std::string& str, char delim) {
    std::stringstream stream{str};
    std::vector<std::string> result;
    for (std::string part; std::getline(stream, part, delim);) {
        result.push_back(part);
    }
    return result;
}

std::vector<std::string> GetLines(std::fstream& f) {
    std::vector<std::string> result;
    for (std::string part; std::getline(f, part, '\n');) {
        result.push_back(part);
    }
    return result;
}

}  // namespace

OBJParser::OBJParser(const FilePath& path) : file_{path} {
    for (auto line : GetLines(file_)) {
        if (line.starts_with(kVertexPrefix)) {
            auto coords = StrSplit(line.substr(kVertexPrefix.size()), ' ');
            assert(coords.size() >= 3 && "OBJ Parser: Bad vertex coordinates");
            verticies_.emplace_back(std::atof(coords[0].data()), std::atof(coords[1].data()),
                                    std::atof(coords[2].data()));
        } else if (line.starts_with(kNormalPrefix)) {
            auto coords = StrSplit(line.substr(kNormalPrefix.size()), ' ');
            assert(coords.size() >= 3 && "OBJ Parser: Bad normal coordinates");
            normals_.emplace_back(std::atof(coords[0].data()), std::atof(coords[1].data()),
                                  std::atof(coords[2].data()));
        }
    }
    Reset();
}

std::optional<Polygon> OBJParser::GetPolygon() {
    if (entries_.size() < Polygon::kVerticiesCount) {
        std::string line;
        do {
            if (!std::getline(file_, line)) {
                return std::nullopt;
            }
        } while (!line.starts_with(kFacePrefix));
        assert(line.starts_with(kFacePrefix) && "OBJ Parser: Unexpected line");
        entries_ = StrSplit(line.substr(kFacePrefix.size()), ' ');
    }
    Triangle3D verticies;
    std::vector<Vector3> normals;
    auto [v_id, n_id] = ParseEntry(entries_.front());
    verticies[0] = verticies_[v_id];
    if (n_id < normals_.size()) {
        normals.push_back(normals_[n_id]);
    }
    for (Index i = 1; i < Polygon::kVerticiesCount; ++i) {
        auto [v_id, n_id] = ParseEntry(entries_.back());
        entries_.pop_back();
        verticies[i] = verticies_[v_id];
        if (n_id < normals_.size()) {
            normals.push_back(normals_[n_id]);
        }
    }
    Vector3 poly_normal = Plane3{verticies}.GetCoefficients();
    if (glm::dot(poly_normal, Mean(normals)) >= 0) {
        std::reverse(verticies.begin(), verticies.end());
    }
    return Polygon{verticies};
}

void OBJParser::Reset() {
    if (!file_ && !file_.eof()) {
        std::cerr << "Error while reading .obj file\n";
    }
    file_.clear();
    file_.seekg(0, std::ios::beg);
}

Index OBJParser::ParseId(const std::string& obj_id, Index size) {
    Signed id = std::atoi(obj_id.data());
    if (id < 0) {
        return size + id;
    }
    return id - 1;
}

std::pair<Index, Index> OBJParser::ParseEntry(const std::string& entry) {
    auto ids = StrSplit(entry, '/');
    Index normal_id = normals_.size();
    if (ids.size() == 3) {
        normal_id = ParseId(ids.back(), normals_.size());
    }
    return std::make_pair(ParseId(ids.front(), verticies_.size()), normal_id);
}

}  // namespace engine
