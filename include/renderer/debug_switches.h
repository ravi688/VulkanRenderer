#pragma once

/* switches */
#define ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH 0
#define ENABLE_DBG_BUFFER2D_RESIZE 1
#define ENABLE_DBG_EVENT_PUBLISH 0

#define DBG_ENABLED(x) (ENABLE_DBG_##x == 1)

#if defined(GLOBAL_DEBUG)
#   if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
#       define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) x
#       define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) , x
#       define NOT_DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#       define NOT_DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#   else
#       define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#       define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#       define NOT_DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) x
#       define NOT_DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) , x
#   endif /* ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH */
#   if DBG_ENABLED(BUFFER2D_RESIZE)
#       define DBG_BUFFER2D_RESIZE(x) x
#       define DBG_ARG_BUFFER2D_RESIZE(x) , x
#       define NOT_DBG_BUFFER2D_RESIZE(x) /* nothing */
#       define NOT_DBG_ARG_BUFFER2D_RESIZE(x) /* nothing */
#   else
#       define DBG_BUFFER2D_RESIZE(x) /* nothing */
#       define DBG_ARG_BUFFER2D_RESIZE(x) /* nothing */
#       define NOT_DBG_BUFFER2D_RESIZE(x) x
#       define NOT_DBG_ARG_BUFFER2D_RESIZE(x) , x
#   endif /* ENABLE_DBG_BUFFER2D_RESIZE */
#   if DBG_ENABLED(EVENT_PUBLISH)
#       define DBG_EVENT_PUBLISH(x) x
#       define DBG_ARG_EVENT_PUBLISH(x) , x
#       define NOT_DBG_EVENT_PUBLISH(x)
#       define NOT_DBG_ARG_EVENT_PUBLISH(x)
#   else
#       define DBG_EVENT_PUBLISH(x)
#       define DBG_ARG_EVENT_PUBLISH(x)
#       define NOT_DBG_EVENT_PUBLISH(x) x
#       define NOT_DBG_ARG_EVENT_PUBLISH(x) , x
#   endif
#endif /* GLOBAL_DEBUG */
