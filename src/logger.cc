#include <string>
#include <logger.h>

using namespace std;

int log_level = 0;
void (*log_handler)(string, int) = nullptr;

void log_set_handler(void (*handler)(string, int)) {
    log_handler = handler;
}

void log_set_level(int level) {
    log_level = level;
}

void log(string message, int level) {
    if (log_level & level) {
        (*log_handler)(message, level);
    }
}

void log_emergency(string message) {
    log(message, LOG_LEVEL_EMERGENCY);
}

void log_alert(string message) {
    log(message, LOG_LEVEL_ALERT);
}

void log_critical(string message) {
    log(message, LOG_LEVEL_CRITICAL);
}

void log_error(string message) {
    log(message, LOG_LEVEL_ERROR);
}

void log_warning(string message) {
    log(message, LOG_LEVEL_WARNING);
}

void log_notice(string message) {
    log(message, LOG_LEVEL_NOTICE);
}

void log_info(string message) {
    log(message, LOG_LEVEL_INFO);
}

void log_debug(string message) {
    log(message, LOG_LEVEL_DEBUG);
}
