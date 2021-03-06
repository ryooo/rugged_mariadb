#include <rugged.h>
#include "mariadb_common.h"

VALUE rb_mRugged;
VALUE rb_mRuggedMysql;
VALUE rb_cRuggedBackend;
VALUE rb_cRuggedMysqlBackend;

int git_odb__error_ambiguous(char *);
int git_odb__error_notfound(char *);
int git_odb__error_ambiguous(char *);

typedef struct {
	rugged_backend backend;
	char *host;
	int port;
	int repository_id;
	char *socket;
	char *username;
	char *password;
	char *database;
} rugged_mariadb_backend;

static void rb_rugged_mariadb_backend__free(rugged_mariadb_backend * backend)
{
    printf(">> rb_rugged_mariadb_backend__free\n");
    if (backend != NULL) {
		if (backend->host != NULL) {
    	    free(backend->host);
    	}
    		    printf("<< rb_rugged_mysql_backend__free\n");

		if (backend->socket != NULL) {
    	    free(backend->socket);
    	}
    		    printf("<< rb_rugged_mysql_backend__free\n");

		if (backend->username != NULL) {
    	    free(backend->username);
    	}
    		    printf("<< rb_rugged_mysql_backend__free\n");

        if (backend->password != NULL) {
            free(backend->password);
        }
        	    printf("<< rb_rugged_mysql_backend__free\n");

        if (backend->database != NULL) {
            //free(backend->database);
        }
        	    printf("<< rb_rugged_mysql_backend__free\n");

	    // free(backend);
	}
	printf("<< rb_rugged_mariadb_backend__free\n");
}

/*
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
*/
static int
rugged_mariadb__odb_backend(git_odb_backend ** backend_out,
			  rugged_backend * backend)
{
	printf (">> rugged_mariadb__odb_backend\n");
	rugged_mariadb_backend *rugged_backend = (rugged_mariadb_backend *) backend;

    printf (">> database: %s\n", rugged_backend->database);
    printf (">> host: %s\n", rugged_backend->host);

    char* database_name = rugged_backend->database;
    MYSQL *db = mysql_init(NULL);

    if (db == NULL){
        printf( "Error: %s\n", mysql_error( db ) ) ;
    }

    if (mysql_real_connect
        (db, rugged_backend->host, rugged_backend->username, rugged_backend->password, strdup(database_name),
         rugged_backend->port, rugged_backend->socket, 0) != db) {
        printf( "Error in mysql_real_connect: %s\n", mysql_error( db ) ) ;
    	goto error;
    }

    my_bool reconnect = 1;
	if (mysql_options(db, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
	    printf( "Error: %s\n", mysql_error( db ) ) ;
    	goto error;
    }

    if (mysql_select_db(db, strdup(database_name)) != 0) {
        printf( "Error: %s\n", mysql_error( db ) ) ;
    	goto error;
    }

	int ret = git_odb_backend_mariadb(backend_out,
	                 db,
				     "git_odb", rugged_backend->repository_id);
    printf ("<< rugged_mariadb__odb_backend\n");
    return ret;
    error:
    return GIT_EUSER;
}

static int
rugged_mariadb__refdb_backend(git_refdb_backend ** backend_out,
			    rugged_backend * backend)
{
    printf (">> rugged_mariadb__refdb_backend\n");
	rugged_mariadb_backend *rugged_backend = (rugged_mariadb_backend *) backend;

    printf (">> database: %s\n", rugged_backend->database);
    printf (">> host: %s\n", rugged_backend->host);

/*
	char *database;
	uint32_t repository_id;
    int odb_partitions;
    int refdb_partitions;
*/
    char* database_name = rugged_backend->database;
    MYSQL *db = mysql_init(NULL);

    if (db == NULL){
        printf( "Error: %s\n", mysql_error( db ) ) ;
    }

    if (mysql_real_connect
        (db, rugged_backend->host, rugged_backend->username, rugged_backend->password, strdup(database_name),
         rugged_backend->port, rugged_backend->socket, 0) != db) {
        printf( "Error: %s\n", mysql_error( db ) ) ;
    	goto error;
    }

    my_bool reconnect = 1;
	if (mysql_options(db, MYSQL_OPT_RECONNECT, &reconnect) != 0) {
    	goto error;
    }

    if (mysql_autocommit(db, 1) != 0) {
        printf( "Error: %s\n", mysql_error( db ) ) ;
    	goto error;
    }

	int ret = git_refdb_backend_mariadb(backend_out,
                    db,
                    "git_refs", rugged_backend->repository_id);

    printf ("<< rugged_mariadb__refdb_backend\n");
    return ret;
    error:
    return GIT_EUSER;
}



static rugged_mariadb_backend *rugged_mariadb_backend_new(char *host, int port,
						      char *socket,
						      char *username,
						      char *password,
						      char *database,
						      uint32_t repository_id)
{
    printf(">> rugged_mariadb_backend_new\n");
	rugged_mariadb_backend *mysql_backend =
	    malloc(sizeof(rugged_mariadb_backend));

	mysql_backend->backend.odb_backend = rugged_mariadb__odb_backend;
	mysql_backend->backend.refdb_backend = rugged_mariadb__refdb_backend;
    mysql_backend->database = database == NULL ? NULL : strdup(database);
	mysql_backend->host = host == NULL ? NULL : strdup(host);
	mysql_backend->port = port;
	mysql_backend->repository_id = repository_id;
    mysql_backend->socket = socket == NULL ? NULL : strdup(socket);
	mysql_backend->username = username == NULL ? NULL : strdup(username);
	mysql_backend->password = password == NULL ? NULL : strdup(password);

    printf("<< rugged_mariadb_backend_new\n");
	return mysql_backend;
}


/*
Public: Initialize a mysql backend.
opts - hash containing the connection options.
:host - (optional) string, default localhost
:port - (optional) integer, default 3306
:password - (optional) string, default NULL
:socket - (optional) string, default /var/run/mysqld/mysqld.sock
:username - (optional) string, default root
:database - string
*/
static VALUE rb_rugged_mariadb_backend_new(VALUE klass, VALUE rb_opts)
{
    printf(">> rb_rugged_mariadb_backend_new\n");

	VALUE val;
	char *host = NULL;
	char *socket = 0;
	char *database = NULL;
	char *username = "root";
	char *password = "";
	int port = 0;
	uint32_t repository_id = 0;
    int partitions = 0;

	Check_Type(rb_opts, T_HASH);

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("host")))) != Qnil) {
		Check_Type(val, T_STRING);
		host = StringValueCStr(val);
	}

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("port")))) != Qnil) {
		Check_Type(val, T_FIXNUM);
		port = NUM2INT(val);
	}

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("socket")))) != Qnil) {
		Check_Type(val, T_STRING);
		socket = StringValueCStr(val);
	}

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("username")))) != Qnil) {
		Check_Type(val, T_STRING);
		username = StringValueCStr(val);
	}

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("password")))) != Qnil) {
		Check_Type(val, T_STRING);
		password = StringValueCStr(val);
	}

	if ((val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("repository_id")))) != Qnil) {
		Check_Type(val, T_FIXNUM);
		repository_id = NUM2INT(val);
	}

	val = rb_hash_aref(rb_opts, ID2SYM(rb_intern("database")));
	Check_Type(val, T_STRING);
	database = StringValueCStr(val);

	VALUE ret = Data_Wrap_Struct(klass, NULL, rb_rugged_mariadb_backend__free,
				rugged_mariadb_backend_new(host, port, socket,
							 username, password,
							 database, repository_id));
	printf("<< rb_rugged_mariadb_backend_new\n");
	return ret;
}

void Init_rugged_mariadb_backend(void)
{
    printf(">> Init_rugged_mariadb_backend\n");
	rb_cRuggedMysqlBackend = rb_define_class_under(rb_mRuggedMysql, "Backend", rb_cRuggedBackend);
	rb_define_singleton_method(rb_cRuggedMysqlBackend, "new", rb_rugged_mariadb_backend_new, 1);
	printf("<< Init_rugged_mariadb_backend\n");
}

void Init_rugged_mariadb(void)
{
    printf(">> Init_rugged_mariadb\n");
	rb_mRugged = rb_const_get(rb_cObject, rb_intern("Rugged"));
	rb_cRuggedBackend = rb_const_get(rb_mRugged, rb_intern("Backend"));
	rb_mRuggedMysql = rb_const_get(rb_mRugged, rb_intern("MariaDB"));
	Init_rugged_mariadb_backend();
	printf("<< Init_rugged_mariadb\n");
}

