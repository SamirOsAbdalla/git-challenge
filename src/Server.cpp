#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>
#include <vector>
#include <zlib.h>

#define BUFFER_SIZE 4096
int main(int argc, char *argv[])
{

    // Uncomment this block to pass the first stage

    if (argc < 2)
    {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[1];

    if (command == "init")
    {
        try
        {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");

            std::ofstream headFile(".git/HEAD");
            if (headFile.is_open())
            {
                headFile << "ref: refs/heads/master\n";
                headFile.close();
            }
            else
            {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }

            std::cout << "Initialized git directory\n";
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }
    else if (command == "cat-file" && argc == 4)
    {
        std::string flag = argv[2];
        if (flag != "-p")
        {
            std::cerr << "Invalid flag to cat-file\n";
            return EXIT_FAILURE;
        }

        const std::string blobSha = argv[3];
        const std::string path = ".git/objects/" + blobSha.substr(0, 2) + "/" + blobSha.substr(2);
        std::ifstream inputStream(path, std::ios::binary);

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inputStream), {});

        uLongf decompressedBufferSize = buffer.size() * 3;
        std::vector<unsigned char> decompressedBuffer(buffer.size() * 10);

        int result = uncompress((Bytef *)decompressedBuffer.data(), &decompressedBufferSize, (Bytef *)buffer.data(), BUFFER_SIZE);
        if (result != Z_OK)
        {
            std::cerr << "Error uncompressing";
            return EXIT_FAILURE;
        }

        std::string out_str = std::string(decompressedBuffer.begin(), decompressedBuffer.begin() + decompressedBufferSize);
        std::cout << out_str.substr(out_str.find('\0') + 1);
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
