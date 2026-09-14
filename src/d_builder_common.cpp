/***********************************************************************************************************************
 * @date 2026-01-04
 * @author Gregory Nitch
 *
 * @brief Implementations for common functions in the D_Builder application.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <filesystem>
#include <sstream>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_builder_common.hpp"

/***********************************************************************************************************************
 * @brief Ensures that image input, loaded and output folders exists and have images in the requried places.
 **********************************************************************************************************************/
void init_img_dirs(void)
{
    std::filesystem::path base_img_path = DEFAULT_BASE_IMG_PATH;
    std::filesystem::path input_path = DEFAULT_INPUT_IMG_PATH;
    std::filesystem::path loaded_path = DEFAULT_SECTION_IMG_LOADED_PATH;
    std::filesystem::path output_path = DEFAULT_OUTPUT_IMG_PATH;

    // check for input dir, check its not empty
    if (!std::filesystem::is_directory(base_img_path) ||
        !std::filesystem::is_directory(input_path))
    {
        std::filesystem::create_directories(base_img_path);
        std::filesystem::create_directories(input_path);
        std::stringstream ss;
        ss << "Required image input directories are missing! I've created them for you but you still have no images in ";
        ss << DEFAULT_INPUT_IMG_PATH;
        ss << '!';
        throw std::runtime_error(ERR_FORMAT(ss.str()));
    }

    size_t theme_count = 0;
    for (const auto &entry : std::filesystem::directory_iterator(input_path))
    {
        if (entry.is_directory())
            theme_count++;
    }
    LOG_DEBUG(std::format("Found {} themes in input directory.", theme_count));

    // create loaded and output if not present
    std::filesystem::create_directories(loaded_path);
    std::filesystem::create_directories(output_path);
    Loaded_Img_Dirs.reserve(theme_count);
    // Create sub folders in loaded path based on themes
    for (const auto &entry : std::filesystem::directory_iterator(input_path))
    {
        if (!entry.is_directory())
            continue;
        std::string theme_name = entry.path().stem().string();
        std::filesystem::path theme_subfolder = loaded_path / theme_name;
        std::filesystem::create_directories(theme_subfolder);
        Loaded_Img_Dirs.emplace(theme_name, theme_subfolder);
        LOG_DEBUG(std::format("Created loaded subdirectory for theme: {}", theme_name));
    }
}
