#include <stdio.h>
#include <string.h>

#include "mariadb_common.h"

int init_statement(MYSQL *db,
    const char *sql_query_short_name,
    const char *sql_statement,
    const char *mysql_table,
    MYSQL_STMT **statement)
{
    printf(">> init_statement\n");
    printf("SQL:%s\n", sql_statement);
    my_bool truth = 1;
    char sql_query[MAX_QUERY_LEN];

    snprintf(sql_query, sizeof(sql_query), sql_statement, mysql_table);

    *statement = mysql_stmt_init(db);
    if (*statement == NULL) {
        fprintf(stderr, __FILE__ ": mysql_stmt_init() failed");
        return GIT_EUSER;
    }

    if (mysql_stmt_attr_set(*statement, STMT_ATTR_UPDATE_MAX_LENGTH,
            &truth) != 0) {
        fprintf(stderr, __FILE__ ": mysql_stmt_attr_set() failed");
        return GIT_EUSER;
    }

    if (mysql_stmt_prepare(*statement, sql_query, strlen(sql_query)) != 0) {
        fprintf(stderr, __FILE__ ": mysql_stmt_prepare(%s) failed: %s",
            sql_query_short_name,
            mysql_error(db));
        return GIT_EUSER;
    }
    printf("<< init_statement\n");
    return GIT_OK;
}
