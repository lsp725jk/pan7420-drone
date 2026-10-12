/**
 * ch09_ex05.c —— 二进制文件：fwrite / fread / fseek / ftell / rewind
 */
#include <stdio.h>
#include <string.h>

/**
 * 故意让结构体有填充字节，好演示"二进制写盘不能跨平台"
 */
struct Record {
    unsigned char  id;                /**
     * 1 字节
     */
                /**
         * 这里编译器会插入 3 字节填充，让下一个 int 4 字节对齐
         */
    int            value;             /**
     * 4 字节
     */
    char           name[8];
};

int main(void)
{
    FILE *fp = tmpfile();                       /* "w+b" 模式的临时文件，自动删除 */
    if (!fp) { perror("tmpfile"); return 1; }

    struct Record w[3] = {
        { 1, 100, "gyro"  },
        { 2, 200, "accel" },
        { 3, 300, "baro"  },
    };

                /* 1) 二进制写 */
    size_t written = fwrite(w, sizeof(w[0]), 3, fp);
    printf("fwrite 写了 %zu 个元素，每个 %zu 字节，结构体总大小 %zu 字节\n",
           written, sizeof(w[0]), sizeof(struct Record));

                /* 2) 定位到文件尾求长度 */
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    printf("文件长度 ftell = %ld 字节 （3 * %zu = %zu）\n",
           len, sizeof(w[0]), 3 * sizeof(w[0]));

                /* 3) 变式：定位回开头再读 —— SEEK_SET / SEEK_CUR / SEEK_END */
    rewind(fp);                                          /* 等价于 fseek(fp, 0, SEEK_SET) */
    struct Record r[3];
    memset(r, 0, sizeof(r));
    size_t got = fread(r, sizeof(r[0]), 3, fp);
    printf("fread 读回 %zu 个：", got);
    for (int k = 0; k < 3; k++)
        printf(" {%u,%d,%s}", r[k].id, r[k].value, r[k].name);
    printf("\n");

                /* 4) 只读中间一条：偏移 = 下标 * 元素大小 */
    fseek(fp, (long)(1 * sizeof(struct Record)), SEEK_SET);
    struct Record one;
    if (fread(&one, sizeof(one), 1, fp) == 1)
        printf("跳过 1 条后读到：{%u,%d,%s}\n", one.id, one.value, one.name);

                /* 5) 变式：SEEK_CUR 相对当前位置前进 */
    fseek(fp, -(long)sizeof(struct Record), SEEK_CUR);
    if (fread(&one, sizeof(one), 1, fp) == 1)
        printf("回退 1 条再读到：{%u,%d,%s}\n", one.id, one.value, one.name);

                /* 6) 坑：结构体大小不等于成员大小之和 */
    printf("sizeof(struct Record) = %zu ，而 1+4+8 = 13 —— 差的就是对齐填充\n",
           sizeof(struct Record));
    printf("填充说明了：二进制文件不能直接跨编译器/跨平台读写\n");

    fclose(fp);
    return 0;
}
