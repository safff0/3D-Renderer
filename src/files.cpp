#include "files.h"

#include <sstream>
#include <string>

namespace engine {

OBJParser::OBJParser(const std::string& path) : file_{path} {
    ReadVerticies();
    ReadNormals();
    ReadFaces();
}

const std::vector<std::vector<Vector3>>& OBJParser::GetFaces() {
    return data_;
}

Index OBJParser::ParseId(const std::string& obj_id, Index size) {
    Signed id = std::atoi(obj_id.data());
    if (id < 0) {
        return size + id;
    }
    return id - 1;
}

void OBJParser::ReadVerticies() {
    for (std::string line; std::getline(file_, line);) {
        if (line.substr(0, 2) == "v ") {
            std::stringstream coords;
            coords.str(line.substr(2));
            Vector3 res;
            Index d = 0;
            for (std::string num; std::getline(coords, num, ' ') && d < 3;) {
                res[d] = std::atof(num.data());
                ++d;
            }
            verticies_.push_back(res);
        }
    }
}

void OBJParser::ReadNormals() {
    file_.clear();
    file_.seekg(0, std::ios::beg);
    for (std::string line; std::getline(file_, line);) {
        if (line.substr(0, 3) == "vn ") {
            std::stringstream coords;
            coords.str(line.substr(3));
            Vector3 res;
            Index d = 0;
            for (std::string num; std::getline(coords, num, ' ') && d < 3;) {
                res[d] = std::atof(num.data());
                ++d;
            }
            normals_.push_back(res);
        }
    }
}

void OBJParser::ReadFaces() {
    file_.clear();
    file_.seekg(0, std::ios::beg);
    for (std::string line; std::getline(file_, line);) {
        if (line.substr(0, 2) == "f ") {
            std::stringstream ids;
            ids.str(line.substr(2));
            std::vector<Vector3> face;
            std::vector<Vector3> normals;
            for (std::string f; std::getline(ids, f, ' ');) {
                Index slashes = std::count(f.begin(), f.end(), '/');
                std::string num;
                std::stringstream stream{f};
                std::getline(stream, num, '/');
                Index id = ParseId(num, verticies_.size());
                face.push_back(verticies_[id]);
                if (slashes == 2) {
                    std::string normal;
                    if (stream.peek() != '/') {
                        std::string temp;
                        std::getline(stream, temp, '/');
                    } else {
                        char temp;
                        stream.read(&temp, 1);
                    }
                    std::getline(stream, normal);
                    Index id = ParseId(normal, normals_.size());
                    normals.push_back(normals_[id]);
                }
            }
            Vector3 avg_normal{0, 0, 0};
            for (Index i = 0; i < normals.size(); ++i) {
                avg_normal += normals[i];
            }
            if (!normals.empty()) {
                avg_normal /= normals.size();
            }
            Vector3 normal = Plane3(face[0], face[1], face[2]).GetCoefficients();
            if (glm::dot(normal, avg_normal) >= 0) {
                std::reverse(face.begin(), face.end());
            }
            data_.push_back(face);
        }
    }
}

}  // namespace engine
