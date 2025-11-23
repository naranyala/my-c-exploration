#define BETTER_STRING_IMPLEMENTATION
#include "better_string4.h"

int main() {
    b_string sql = bs_new("SELECT * FROM users");
    
    bs_append(&sql, " WHERE id = ");
    bs_append_fmt(&sql, "%d", 42);
    
    printf("Query: %s\n", sql.data); // "SELECT * FROM users WHERE id = 42"
    
    bs_free(&sql);
    return 0;
}
