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

typedef struct {
    git_odb_backend parent;

    uint32_t git_repository_id;
    char *table;

    MYSQL *db;
    MYSQL_STMT *st_read;
    MYSQL_STMT *st_read_prefix;
    MYSQL_STMT *st_write;
    MYSQL_STMT *st_read_header;
    MYSQL_STMT *st_read_header_prefix;
} mariadb_odb_backend_t;

typedef struct {
    git_refdb_backend parent;

    MYSQL *db;
    uint32_t repository_id;
    char *table;

    MYSQL_STMT *st_exists;
    MYSQL_STMT *st_lookup;
    MYSQL_STMT *st_iterator;
    MYSQL_STMT *st_write_no_force;
    MYSQL_STMT *st_write_force;
    MYSQL_STMT *st_rename;
    MYSQL_STMT *st_delete;
    MYSQL_STMT *st_optimize;
} mariadb_refdb_backend_t;

int init_statement(MYSQL *db,
    const char *sql_query_short_name,
    const char *sql_statement,
    const char *mysql_table,
    MYSQL_STMT **statement);

int git_refdb_backend_mariadb(git_refdb_backend **backend_out,
    MYSQL *db,
    const char *mariadb_table, uint32_t repository_id);

int git_odb_backend_mariadb(git_odb_backend **backend_out,
    MYSQL *db,
    const char *mariadb_table, uint32_t repository_id);

#endif
