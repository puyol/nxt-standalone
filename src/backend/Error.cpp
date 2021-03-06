// Copyright 2018 The NXT Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "backend/Error.h"

#include "backend/ErrorData.h"

namespace backend {

    ErrorData* MakeError(const char* message, const char* file, const char* function, int line) {
        ErrorData* error = new ErrorData(message);
        error->AppendBacktrace(file, function, line);
        return error;
    }

    void AppendBacktrace(ErrorData* error, const char* file, const char* function, int line) {
        error->AppendBacktrace(file, function, line);
    }

}  // namespace backend
