
#include <Renderer/Font.hpp>
#include <renderer/font.h>

namespace V3D
{
	SHOWCASE_API Font::Font(const std::string& filePath)
	{
		handle = font_load_and_create(filePath.c_str());
	}

	SHOWCASE_API void Font::unload() const
	{
		font_destroy(handle);
		font_release_resources(handle);
	}
}
