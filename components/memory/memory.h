#ifndef _ZCAM_MEMORY_H_
#define _ZCAM_MEMORY_H_

void init_memory();
void mem_store_str(const char* key, const char* value);
const char* mem_get_str(const char* key);

#endif