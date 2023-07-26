#ifndef SUMMERDB_SUMMERDB_H
#define SUMMERDB_SUMMERDB_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DUCKDB_TYPE {
  DUCKDB_TYPE_INVALID = 0,
  DUCKDB_TYPE_PARAMETER_OFFSET,
  DUCKDB_TYPE_BOOLEAN,
  DUCKDB_TYPE_TINYINT,
  DUCKDB_TYPE_SMALLINT,
  DUCKDB_TYPE_INTEGER,
  DUCKDB_TYPE_BIGINT,
  DUCKDB_TYPE_DECIMAL,
  DUCKDB_TYPE_POINTER,
  DUCKDB_TYPE_TIMESTAMP,
  DUCKDB_TYPE_DATE,
  DUCKDB_TYPE_VARCHAR,
  DUCKDB_TYPE_VARBINARY,
  DUCKDB_TYPE_ARRAY,
  DUCKDB_TYPE_UDT
} summerdb_type;

typedef struct {
  summerdb_type type;
  char* data;
  size_t count;
  char* name;
  bool* nullmask;
} summerdb_column;

typedef struct {
  size_t row_count;
  size_t column_count;
  summerdb_column* columns;
} summerdb_result;

typedef void* summerdb_database;
typedef void* summerdb_connection;

typedef enum { SummerDBSuccess = 0, SummerDBError = 1 } summerdb_state;

summerdb_state summerdb_open(
    const char* path,           /* Database filename (UTF-8) */
    summerdb_database* database /* OUT: SummerDB DB handle */
);

summerdb_state summerdb_close(summerdb_database database /* Database to close */
);

summerdb_state summerdb_connect(
    summerdb_database database,     /* Database to open connection to */
    summerdb_connection* connection /* OUT: Connection handle */
);

summerdb_state summerdb_disconnect(
    summerdb_connection connection /* Connection handle */
);

summerdb_state summerdb_query(
    summerdb_connection connection, /* Connection to query */
    const char* query,              /* SQL query to execute */
    summerdb_result* result         /* OUT: query result */
);

//! Returns whether or not a specific value in a specific column is NULL
int summerdb_value_is_null(summerdb_column column, size_t index);

const char* summerdb_get_value_str(summerdb_column column,
                                   size_t index /* Row index */
);

void summerdb_print_result(summerdb_result result /* The result to print */
);

void summerdb_destroy_result(summerdb_result result /* The result to destroy */
);

#ifdef __cplusplus
};
#endif

#endif  // SUMMERDB_SUMMERDB_H