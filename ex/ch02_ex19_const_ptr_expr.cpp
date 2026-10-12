/**
 * ch02_ex19_const_ptr_expr.c —— const 在类型化表达式里的读写权限
 */
#include <stdio.h>
#include <string.h>

/**
 * 返回只读字符串的两种风格
 */
static const char *mode_to_text(int m)      /**
 * const 在 * 左边：返回的内容只读
 */
{
    switch (m) {
    case 0:  return "IDLE";
    case 1:  return "ARMED";
    case 2:  return "FLYING";
    default: return "UNKNOWN";
    }
}

/* 想返回"可写"的缓冲区，就让调用者提供（最推荐） */
static void mode_to_buf(int m, char *buf, size_t cap)
{
    if (buf == NULL || cap == 0) {
        return;
    }
    const char *src = mode_to_text(m);
    size_t n = strlen(src);
    if (n >= cap) {
        n = cap - 1;                                       /* 防止溢出，必须留一个字节给 '\0' */
    }
    memcpy(buf, src, n);
    buf[n] = '\0';
}

int main(void)
{
                /* ---- 1. 三种指针声明的读写权限 ---- */
    printf("---- 1. 从名字往外读 ----\n");
    int a = 10, b = 20;
    const int *p1 = &a;                  /* 指向 const int：*p1 不能改，p1 能改 */
    int * const p2 = &a;                 /* const 指针：  *p2 能改，  p2 不能改 */
    const int * const p3 = &a;           /* 两者都不能改 */

    p1 = &b;                             /* 合法：换指向 */
    printf("p1 换指向 b -> *p1 = %d\n", *p1);
    *p2 = 11;                            /* 合法：改值 */
    printf("*p2 = 11 之后 a = %d\n", a);
    printf("*p3 = %d（p3 既不能换指向也不能改值）\n", *p3);
    printf("【记忆】const 在 * 左边 → 值只读；const 在 * 右边 → 指针只读。\n");

                /* ---- 2. 字符串字面量与 const 的关系 ---- */
    printf("---- 2. 字符串字面量 ----\n");
    const char *ro = "hello";               /* 正确：字面量在只读段 */
    printf("const char *ro = \"hello\"  -> %s\n", ro);
    printf("如果你写 char *s = \"hello\"; 然后 s[0]='H'，运行期会崩（只读段）。\n");
    printf("要改就先复制到自己的数组里：\n");
    char buf[16];
    strcpy(buf, ro);                        /* 复制一份可写的副本 */
    buf[0] = 'H';
    printf("复制后修改: %s\n", buf);

                /* ---- 3. const 的传染性：调用者的写法要跟着变 ---- */
    printf("---- 3. 返回 const 指针的影响 ----\n");
    const char *name = mode_to_text(2);
    printf("mode_to_text(2) = %s\n", name);
                /* name[0] = 'x';   <-- 编译错误：name 指向的是 const 内容 */
    printf("【传染性】函数返回 const char*，调用者就不能用 char* 接（会丢 const）。\n");
    printf("          想修改内容，就自己复制一份（见下面的 buf 版本）。\n");

    char mybuf[8];
    mode_to_buf(2, mybuf, sizeof(mybuf));
    mybuf[0] = 'x';                         /* 这是自己的数组，随便改 */
    printf("经 mode_to_buf 拿到自己的副本再改: %s\n", mybuf);

                /* ---- 4. 长度上限必须由调用者给 ---- */
    printf("---- 4. 为什么安全版本要多一个 cap 参数 ----\n");
    char tiny[4];
    mode_to_buf(2, tiny, sizeof(tiny));             /* "FLYING" 太长，会被截断 */
    printf("缓冲区只有 4 字节时得到: \"%s\"（被安全截断，没有溢出）\n", tiny);
    printf("【坑】如果 mode_to_buf 里用 strcpy(buf, src) 而不检查 cap，\n");
    printf("     这一行就会写坏 tiny 后面的栈内存 —— 这就是缓冲区溢出。\n");
    printf("     嵌入式里这类漏洞的后果通常是 HardFault 或随机的诡异行为。\n");
    return 0;
}
