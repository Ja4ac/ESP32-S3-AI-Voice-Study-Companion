#include "sr_session.h"

#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

static const char *TAG = "SR_SESSION";

static esp_err_t sr_session_copy_in(sr_session_t *session, const int16_t *data, size_t bytes)
{
    if (session == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // 校验缓冲区剩余空间：如果写入后超出最大容量，报错并返回内存不足
    if ((session->length_bytes + bytes) > session->capacity_bytes) {
        ESP_LOGE(TAG, "Audio buffer full, current=%u, append=%u, capacity=%u",
                 (unsigned)session->length_bytes,
                 (unsigned)bytes,
                 (unsigned)session->capacity_bytes);
        return ESP_ERR_NO_MEM;
    }

    // 将新音频数据追加到缓冲区的末尾
    memcpy((uint8_t *)session->buffer + session->length_bytes, data, bytes);
    session->length_bytes += bytes;
    return ESP_OK;
}

esp_err_t sr_session_init(sr_session_t *session, size_t capacity_bytes)
{
    if (session == NULL || capacity_bytes == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(session, 0, sizeof(*session));

    session->buffer = malloc(capacity_bytes);
    if (session->buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate session buffer");
        return ESP_ERR_NO_MEM;
    }

    session->capacity_bytes = capacity_bytes;
    session->initialized = true;
    ESP_LOGI(TAG, "Session initialized, capacity=%u bytes", (unsigned)capacity_bytes);
    return ESP_OK;
}

void sr_session_reset(sr_session_t *session)
{
    if (session == NULL || !session->initialized) {
        return;
    }

    session->length_bytes = 0;
    session->vad_cache_inserted = false;
}

// 向语音会话中插入VAD预缓存音频，避免识别时丢失开头的文字
esp_err_t sr_session_prepend_vad_cache(sr_session_t *session, const int16_t *data, size_t bytes)
{
    esp_err_t err;

    if (session == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!session->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || bytes == 0) {
        return ESP_OK;
    }

    // VAD缓存仅允许插入一次
    if (session->vad_cache_inserted) {
        return ESP_OK;
    }

    err = sr_session_copy_in(session, data, bytes);
    if (err == ESP_OK) {
        session->vad_cache_inserted = true;
    }
    return err;
}

esp_err_t sr_session_append(sr_session_t *session, const int16_t *data, size_t bytes)
{
    if (session == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!session->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || bytes == 0) {
        return ESP_OK;
    }

    return sr_session_copy_in(session, data, bytes);
}

// 检测音频缓冲区是否有音频
bool sr_session_has_audio(const sr_session_t *session)
{
    if (session == NULL || !session->initialized) {
        return false;
    }

    return session->length_bytes > 0;
}

// 将session中的音频复制给上层，session内部缓冲复用
esp_err_t sr_session_clone_audio(const sr_session_t *session, int16_t **out_audio, size_t *out_bytes)
{
    int16_t *audio_copy = NULL;

    if (session == NULL || out_audio == NULL || out_bytes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_audio = NULL;
    *out_bytes = 0;

    if (!session->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (session->length_bytes == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    audio_copy = malloc(session->length_bytes);
    if (audio_copy == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(audio_copy, session->buffer, session->length_bytes);
    *out_audio = audio_copy;
    *out_bytes = session->length_bytes;
    return ESP_OK;
}

void sr_session_deinit(sr_session_t *session)
{
    if (session == NULL) {
        return;
    }

    if (session->buffer != NULL) {
        free(session->buffer);
    }

    memset(session, 0, sizeof(*session));
}
