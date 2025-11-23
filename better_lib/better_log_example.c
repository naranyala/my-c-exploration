
#include "better_log.h"

int main() {
    blog_log(BLOG_INFO, "program started");
    blog_log(BLOG_WARN, "value=%d", 42);
    blog_log(BLOG_ERR, "fatal error");
}
