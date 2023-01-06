#pragma once

static const char* keywords[] = 
{
	"",
	"true",
	"false",
	"Shader",
	"Properties",
	"Layout",
	"RenderPass",
	"SubPass",
	"GLSL",
	"GraphicsPipeline"
};

/* keyword enumerations */
enum
{
	KEYWORD_UNDEFINED = 0UL,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_SHADER,
	KEYWORD_PROPERTIES,
	KEYWORD_LAYOUT,
	KEYWORD_RENDERPASS,
	KEYWORD_SUBPASS,
	KEYWORD_GLSL,
	KEYWORD_GFXPIPELINE,
	KEYWORD_MAX
};

/* note: the name of the blocks are also treated as qualifiers */
enum
{
	QUALIFIER_SHADER = KEYWORD_SHADER,
	QUALIFIER_PROPERTIES = KEYWORD_PROPERTIES,
	QUALIFIER_LAYOUT = KEYWORD_LAYOUT,
	QUALIFIER_RENDERPASS = KEYWORD_RENDERPASS,
	QUALIFIER_SUBPASS = KEYWORD_SUBPASS,
	QUALIFIER_GLSL = KEYWORD_GLSL,
	QUALIFIER_GFXPIPELINE = KEYWORD_GFXPIPELINE
};

/* attributes */
static const char* attribute_keywords[] = 
{
	"Name",
	"NoParse",
	"Read",
	"Write"
};

/* attribute enumerations */
enum
{
	ATTRIBUTE_NAME,
	ATTRIBUTE_NOPARSE,
	ATTRIBUTE_READ,
	ATTRIBUTE_WRITE
};
