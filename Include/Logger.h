#ifndef LOGSLIB_H
    #define LOGSLIB_H

    #ifdef __cplusplus
        extern "C" {
    #endif

    int openLogs(const char *logs_file_name);
    void writeLogs(const char *format, ...);
    int closeLogs();
    void FFlush();
    void printReshetka();
    int FFFFFree(const char *file);

    #define PRINT_LINE printf("[%s:%s:%d]\n", __FILE__, __func__, __LINE__);

    #ifdef __cplusplus
        }
    #endif
    
#endif