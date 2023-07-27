#include <cstring>

#include "SummerDB/Common/Types/VectorOperations.hpp"
#include "SummerDB/SummerDB.h"
#include "SummerDB/SummerDB.hpp"

using namespace SummerDB;

summerdb_state summerdb_open(const char* path, summerdb_database* out) {
  SummerDatabase* database = new SummerDatabase(path);
  *out = (summerdb_database)database;
  return SummerDBSuccess;
}

summerdb_state summerdb_close(summerdb_database database) {
  if (database) {
    SummerDatabase* db = (SummerDatabase*)database;
    delete db;
  }
  return SummerDBSuccess;
}

summerdb_state summerdb_connect(summerdb_database database,
                                summerdb_connection* out) {
  SummerDatabase* db = (SummerDatabase*)database;
  SummerDBConnection* connection = new SummerDBConnection(*db);
  *out = (summerdb_connection)connection;
  return SummerDBSuccess;
}

summerdb_state summerdb_disconnect(summerdb_connection connection) {
  if (connection) {
    SummerDBConnection* conn = (SummerDBConnection*)connection;
    delete conn;
  }
  return SummerDBSuccess;
}

static summerdb_type _convert_type_cpp_to_c(TypeId type) {
  switch (type) {
    case TypeId::PARAMETER_OFFSET:
      return SUMMERDB_TYPE_PARAMETER_OFFSET;
    case TypeId::BOOLEAN:
      return SUMMERDB_TYPE_BOOLEAN;
    case TypeId::TINYINT:
      return SUMMERDB_TYPE_TINYINT;
    case TypeId::SMALLINT:
      return SUMMERDB_TYPE_SMALLINT;
    case TypeId::INTEGER:
      return SUMMERDB_TYPE_INTEGER;
    case TypeId::BIGINT:
      return SUMMERDB_TYPE_BIGINT;
    case TypeId::DECIMAL:
      return SUMMERDB_TYPE_DECIMAL;
    case TypeId::POINTER:
      return SUMMERDB_TYPE_POINTER;
    case TypeId::TIMESTAMP:
      return SUMMERDB_TYPE_TIMESTAMP;
    case TypeId::DATE:
      return SUMMERDB_TYPE_DATE;
    case TypeId::VARCHAR:
      return SUMMERDB_TYPE_VARCHAR;
    case TypeId::VARBINARY:
      return SUMMERDB_TYPE_VARBINARY;
    case TypeId::ARRAY:
      return SUMMERDB_TYPE_ARRAY;
    case TypeId::UDT:
      return SUMMERDB_TYPE_UDT;
    default:
      return SUMMERDB_TYPE_INVALID;
  }
}

static TypeId _convert_type_c_to_cpp(summerdb_type type) {
  switch (type) {
    case SUMMERDB_TYPE_PARAMETER_OFFSET:
      return TypeId::PARAMETER_OFFSET;
    case SUMMERDB_TYPE_BOOLEAN:
      return TypeId::BOOLEAN;
    case SUMMERDB_TYPE_TINYINT:
      return TypeId::TINYINT;
    case SUMMERDB_TYPE_SMALLINT:
      return TypeId::SMALLINT;
    case SUMMERDB_TYPE_INTEGER:
      return TypeId::INTEGER;
    case SUMMERDB_TYPE_BIGINT:
      return TypeId::BIGINT;
    case SUMMERDB_TYPE_DECIMAL:
      return TypeId::DECIMAL;
    case SUMMERDB_TYPE_POINTER:
      return TypeId::POINTER;
    case SUMMERDB_TYPE_TIMESTAMP:
      return TypeId::TIMESTAMP;
    case SUMMERDB_TYPE_DATE:
      return TypeId::DATE;
    case SUMMERDB_TYPE_VARCHAR:
      return TypeId::VARCHAR;
    case SUMMERDB_TYPE_VARBINARY:
      return TypeId::VARBINARY;
    case SUMMERDB_TYPE_ARRAY:
      return TypeId::ARRAY;
    case SUMMERDB_TYPE_UDT:
      return TypeId::UDT;
    default:
      return TypeId::INVALID;
  }
}

template <class T>
T get_value(summerdb_column column, size_t index) {
  T* data = (T*)column.data;
  return data[index];
}

static Value _summerdb_c_get_value(summerdb_column column, size_t index) {
  switch (column.type) {
    case SUMMERDB_TYPE_BOOLEAN:
      return Value::BOOLEAN(get_value<int8_t>(column, index));
    case SUMMERDB_TYPE_TINYINT:
      return Value::TINYINT(get_value<int8_t>(column, index));
    case SUMMERDB_TYPE_SMALLINT:
      return Value::SMALLINT(get_value<int16_t>(column, index));
    case SUMMERDB_TYPE_INTEGER:
      return Value::INTEGER(get_value<int32_t>(column, index));
    case SUMMERDB_TYPE_BIGINT:
      return Value::BIGINT(get_value<int64_t>(column, index));
    case SUMMERDB_TYPE_DECIMAL:
      return Value(get_value<double>(column, index));
    case SUMMERDB_TYPE_POINTER:
      return Value::POINTER(get_value<uint64_t>(column, index));
    case SUMMERDB_TYPE_DATE:
      return Value::DATE(get_value<date_t>(column, index));
    case SUMMERDB_TYPE_VARCHAR:
      return Value(std::string(get_value<char*>(column, index)));
    default:
      throw std::runtime_error("Invalid value for C to C++ conversion!");
  }
}

int summerdb_value_is_null(summerdb_column column, size_t index) {
  return column.nullmask[index];
}

const char* summerdb_get_value_str(summerdb_column column, size_t index) {
  Value v = _summerdb_c_get_value(column, index);
  std::string str = v.ToString();
  char* cstr = new char[str.length() + 1];
  std::strcpy(cstr, str.c_str());
  return cstr;
}

void summerdb_print_result(summerdb_result result) {
  // print the result
  // first print the header
  for (size_t i = 0; i < result.column_count; i++) {
    printf(
        "%s\t",
        TypeIdToString(_convert_type_c_to_cpp(result.columns[i].type)).c_str());
  }
  printf(" [ %zu ]\n", result.row_count);
  for (size_t j = 0; j < result.row_count; j++) {
    for (size_t i = 0; i < result.column_count; i++) {
      if (summerdb_value_is_null(result.columns[i], j)) {
        printf("NULL\t");
      } else {
        Value v = _summerdb_c_get_value(result.columns[i], j);
        printf("%s\t", v.ToString().c_str());
      }
    }
    printf("\n");
  }
  printf("\n");
}

summerdb_state summerdb_query(summerdb_connection connection, const char* query,
                              summerdb_result* out) {
  SummerDBConnection* conn = (SummerDBConnection*)connection;
  auto result = conn->Query(query);
  if (!result->GetSuccess()) {
    result->Print();
    return SummerDBError;
  }
  // construct the C result from the C++ result
  if (!out) {
    return SummerDBSuccess;
  }
  out->row_count = result->size();
  out->column_count = result->column_count();
  out->columns =
      (summerdb_column*)malloc(out->column_count * sizeof(summerdb_column));
  if (!out->columns)
    goto mallocfail;
  memset(out->columns, 0, out->column_count * sizeof(summerdb_column));

  for (size_t i = 0; i < out->column_count; i++) {
    auto& types = result->types();

    auto type = types[i];
    auto type_size = GetTypeIdSize(type);
    auto& column = out->columns[i];

    column.type = _convert_type_cpp_to_c(type);
    column.count = result->size();
    column.name = NULL;  // FIXME: don't support names yet
    column.data = (char*)malloc(type_size * result->size());
    column.nullmask = (bool*)malloc(sizeof(bool) * result->size());
    if (!column.data || !column.nullmask)
      goto mallocfail;

    // copy the data
    if (TypeIsConstantSize(type)) {
      char* ptr = column.data;
      for (auto& chunk : result->collection.chunks) {
        auto& vector = chunk->data[i];
        VectorOperations::Copy(vector, ptr);
        ptr += type_size * chunk->count;
      }
    } else {
      // NULL initialize: we are going to do mallocs
      memset(column.data, 0, type_size * result->size());

      if (types[i] == TypeId::VARCHAR) {
        char** dataptr = (char**)column.data;
        for (auto& chunk : result->collection.chunks) {
          auto& vector = chunk->data[i];
          const char** str_data = (const char**)vector.data;
          for (size_t j = 0; j < chunk->count; j++) {
            const char* strptr = str_data[j];
            if (!str_data[j]) {
              strptr = "NULL";
            }
            *dataptr = (char*)malloc(strlen(strptr) + 1);
            if (!*dataptr)
              goto mallocfail;
            strcpy(*dataptr, strptr);
            dataptr++;
          }
        }
      } else {
        // not supported yet
        printf("Copy of non-string varlength values not supported yet!\n");
        goto mallocfail;
      }
    }
    size_t index = 0;
    // set the nullmask
    for (auto& chunk : result->collection.chunks) {
      auto& vector = chunk->data[i];
      for (size_t i = 0; i < vector.count; i++) {
        column.nullmask[index + i] = vector.ValueIsNull(i);
      }
      index += vector.count;
    }
  }
  return SummerDBSuccess;
mallocfail:
  summerdb_destroy_result(*out);
  return SummerDBError;
}

void summerdb_destroy_result(summerdb_result result) {
  if (result.columns) {
    for (size_t i = 0; i < result.column_count; i++) {
      auto& column = result.columns[i];
      if (column.type >= SUMMERDB_TYPE_VARCHAR) {
        // variable length size: delete individual elements
        void** dataptr = (void**)column.data;
        for (size_t j = 0; j < result.row_count; j++) {
          if (dataptr[j]) {
            free(dataptr[j]);
          }
        }
      }
      if (column.data) {
        free(column.data);
      }
      if (column.nullmask) {
        free(column.nullmask);
      }
    }

    free(result.columns);
  }
}
