#pragma once

#include <stddef.h>

void logit(const char *msg);
void logit_hexdump(const char *msg, const unsigned char *data, size_t len);
void hexdump(const unsigned char *data, size_t len);
void hexdump_quiet(const unsigned char *data, size_t len);
