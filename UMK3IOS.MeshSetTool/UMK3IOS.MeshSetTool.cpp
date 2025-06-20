// UMK3IOS.MeshSetTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

struct header
{
    char mdlName[64];
    char textureName[64];
    int verts;
    int faces;
    float flt;
};

struct face {
    short f[3];
};

#pragma pack (push,1)
struct vert {
    short field0[3];
    float uv[2];
    float unk[3];
};
#pragma (pop)


int main(int argc, char* argv[])
{
    if (argc < 2 || argc == 1)
    {
        std::cout << "UMK3IOS.MeshSetTool by ermaccer\n"
            << "Usage: umk3ios <file>\n";
        return 1;
    }

    std::string inputFile = argv[argc - 1];

    std::ifstream pFile(inputFile, std::ifstream::binary);

    if (!pFile)
    {
        std::cout << "ERROR: Could not open " << inputFile << "!" << std::endl;;
        return 1;
    }


    int numModels = 0;

    pFile.read((char*)&numModels, sizeof(int));

    std::cout << "INFO: Models: " << numModels << std::endl;

    std::string input = inputFile;
    std::string folder = input.substr(0, input.find_last_of("."));

    folder += "_out";

    std::filesystem::create_directory(folder);
    std::filesystem::current_path(folder);

    for (int i = 0; i < numModels; i++)
    {
        header h;
        pFile.read((char*)&h, sizeof(header));

        std::cout << "Processing: " << h.mdlName << std::endl;

        std::string fileName = std::to_string(i);
        fileName += "_";
        fileName += h.mdlName;

        std::string material = fileName;
        material.insert(0, "mat_");
        material += ".mtl";

        std::ofstream mtl(material, std::ofstream::binary);


        std::string output = fileName;
        output += ".obj";
        std::ofstream obj(output, std::ofstream::binary);

        obj << "mtllib " << material << std::endl;

        std::vector<face> faces;
        for (int i = 0; i < h.faces; i++)
        {
            face f;
            pFile.read((char*)&f, sizeof(face));
            faces.push_back(f);
        }

        std::vector<vert> verts;
        for (int i = 0; i < h.verts; i++)
        {
            vert v;
            pFile.read((char*)&v, sizeof(vert));
            verts.push_back(v);
        }


        for (unsigned int i = 0; i < verts.size(); i++)
        {
            float x, y, z;
            x = static_cast<float>(verts[i].field0[0] / 32767.0f);
            y = static_cast<float>(verts[i].field0[1] / 32767.0f);
            z = static_cast<float>(verts[i].field0[2] / 32767.0f);

            obj << "v " << x << " " << y << " " << z << std::endl;
        }

        for (unsigned int i = 0; i < verts.size(); i++)
        {
            obj << "vt " << verts[i].uv[0] << " " << 1.0f - verts[i].uv[1] << std::endl;
        }

        obj << "g " << h.mdlName << std::endl;
        obj << "usemtl mat" << std::endl;


        for (unsigned int i = 0; i < faces.size(); i++)
        {
            obj << "f " << faces[i].f[0] + 1 << "/" << faces[i].f[0] + 1
                << " "
                << faces[i].f[1] + 1 << "/" << faces[i].f[1] + 1
                << " "
                << faces[i].f[2] + 1 << "/" << faces[i].f[2] + 1 << std::endl;

        }

        std::string textureName = h.textureName;
        textureName = textureName.substr(0, textureName.find_last_of("."));
        textureName += ".png";

        mtl << "newmtl mat" << std::endl;
        mtl << "Kd 1.00 1.00 1.00" << std::endl;
        mtl << "map_Kd " << textureName << std::endl;

        obj.close();
        mtl.close();
    }
    std::cout << "Finished." << std::endl;

    return 0;
}