#ifndef __TTS_H_
#define __TTS_H_

#include "esp_err.h"
#include "project_secrets.h"

#include <stdbool.h>

/**
 * TTS 模块职责：
 * 1. 调用方先通过 ASR 或其他来源拿到待播报文本。
 * 2. 调用 tts_speak_text() 把文本上传到豆包语音合成模型。
 * 3. 云端按流式方式返回 base64 音频片段。
 * 4. 模块把音频片段解码成 PCM，并立即写入 I2S 功放输出。
 * 5. 函数在整段语音播放完成后返回。
 *
 * 使用前请先填写以下鉴权与音色配置：
 * - TTS_APP_ID：火山引擎应用 ID
 * - TTS_ACCESS_KEY：火山引擎 Access Key
 * - TTS_RESOURCE_ID：语音合成模型资源 ID，豆包语音合成 2.0 使用 seed-tts-2.0
 * - TTS_SPEAKER：已开通并可用的音色 ID
 */
#define TTS_APP_ID            PROJECT_TTS_APP_ID
#define TTS_ACCESS_KEY        PROJECT_TTS_ACCESS_KEY
#define TTS_RESOURCE_ID       PROJECT_TTS_RESOURCE_ID
#define TTS_API_URL           "https://openspeech.bytedance.com/api/v3/tts/unidirectional/sse"
#define TTS_SPEAKER           PROJECT_TTS_SPEAKER

#define TTS_AUDIO_FORMAT      "pcm"
#define TTS_SAMPLE_RATE       16000
#define TTS_CHANNELS          1
#define TTS_BITS_PER_SAMPLE   16
#define TTS_MAX_TEXT_BYTES    1024

esp_err_t tts_init(void);
bool tts_is_configured(void);
esp_err_t tts_speak_text(const char *text);

#endif
