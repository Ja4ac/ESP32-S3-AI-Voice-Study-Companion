#ifndef __LLM_H_
#define __LLM_H_

#include "esp_err.h"
#include "project_secrets.h"
#include <string.h>

#define LLM_BASE_URL          "https://api.deepseek.com/chat/completions"
#define LLM_API_KEY           PROJECT_LLM_API_KEY
#define LLM_SYSTEM_ROLE       "你是专为语音播报设计的助手。规则：禁止使用任何 Markdown 格式符号：*、**、`、~、#、-、=、>、<、[]、() 等一律不许出现。回答简洁、口语化，适合直接转语音播放，不产生多余字符。"

esp_err_t llm_init(void);
esp_err_t llm_chat(const char *text_in, char **text_out);

#endif
