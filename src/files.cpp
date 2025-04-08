#include "files.h"

#include <sstream>
#include <string>

namespace engine {

OBJParser::OBJParser(const std::string& path) : file_{path} {
    ReadVerticies();
    ReadFaces();
}

const std::vector<std::vector<Vector3>>& OBJParser::GetFaces() {
    return data_;
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

void OBJParser::ReadFaces() {
    file_.clear();
    file_.seekg(0, std::ios::beg);
    for (std::string line; std::getline(file_, line);) {
        if (line.substr(0, 2) == "f ") {
            std::stringstream ids;
            ids.str(line.substr(2));
            std::vector<Vector3> face;
            for (std::string f; std::getline(ids, f, ' ');) {
                std::string num;
                std::getline(std::stringstream{f}, num, '/');
                Signed id = std::atoi(num.data());
                if (id < 0) {
                    id = verticies_.size() - id;
                } else {
                    --id;
                }
                face.push_back(verticies_[id]);
            }
            std::reverse(face.begin(), face.end());
            data_.push_back(face);
        }
    }
}

}  // namespace engine
