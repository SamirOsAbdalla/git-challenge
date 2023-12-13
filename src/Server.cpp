#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>

#define BUFFER_SIZE 1024
#define DECOMPRESSED_BUFFER_SIZE 1024
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
        if (flag == "-p")
        {
            const std::string sha = argv[3];
            const std::string path = ".git/objects/" + sha.substr(0, 2) + "/" + sha.substr(2);

            std::ofstream file(path, std::ios::binary);
            unsigned char buffer[BUFFER_SIZE];

            file << buffer;
            unsigned char decompressed_buffer[DECOMPRESSED_BUFFER_SIZE];
            uLongf decompressed_buffer_size = DECOMPRESSED_BUFFER_SIZE;
            int result = uncompress(decompressed_buffer, &decompressed_buffer_size, buffer, BUFFER_SIZE);
            if (result != Z_OK)
            {
                std::cerr << "Uncompression Error\n";
                return EXIT_FAILURE;
            }

            std::cout.write(reinterpret_cast<const char *>(decompressed_buffer), decompressed_buffer_size);
        }
        else
        {
            std::cerr << "Unknown command " << command << '\n';
            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
