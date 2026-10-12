/**
 * ch08_ex15_local.h —— 放在"和源文件同一个目录"里的头文件，
 */
#ifndef CH08_EX15_LOCAL_H
#define CH08_EX15_LOCAL_H

#define LOCAL_HEADER_VERSION 3

static inline int local_add(int a, int b) { return a + b; }

#endif
