#define LOG_LEVEL_EMERGENCY 1
#define LOG_LEVEL_ALERT 1<<1
#define LOG_LEVEL_CRITICAL 1<<2
#define LOG_LEVEL_ERROR 1<<3
#define LOG_LEVEL_WARNING 1<<4
#define LOG_LEVEL_NOTICE 1<<5
#define LOG_LEVEL_INFO 1<<6
#define LOG_LEVEL_DEBUG 1<<7

void log_emergency(std::string);
void log_alert(std::string);
void log_critical(std::string);
void log_error(std::string);
void log_warning(std::string);
void log_notice(std::string);
void log_info(std::string);
void log_debug(std::string);
void log(std::string, int level);

#define LOG_HANDLER void (*)(std::string, int level)
void log_set_handler(LOG_HANDLER);

#define LOG_LEVEL_ALL 0b11111111
#define LOG_LEVEL_IMPORTANT 0b00001111
void log_set_level(int);
