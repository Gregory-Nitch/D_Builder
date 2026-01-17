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
    if (!std::filesystem::exists(base_img_path) ||
        !std::filesystem::exists(input_path) ||
        !std::filesystem::is_directory(base_img_path) ||
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

    if (std::filesystem::is_empty(input_path))
    {
        std::stringstream ss;
        ss << "Image input directory is empty! You'll need to add images too ";
        ss << DEFAULT_INPUT_IMG_PATH;
        ss << '!';
        throw std::runtime_error(ERR_FORMAT(ss.str()));
    }

    // create loaded and output if not present
    std::filesystem::create_directories(loaded_path);
    std::filesystem::create_directories(output_path);
}
