#pragma once

/* switches */
#define ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH 0
#define ENABLE_DBG_BUFFER2D_RESIZE 1


#if defined(GLOBAL_DEBUG)
#   if ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH == 1
#       define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) x
#       define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) , x
#   else
#       define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#       define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#   endif /* ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH */
#   if ENABLE_DBG_BUFFER2D_RESIZE == 1
#       define DBG_BUFFER2D_RESIZE(x) x
#       define DBG_ARG_BUFFER2D_RESIZE(x) , x
#   else
#       define DBG_BUFFER2D_RESIZE(x) /* nothing */
#       define DBG_ARG_BUFFER2D_RESIZE(x) /* nothing */
#   endif /* ENABLE_DBG_BUFFER2D_RESIZE */
#endif /* GLOBAL_DEBUG */

#define DBG_ENABLED(x) (ENABLE_DBG_##x == 1)
