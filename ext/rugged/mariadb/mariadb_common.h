#ifndef __LIBGIT2_MARIADB_COMMON_H
#define __LIBGIT2_MARIADB_COMMON_H

#include <git2.h>
#include <git2/errors.h>
#include <git2/odb_backend.h>
#include <git2/refdb.h>
#include <git2/sys/odb_backend.h>
#include <git2/sys/refdb_backend.h>
#include <git2/types.h>

#include <mysql.h>

#include <stdio.h>


#define MAX_QUERY_LEN 1024 /* without the values */

typedef struct mysql_refdb_backend {
	git_refdb_backend parent;
	MYSQL *db;
	MYSQL_STMT *st_read;
	MYSQL_STMT *st_read_all;
	MYSQL_STMT *st_write;
	MYSQL_STMT *st_delete;
} mysql_refdb_backend;

typedef struct {
	git_odb_backend parent;
	MYSQL *db;
	MYSQL_STMT *st_read;
	MYSQL_STMT *st_write;
	MYSQL_STMT *st_read_header;
} mysql_odb_backend;


int init_statement(MYSQL *db,
    const char *sql_query_short_name,
    const char *sql_statement,
    const char *mysql_table,
    MYSQL_STMT **statement);

int git_refdb_backend_mariadb(git_refdb_backend **backend_out,
    MYSQL *db,
    const char *mariadb_table,
    uint32_t git_repository_id,
    int refdb_partitions);

int git_odb_backend_mariadb(git_odb_backend **backend_out,
    MYSQL *db,
    const char *mariadb_table,
    uint32_t git_repository_id,
    int odb_partitions);

#endif
