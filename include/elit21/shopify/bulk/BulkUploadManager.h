#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include <cstddef>
#include <string>
namespace elit21::shopify::bulk {struct BulkUploadFile{std::string path,filename,mime_type,sha256;std::size_t bytes{0};};class BulkUploadManager final:public platform::BusinessComponent{public:BulkUploadManager();Result<BulkUploadFile>inspect(const std::string&path,std::size_t maximum_bytes=100U*1024U*1024U)const;platform::OperationResult execute(const platform::OperationContext&context)override;};}
