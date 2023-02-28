#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "zcam:memory";

nvs_handle_t open_storage_handle()
{
    ESP_LOGI(TAG, "opening non-volatile storage (nvs) handle... ");
    nvs_handle_t storage_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &storage_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "error (%s) opening non-volatine storage (nvs) handle...", esp_err_to_name(err));
        return NULL;
    }
    ESP_LOGI(TAG, "done.");
    return storage_handle;
}

void init_memory()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void mem_store_str(const char* key, const char* value)
{
    nvs_handle_t storage_handle = open_storage_handle();

    ESP_LOGI(TAG, "updating %s: %s", key, value);
    esp_err_t err = nvs_set_str(storage_handle, key, value);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "error: %s", esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "committing updates in nvs");
    err = nvs_commit(storage_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "error: %s", esp_err_to_name(err));
    }

    nvs_close(storage_handle);
}

const char* mem_get_str(const char* key)
{
    nvs_handle_t storage_handle = open_storage_handle();

    ESP_LOGI(TAG, "reading %s from nvs...", key);
    size_t key_len;
    esp_err_t err = nvs_get_str(storage_handle, key, NULL, &key_len);
    switch (err)
    {
        case ESP_OK:
            const char* value = malloc(key_len);
            err = nvs_get_str(storage_handle, key, value, &key_len);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "error: %s", esp_err_to_name(err));
            }
            nvs_close(storage_handle);
            return value;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(TAG, "%s has not been initialized in nvs", key);
            break;
        default:
            ESP_LOGE(TAG, "encountered error (%s) while reading %s from nvs.", esp_err_to_name(err), key);
    }
    nvs_close(storage_handle);
    return NULL;
}