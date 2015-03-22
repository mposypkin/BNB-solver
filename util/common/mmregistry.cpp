#include "mmregistry.hpp"

MemManager* MMRegistry::mDefaultManager = NULL;
bool MMRegistry::mConcurrent = false;
pthread_key_t MMRegistry::mMemManagerKey;
