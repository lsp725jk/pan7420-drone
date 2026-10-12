/**
 * ch00_evolution_featmacros.cpp —— 《技术演进总纲》实测脚本 1/4
 */

#include <cstdio>
#include <version>          /**
 * 特性宏都在这里（或各标准库头文件里）
 */

/**
 * 一个"有就打印值，没有就打印未定义"的小宏。 注意：不能用 #if defined(x) 之外的方式测试 —— #ifdef 必须写在外面。
 */
#define SHOW(MACRO)                                                      \
    do {                                                                 \
        std::printf("%-34s = (未定义)\n", #MACRO);                        \
    } while (0)

int main()
{
    std::printf("__cplusplus = %ld\n", (long)__cplusplus);
#ifdef _MSVC_LANG
    std::printf("_MSVC_LANG  = %ld\n", (long)_MSVC_LANG);
#else
    std::printf("_MSVC_LANG  = (非 MSVC，未定义)\n");
#endif

#if defined(__cpp_lib_print)
    std::printf("%-34s = %ld\n", "__cpp_lib_print", (long)__cpp_lib_print);
#else
    SHOW(__cpp_lib_print);
#endif
#if defined(__cpp_lib_expected)
    std::printf("%-34s = %ld\n", "__cpp_lib_expected", (long)__cpp_lib_expected);
#else
    SHOW(__cpp_lib_expected);
#endif
#if defined(__cpp_lib_mdspan)
    std::printf("%-34s = %ld\n", "__cpp_lib_mdspan", (long)__cpp_lib_mdspan);
#else
    SHOW(__cpp_lib_mdspan);
#endif
#if defined(__cpp_lib_format)
    std::printf("%-34s = %ld\n", "__cpp_lib_format", (long)__cpp_lib_format);
#else
    SHOW(__cpp_lib_format);
#endif
#if defined(__cpp_lib_span)
    std::printf("%-34s = %ld\n", "__cpp_lib_span", (long)__cpp_lib_span);
#else
    SHOW(__cpp_lib_span);
#endif
#if defined(__cpp_lib_string_view)
    std::printf("%-34s = %ld\n", "__cpp_lib_string_view", (long)__cpp_lib_string_view);
#else
    SHOW(__cpp_lib_string_view);
#endif
#if defined(__cpp_lib_optional)
    std::printf("%-34s = %ld\n", "__cpp_lib_optional", (long)__cpp_lib_optional);
#else
    SHOW(__cpp_lib_optional);
#endif
#if defined(__cpp_lib_ranges)
    std::printf("%-34s = %ld\n", "__cpp_lib_ranges", (long)__cpp_lib_ranges);
#else
    SHOW(__cpp_lib_ranges);
#endif
#if defined(__cpp_concepts)
    std::printf("%-34s = %ld\n", "__cpp_concepts", (long)__cpp_concepts);
#else
    SHOW(__cpp_concepts);
#endif
#if defined(__cpp_constexpr)
    std::printf("%-34s = %ld\n", "__cpp_constexpr", (long)__cpp_constexpr);
#else
    SHOW(__cpp_constexpr);
#endif
#if defined(__cpp_if_constexpr)
    std::printf("%-34s = %ld\n", "__cpp_if_constexpr", (long)__cpp_if_constexpr);
#else
    SHOW(__cpp_if_constexpr);
#endif
#if defined(__cpp_structured_bindings)
    std::printf("%-34s = %ld\n", "__cpp_structured_bindings", (long)__cpp_structured_bindings);
#else
    SHOW(__cpp_structured_bindings);
#endif
#if defined(__cpp_lib_make_unique)
    std::printf("%-34s = %ld\n", "__cpp_lib_make_unique", (long)__cpp_lib_make_unique);
#else
    SHOW(__cpp_lib_make_unique);
#endif
#if defined(__cpp_rvalue_references)
    std::printf("%-34s = %ld\n", "__cpp_rvalue_references", (long)__cpp_rvalue_references);
#else
    SHOW(__cpp_rvalue_references);
#endif
#if defined(__cpp_lib_three_way_comparison)
    std::printf("%-34s = %ld\n", "__cpp_lib_three_way_comparison", (long)__cpp_lib_three_way_comparison);
#else
    SHOW(__cpp_lib_three_way_comparison);
#endif

    std::printf("\n提示：宏值 YYYYMM 就是该特性定稿的年月，可据此判断它属于哪个标准。\n");
    return 0;
}
