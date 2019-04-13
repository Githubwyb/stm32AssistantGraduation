/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description log file
 */

#ifndef _LOG_H_
#define _LOG_H_

#define DEBUG

#define LOG_HEX(data, length) log_hex(data, length)
#define LOG_BIN(data, length) log_binary(data, length)
#define PRINT(fmt, ...) log_print(fmt, ##__VA_ARGS__)

#ifdef DEBUG

#define FileName(str) splitFileName(str)

#define LOG_DEBUG(fmt, ...)                                                                  \
    log_header(LOG_LEVEL_DEBUG);                                                             \
    log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_WARN(fmt, ...)                                                                   \
    log_header(LOG_LEVEL_WARN);                                                              \
    log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_INFO(fmt, ...)                                                                   \
    log_header(LOG_LEVEL_INFO);                                                              \
    log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_ERROR(fmt, ...)                                                                  \
    log_header(LOG_LEVEL_ERROR);                                                             \
    log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_FATAL(fmt, ...)                                                                  \
    log_header(LOG_LEVEL_FATAL);                                                             \
    log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_PRINT(fmt, ...)        \
    log_print(fmt, ##__VA_ARGS__); \
    log_print("\r\n")

#else

#define LOG_DEBUG(fmt, ...)
#define LOG_WARN(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_FATAL(fmt, ...)
#define LOG_PRINT(fmt, ...)

#endif

typedef enum LOG_LEVEL {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} LOG_LEVEL;

/*
 * @function 以网络结构显示数据
 * @param data 数据首地址
 * @param length 数据长度
 */
void log_hex(const void *data, int length);

/*
 * @function 以二进制显示数据
 * @param data 数据首地址
 * @param length 数据长度
 */
void log_binary(const void *data, int length);

/*
 * @function 相当于printf的打印
 * @param fmt 格式化的数据
 * @param ... 不定变量
 */
void log_print(const char *fmt, ...);

/*
 * @function 打印日志头部信息
 * @param level 日志等级
 */
void log_header(LOG_LEVEL level);

/*
 * @function 切分文件名，将前面的斜杠去除
 * @param fileName 文件名
 */
const char *splitFileName(const char *fileName);

#endif // !_LOG_H_
