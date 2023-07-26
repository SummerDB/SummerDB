#include "SummerDB/Core/ClientContext.hpp"

#include "SummerDB/Core/Database.hpp"

namespace SummerDB {

ClientContext::ClientContext(SummerDB& database)
    : db(database), transaction(database.transaction_manager) {}

}  // namespace SummerDB