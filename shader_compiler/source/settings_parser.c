

#include <shader_compiler/settings_parser.h>
#include <shader_compiler/assert.h> 		// assert()

#include <string.h> 		// string processing functions
#include <ctype.h>			// character functions


static const char* skip_whitespaces(const char* str, const char* const end)
{
	while((str < end) && isspace(*str)) str++;
	return str;
}

static u32 get_word_length(const char* str, const char* const end, const char* delimiters)
{
	u32 count = 0;
	while((str < end) && (!isspace(*str)) && (strchr(delimiters, *str) == NULL)) { str++; count++; }
	return count;
}

static const char* parse(const char* str, u32 length, settings_parser_callbacks_t* callbacks, u32 categoryRank);

SC_API void settings_parse(const char* str, u32 length, settings_parser_callbacks_t* callbacks)
{
	parse(str, length, callbacks, 0);
}


static const char* parse(const char* str, u32 length, settings_parser_callbacks_t* callbacks, u32 categoryRank)
{
	assert(str != NULL);
	assert(callbacks != NULL);

	const char* origin = str;
	const char* const end = origin + length;
	str = skip_whitespaces(str, end);

	if(str >= end)
	{
		if(categoryRank != 0)
			LOG_FETAL_ERR("A category is not closed, expected '}' before the end of file\n");
		return str;
	}

	u32 len = get_word_length(str, end, "{}=,");
	const char* name = str;
	str += len;
	str = skip_whitespaces(str, end);
	switch(*str)
	{
		// start of a category
		case '{':
			if(callbacks->category != NULL)
				callbacks->category(name, len, callbacks->user_data);
			categoryRank++;
		break;

		// end of a category
		case '}':
			if((categoryRank > 0) && (len != 0))
			{
				if(callbacks->field != NULL)
					callbacks->field(name, len, NULL, 0, callbacks->user_data);
			}

			if(categoryRank > 0)
				--categoryRank;
			else
				LOG_FETAL_ERR("Unexpected '}', closing brace '}' must match with opening branch '{'\n");
		break;

		// assignment to a field
		case '=':
			const char* field_name = name;
			++str;
			str = skip_whitespaces(str, end);
			u32 _len = get_word_length(str, end, ",");			// \n is already included
			if(callbacks->field != NULL)
				callbacks->field(field_name, len, str, _len, callbacks->user_data);
			str += _len;
		break;

		// declaration of a field but not assignment
		case ',':
			if(callbacks->field  != NULL)
				callbacks->field(name, len, NULL, 0, callbacks->user_data);
		break;

		case 0:
			LOG_FETAL_ERR("Unexpected end of file\n");
			break;

		default:
			LOG_FETAL_ERR("Expected '{', '=' or ',' before \"%.*s\"\n", len, str);
	}
	++str;
	return parse(str, length - (str - origin), callbacks, categoryRank);
}
